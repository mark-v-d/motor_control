/*
    PQ23,
	30.5mH, 7.5 Ohm @ 400Hz u->v, w open
	23.0mH, 5.2 Ohm @ 400Hz u->v+w
*/

#include <atomic>
#include <complex>

constexpr float pi=acos(-1);

#include "hardware.h"
#include "ethernet.h"
#include "icmp.h"
#include "ccu4.h"
#include "ccu8.h"
#include "udp_struct.h"
#include "udp_sync.h"
#include "bitfields.h"
#include "encoder.h"
#include <arpa/inet.h>

#include "bsl.h"
using namespace std::complex_literals;
using namespace std::chrono_literals;
using std::sqrt;
using std::cos;
using std::sin;
using std::complex;
using std::abs;

std::tuple hr_out{
    hrpwm0::half_bridge(HBH0_HR,HBL0_HR),
    hrpwm0::half_bridge(HBH1_HR,HBL1_HR),
    hrpwm0::half_bridge(HBH2_HR,HBL2_HR)
};

constexpr ccu8::resolution_t pwm_time=1.0s/18000;

uart::full_duplex copro(COPRO_TXD,COPRO_RXD);

std::atomic<uint32_t> sleep_counter(0);

icmpProcessing icmp;

Ethernet eth0;

udp_sync syncer __attribute__((section ("ETH_RAM")));
udp_struct<motion_ns::to_drive,motion_ns::to_host> drive_io __attribute__((section ("ETH_RAM")));

std::array<int16_t,16> rx_data;
std::complex<float> Iset;

constexpr float current_scale=1.0/400;
constexpr std::array<std::complex<float>,3> clarke{
    1.0f,
    -0.5f+0.5if*sqrt(3.0f),
    -0.5f-0.5if*sqrt(3.0f)
};

inline auto space_vector_mapping(std::complex<float> Vstator)
{
    std::array<float,3> out;

    if( (out[0]=imag(-clarke[2]*Vstator))>=0 &&
	(out[1]=imag( clarke[0]*Vstator))>=0
    ) {
	out[2]=0;
    } else if(
	(out[1]=imag(-clarke[1]*Vstator))>=0 &&
	(out[2]=imag( clarke[2]*Vstator))>=0
    ) {
	out[0]=0;
    } else {
	out[0]=imag( clarke[1]*Vstator);
	out[1]=0;
	out[2]=imag(-clarke[0]*Vstator);
    }
    return std::tuple_cat(out);
}

class complex_PI {
public:
    complex<float> integrator;
    float limit=0;
    float P=-1;
    float I=-1e-2;

    complex<float> compute(complex<float> error) {
	auto result=P*error+integrator;
	integrator+=I*error;
	auto output=result;

	if(abs(output)>limit) {
	    output*=limit/abs(output);
	    auto correction=output-result;
	    integrator+=correction*I/P;
	}
	return output;
    }
} Kcurrent;


static volatile int subsample;

class ethernet_pll_t {
    static constexpr float Kp=2e-3;
    static constexpr float Ki=1e-4;
    static constexpr uint32_t setpoint=2;
    static constexpr ccu8::resolution_t limit=1us;
    int32_t error;
    float integrator=0;
    uint32_t old_target_s;
    uint32_t old_target_ns;
    int sub=1;
    uint32_t unlocked=100;

public:
    void compute(int subsample) {
	auto [now_s, now_ns]=eth0.system_time();
	auto [target_s, target_ns]=eth0.target_time();
	if(!syncer.locked(&eth0)) {
	    unlocked=100;
	    return;
	}

	if(subsample!=sub ||
	   old_target_s==target_s && old_target_ns==target_ns)
	    return;
	error=1'000'000'000*(target_s-now_s)+(target_ns-now_ns);
	itm.PORT[8].u32=error;
	ccu8::resolution_t t(Kp*error+integrator);
	integrator+=Ki*error;
	auto old_t=t;
	t=std::min(limit,std::max(-limit,t));
	integrator+=(t-old_t)/1ns*Ki/Kp;
	itm.PORT[10].f=integrator;
	t+=pwm_time;
	itm.PORT[9].u32=t.count();
	std::apply([=](auto ...x) { (x.period(t),...);}, hr_out);
	old_target_s=target_s;
	old_target_ns=target_ns;
	if(unlocked && (error<1000 || error>-1000))
	    unlocked--;
    }

    int32_t timestamp() { return error; }
    bool locked() { return !unlocked; }
} pll;

uint32_t get_timestamp()
{
    return pll.timestamp();
}

extern "C" void CCU80_0_IRQHandler(void)
{
    static_assert(std::get<0>(hr_out).UNIT==0, "Wrong interrupt handler");
    constexpr char data=0x05a;
    copro.tx(data);
    pll.compute(subsample);
    itm.PORT[1].u8=subsample;

    if(++subsample>3) {
	IO0=1;
	encoder->trigger();
	subsample=0;
    }

    FCE_KE2->CFG=0;
    FCE_KE2->CRC=0xffff;
    itm.PORT[0].u16=FCE_KE2->CRC;
    uint16_t d;
    for(int rxd_counter=0; copro->TRBSR & USIC_CH_TRBSR_RBFLVL_Msk;) {
	d>>=8;
	d|=copro->OUTR<<8;
	itm.PORT[0].u8=d>>8;
	rx_data[rxd_counter++/2]=d;
	if(!(rxd_counter&1)) {
	    FCE_KE2->IR=std::byteswap(d);
	    itm.PORT[rxd_counter/2].u16=d;
	    itm.PORT[0].u16=FCE_KE2->CRC;
	}
    }

    itm.PORT[2].u8=syncer.locked(&eth0);
    itm.PORT[3].u8=pll.locked();

    rx_data[0]-=2047;
    rx_data[1]-=2047;

    auto [position, angle, valid]=encoder->get_pav();
    constexpr auto C0=current_scale*(clarke[0]-clarke[2]);
    constexpr auto C1=current_scale*(clarke[1]-clarke[2]);

    auto Istator=C0*float(rx_data[0])+C1*float(rx_data[1]);
    auto rotate=std::polar(1.0f, -angle);
    auto Irotor=rotate*Istator;
    auto Vrotor=Kcurrent.compute(Irotor-(drive_io->Iset[0]+1if*drive_io->Iset[1]));
    auto Vstator=conj(rotate)*Vrotor;
    hr_out=space_vector_mapping(Vstator);

    if(drive_io->new_data) {
	drive_io->new_data=0;
	motion_ns::to_host report;
	report.position=position;
	report.angle=angle;
	report.valid=valid;
	report.Irotor[0]=real(Irotor);
	report.Vrotor[0]=real(Vrotor);
	drive_io.transmit(&eth0,report);
    } else if(drive_io.age(&eth0)>10ms) {
	drive_io->Iset[0]=drive_io->Iset[1]=0;
    }

    std::apply(ccu8::shadow_transfer,hr_out);
    IO0=0;
}

extern "C" void VADC0_G0_0_IRQHandler(void)
{
    LED3=0;
    vadc.G[0].REFCLR=vadc.G[0].REFLAG;
    LED3=1;
}

/* Need to modify startup_XMC... for this to work,
    replace "b ." to "b DefaultHandler"
*/
extern "C" void Default_Handler(void)
{
    itm.PORT[0].u32=SCB->SHCSR;
    itm.PORT[1].u32=SCB->CFSR;
    itm.PORT[1].u32=SCB->BFAR;
    for(uint8_t i=0;i<10;i++)
	itm.PORT[0].u8=i; // make sure the frame on the traceport is finished
    for(;;);
}

volatile uint32_t counter, led, txd=-1, hrpwm_status;
volatile int init_enable=0;

void init_adc(void);
int main()
{
    SystemCoreClockUpdate();
    ETH_RESET=1; ETH_RESET.set(XMC_GPIO_MODE_OUTPUT_PUSH_PULL);
    ENC_5V=0; ENC_5V.set(XMC_GPIO_MODE_OUTPUT_PUSH_PULL);
    ENC_12V=0; ENC_12V.set(XMC_GPIO_MODE_OUTPUT_PUSH_PULL);
    ENC_DIR=0; ENC_DIR.set(XMC_GPIO_MODE_OUTPUT_PUSH_PULL);
    IO7=0; IO7.set(XMC_GPIO_MODE_OUTPUT_PUSH_PULL);	// power enable copro
    IO0=0; IO0.set(XMC_GPIO_MODE_OUTPUT_PUSH_PULL);
    IO3=0; IO3.set(XMC_GPIO_MODE_OUTPUT_PUSH_PULL);

    // Turn traceport on.
    LED0.set(XMC_GPIO_HWCTRL_PERIPHERAL1);
    LED0.set(XMC_GPIO_OUTPUT_STRENGTH_STRONG_SHARP_EDGE);
    LED1.set(XMC_GPIO_HWCTRL_PERIPHERAL1);
    LED1.set(XMC_GPIO_OUTPUT_STRENGTH_STRONG_SHARP_EDGE);
    LED2.set(XMC_GPIO_HWCTRL_PERIPHERAL1);
    LED2.set(XMC_GPIO_OUTPUT_STRENGTH_STRONG_SHARP_EDGE);
    LED3.set(XMC_GPIO_HWCTRL_PERIPHERAL1);
    LED3.set(XMC_GPIO_OUTPUT_STRENGTH_STRONG_SHARP_EDGE);
    LED4.set(XMC_GPIO_HWCTRL_PERIPHERAL1);
    LED4.set(XMC_GPIO_OUTPUT_STRENGTH_STRONG_SHARP_EDGE);
    tpi.CSPSR=8;
    tpi.SPPR=0;
    tpi.FFCR=0;
    dwt.CTRL=0x40010001;
    itm.LAR=0xC5ACCE55;
    itm.TCR=0x0001000d;
    itm.TER=0xffffffff;

    eth0.init( // Hangt zonder ethernet kabel
	0,
	RXD0, RXD1, CLK_RMII, CRS_DV, RXER, TXD0, TXD1, TX_EN, MDC, MDIO,
	&icmp
    );
    /*
    eth0.add_udp_receiver(&poker,ntohs(2));
    */
    syncer.TimestampInit();
    eth0.add_udp_receiver(&drive_io,ntohs(1));
    eth0.add_udp_receiver(&syncer,ntohs(3));

    FCE->CLC=0; // Enable CRC engine

    SysTick_Config(1000000);
    SysTick->CTRL&=~SysTick_CTRL_TICKINT_Msk;
    NVIC_DisableIRQ(SysTick_IRQn);

    //PPB->SCR=1;

    // Start XMC1300
    bsl_init(IO7,COPRO_TXD,COPRO_RXD);
    copro.SetBaudrate(uart::Baudrate(4e6));
    uart::fifo_configure<0,16>(copro);

    HBH0_HR.set(XMC_GPIO_OUTPUT_STRENGTH_STRONG_SHARP_EDGE);
    HBL0_HR.set(XMC_GPIO_OUTPUT_STRENGTH_STRONG_SHARP_EDGE);

    ccu8::init<0>(
	XMC_CCU8_CLOCK_SCU,
	XMC_CCU8_SLICE_MCMS_ACTION_TRANSFER_PR_CR
    );
    hrpwm_status=hrpwm0::init();

    std::apply([](auto& ... hr) {
	(hr.init(1,1), ...);
	(hr.period(pwm_time), ...);
	(hr.deadtime(100ns, 100ns), ...);
	(hr.shadow_transfer_mode(ccu8::TRANSFER_PERIOD), ...);
	((hr=0.25f), ...);
    }, hr_out);

    std::get<0>(hr_out)->ccu8->INTE=CCU8_CC8_INTE_PME_Msk;
    std::get<0>(hr_out)->ccu8->SRS=bitfield<CCU8_CC8_SRS_POSR_Msk>(0);
    NVIC_SetPriority(std::get<0>(hr_out).irq<0>(), 1);
    NVIC_EnableIRQ(std::get<0>(hr_out).irq<0>());

    std::apply(ccu8::shadow_transfer,hr_out);
    std::apply(ccu8::start,hr_out);

    auto old_led=led;

    init_encoder();

    std::atomic_thread_fence(std::memory_order_release);

    for(;;) {
	if(led!=old_led) {
	    old_led=led;
	    LED0=old_led&1;
	    LED1=(old_led>>1)&1;
	    LED2=(old_led>>2)&1;
	    LED3=(old_led>>3)&1;
	}
	counter++;
    }
    return 0;
}


/* This removes a lot of code from the binary
*/
void __gnu_cxx::__verbose_terminate_handler(void)
{
    LED0.set(XMC_GPIO_MODE_OUTPUT_PUSH_PULL);
    LED1.set(XMC_GPIO_MODE_OUTPUT_PUSH_PULL);
    LED2.set(XMC_GPIO_MODE_OUTPUT_PUSH_PULL);
    LED3.set(XMC_GPIO_MODE_OUTPUT_PUSH_PULL);
    for(;;) {
	LED0^=1;
	LED1^=1;
	LED2^=1;
    }
}

void init_adc(void)
{
    using namespace vadc_g_ns;
    /*
	FIXME, no checks or automation.

	Only channel 0 of the ADCs is used. Group 0 is used in queued mode
	only and triggered by the timer at the PWM zero crossing.
	4 results are accumulated in result 1, and put in a fifo to
	be read at result 0.

	   G0CH0	master CUR0
	   G1CH6	ENC_SIN (alias)
	   G2CH0	ENC_COS
	   G3CH2	CUR1 (alias)
    */
    XMC_SCU_CLOCK_UngatePeripheralClock(XMC_SCU_PERIPHERAL_CLOCK_VADC);
    XMC_SCU_RESET_DeassertPeripheralReset(XMC_SCU_PERIPHERAL_RESET_VADC);
    vadc.CLC=0;
    vadc.GLOBCFG=vadc_ns::globcfg_t({{
	.diva=3,
	.dcmsb=0,
	.divd=1,
	.divwc=1
    }}).raw;
    vadc.GLOBICLASS[0]=iclass_t({{
	.stcs=0,	// no additional cycles
	.cms=0		// 12-bit conversion
    }}).raw;

    vadc.G[1].ALIAS=alias_t({{.alias0=6}}).raw;
    vadc.G[3].ALIAS=alias_t({{.alias0=2}}).raw;

    for(int i=0;i<4;i++) { // All channels
	// Channel control
	vadc.G[i].CHCTR[0]=chctr_t({{
	    .iclsel=2,		// global class 0
	    .chevmode=0,	// no event
	    .sync=1,		// Synchronised conversion (G0 master)
	    .resreg=1		// Top of 2 entry fifo
	}}).raw;
	vadc.G[i].RCR[0]=rcr_t({{
	    .drctr=0,	// 4 results
	    .dmm=0,	// accumulation
	    .wfr=0,	// overwrite
	    .fen=1,	// part of fifo
	    .srgen=0	// no service request
	}}).raw;
    }
    for(int i: {0,3}) {	// Current measurement channels
	// Current measurement is averaged
	vadc.G[i].RCR[1]=rcr_t({{
	    .drctr=3,	// 4 results
	    .dmm=0,	// accumulation
	    .wfr=0,	// overwrite
	    .fen=0,	// top of fifo
	    .srgen=uint32_t(i==0? 1:0)	// no service request (only master)
	}}).raw;
    }
    for(int i: {1,2}){
	vadc.G[i].RCR[1]=rcr_t({{ // Encoder channels
	    // sincos is not averaged
	    .drctr=0,// 1 results
	    .dmm=0,	// accumulation
	    .wfr=0,	// overwrite
	    .fen=0,	// top of fifo
	    .srgen=0	// no service request
	}}).raw;
    }
    for(int i=1;i<4;i++) {
	// slave
	vadc.G[i].SYNCTR=synctr_t({{
	    .stsel=1,	// synchronise to G0
	}}).raw;
    }
    {
	// master
	int i=0;
	// Arbiter, only queued mode is enabled
	vadc.G[i].ARBPR=arbpr_t({{
	    .prio0=3,
	    .csm0=1,
	    .asen0=1,
	}}).raw;
	vadc.G[i].QMR0=qmr0_t({{
	    .engt=1,
	    .entr=1
	}}).raw;
	vadc.G[i].QINR0=qinr0_t({{
	    .reqchnr=0,
	    .rf=1,
	    .ensi=0,
	    .extr=1,

	}}).raw;
	// FIXME, make the xtsel mapping automatic
	//static_assert(ccu8_ns::unit(HB0)==0, "Wrong timer for ADC trigger");
	vadc.G[i].QCTRL0=qctrl0_t({{
	    .xtsel=8, 	// CCU80::SR2 (See asserts)
	    .xtmode=1,
	    .xtwc=1,
	    .tmen=0,	// Uncertain
	    .tmwc=1
	}}).raw;
	vadc.G[i].SYNCTR=synctr_t({{
	    .stsel=0,	// Master
	    .evalr1=1,
	    .evalr2=1,
	    .evalr3=1
	}}).raw;
    }
    vadc.G[0].ARBCFG=arbcfg_t({{
	.anonc=3,	// permanently on (master/standalone mode)
	.arbrnd=0,	// 4 slots per round
	.arbm=0,	// arbiter runs permanently
	.anons=3	// G0 is the master
    }}).raw;
#if 0
    NVIC_SetPriority(VADC0_G0_0_IRQn,  0);
    NVIC_ClearPendingIRQ(VADC0_G0_0_IRQn);
    NVIC_EnableIRQ(VADC0_G0_0_IRQn);
#endif
}

// XMC_ETH_MAC_InitRxDescriptors
