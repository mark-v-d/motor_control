/*
    PQ23,
	30.5mH, 7.5 Ohm @ 400Hz u->v, w open
	23.0mH, 5.2 Ohm @ 400Hz u->v+w
*/
#include "vadc.h"

#include <atomic>
#include <complex>

constexpr float pi=acos(-1);

#include "hardware.h"
#include "ethernet.h"
#include "icmp.h"
//#include "ccu4.h"
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
using std::abs;
using C=std::complex<float>;

std::tuple hr_out{
    hrpwm0::half_bridge(HBH0_HR,HBL0_HR),
    hrpwm0::half_bridge(HBH1_HR,HBL1_HR),
    hrpwm0::half_bridge(HBH2_HR,HBL2_HR)
};

uart::full_duplex copro(COPRO_TXD,COPRO_RXD);

std::atomic<uint32_t> sleep_counter(0);

icmpProcessing icmp;

Ethernet eth0;

uint8_t rx_buffer[16];
uint8_t tx_buffer[8];
uint8_t command=0x53;
uint8_t addr=0x40;

udp_sync syncer __attribute__((section ("ETH_RAM")));
udp_struct<motion_ns::to_drive,motion_ns::to_host> drive_io
    __attribute__((section ("ETH_RAM")));
udp_struct<config_ns::to_drive,config_ns::to_host> drive_config
    __attribute__((section ("ETH_RAM")));

std::array<int16_t,16> rx_data;
C Iset;

constexpr float current_scale=27.5/2048;

constexpr std::array<C,3> clarke{
    1.0f,
    -0.5f+0.5if*sqrt(3.0f),
    -0.5f-0.5if*sqrt(3.0f)
};

inline auto space_vector_mapping(C Vstator)
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
    C limit=0;
    C integrator;	// Deze settings z@112.5 rad/s
    float P=0.2;
    float I=5e-3;
    C output;
public:

    C compute(C error) {
	auto result=P*error+integrator;
	integrator+=I*error;

	C limited{
	    abs(real(result))>real(limit)?
		std::copysign(real(limit),real(result)): real(result),
	    abs(imag(result))>imag(limit)?
		std::copysign(imag(limit),imag(result)): imag(result)
	};

	if(result!=limited) {
	    auto correction=limited-result;
	    integrator+=correction*I/P;
	}
	return limited;
    }

    void set_limit(C l) { limit=l; }
};

class complex_ss_2 {
public:
    float a[2][2]={{1, 0},{0,0}};
    float b[2]={0.02222, 0};
    float c[2]={1,0};
    float d=0.5;
    C state[2];
    float K[2]={0.04444,0};
    C limit;

    C compute(C error) {
	C result=d*error+state[0]*c[0]+state[1]*c[1];
	state[0]=a[0][0]*state[0]+a[0][1]*state[1]+b[0]*error;
	state[1]=a[1][0]*state[0]+a[1][1]*state[1]+b[1]*error;

	C limited{
	    abs(real(result))>real(limit)?
		std::copysign(real(limit),real(result)): real(result),
	    abs(imag(result))>imag(limit)?
		std::copysign(imag(limit),imag(result)): imag(result)
	};

	state[0]+=K[0]*(limited-result);
	state[1]+=K[1]*(limited-result);
	return limited;
    }

    void set_limit(C l) { limit=l; }
};


complex_ss_2 Kcurrent;

static volatile int subsample;

motion_ns::to_host report;
C override=0;
C limit{0.1, 0.5};
float angle_offset=0;
float angle_override=0;

std::atomic<int> spinlock;
class locker_t {
    bool result;
    std::atomic<int> *p;
public:
    locker_t(std::atomic<int> &lock) {
	int expected=0;
	result=lock.compare_exchange_weak(expected,1);
	p=&lock;
    }
    ~locker_t() { (*p)=0; }

    operator bool() { return result; }
};

extern "C" void CCU80_2_IRQHandler(void)
{
    static_assert(std::get<0>(hr_out).UNIT==0, "Wrong interrupt handler");
    constexpr char data=0x05a;
    copro.tx(data);
    if(subsample==1) {
	auto t=syncer.sync(&eth0,200ns,20e-3,5e-5);
	if(t!=0s)
	    std::apply([=](auto ...x) { (x.period(t+pwm_time),...);}, hr_out);
	report.timer_delta=t/1ns;
    }
    itm.PORT[1].u8=subsample;

    // 3 is sometimes too late for the ethernet packet
    locker_t encoder_ready(spinlock);
    if(encoder_ready && subsample==2)
	encoder->trigger();
    if(++subsample>3) {
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
	report.rx_data[rxd_counter/2]=rx_data[rxd_counter/2]=d;
	rxd_counter++;
	if(!(rxd_counter&1)) {
	    FCE_KE2->IR=std::byteswap(d);
	    itm.PORT[rxd_counter/2].u16=d;
	    itm.PORT[0].u16=FCE_KE2->CRC;
	}
	report.rx_counter=rxd_counter;
    }

    C setpoint=0;
    if(syncer.locked(&eth0) && drive_io.age(&eth0)<2ms) {
	setpoint=drive_io->Iset[0]+1if*drive_io->Iset[1];
	ccu8::clear_trap(hr_out);	// enable outputs
	Kcurrent.set_limit(limit);
    } else if(abs(override)!=0.0f) {
	setpoint=override;
	ccu8::clear_trap(hr_out);	// enable outputs
	Kcurrent.set_limit(limit);
    } else {
	ccu8::set_trap(hr_out);	// disable outputs
	Kcurrent.set_limit(0.0f);
    }

    rx_data[0]-=2047;
    rx_data[1]-=2047;
    rx_data[3]-=2047;

    auto [position, angle, valid]=encoder->get_pav();
    angle+=angle_offset;
    if(angle_override!=0.0f)
	angle=angle_override;
    if(valid) {
	IO0=report.position==position;
	report.position=position;
	report.angle=angle;
	report.valid=valid;
    } else {
	report.invalid++;
	angle=report.angle;
    }
    constexpr auto C0=current_scale*(clarke[0]-clarke[2]);
    constexpr auto C1=current_scale*(clarke[1]-clarke[2]);

    auto Istator=current_scale*(
	    clarke[0]*float(rx_data[0])+
	    clarke[1]*float(rx_data[1])+
	    clarke[2]*float(rx_data[3]));
    auto rotate=std::polar(1.0f, angle);
    auto Irotor=rotate*Istator;
    auto Vrotor=Kcurrent.compute(setpoint-Irotor);
    auto Vstator=conj(rotate)*Vrotor;
    hr_out=space_vector_mapping(Vstator);

    report.Irotor[0]=real(Irotor);
    report.Irotor[1]=imag(Irotor);
    report.Vrotor[0]=real(Vrotor);
    report.Vrotor[1]=imag(Vrotor);
    report.glass_counter=glass_scale.count();
    report.glass_index=glass_scale.index();
    report.tpower=(0xffff&adc::vadc.G[1].RES[1]);
    report.offset=(0xffff&adc::vadc.G[0].RES[1]);
    report.ADC[0]=(0xffff&adc::vadc.G[0].RES[0])-report.offset;
    report.ADC[1]=(0xffff&adc::vadc.G[1].RES[0])-report.offset;
    itm.PORT[8].u16=report.ADC[0];
    itm.PORT[9].u16=report.ADC[1];
    if(drive_io->new_data) {
	static int32_t old_pos;
	IO3=old_pos==report.position;
	drive_io->new_data=0;
	drive_io.transmit(&eth0,report);
	old_pos=report.position;
    } else if(drive_io.age(&eth0)>10ms) {
	drive_io->Iset[0]=drive_io->Iset[1]=0;
    }

    std::apply(ccu8::shadow_transfer,hr_out);
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
    uint32_t addr;
    asm volatile (
	"ldr.w %0,[sp,0x18];"	// Return address
	: "=r"(addr)
    );
    ccu8::set_trap(hr_out);	// disable outputs
    itm.PORT[0].u32=addr;
    for(uint8_t i=0;i<10;i++)
	itm.PORT[0].u8=i; // make sure the frame on the traceport is finished
    for(;;);
}

volatile uint32_t counter, led, txd=-1, hrpwm_status;
volatile int init_enable=0;

void init_adc(void);
volatile int trap_enable=0;
int main()
{
    SystemCoreClockUpdate();
    ETH_RESET=1; ETH_RESET.set(XMC_GPIO_MODE_OUTPUT_PUSH_PULL);
    ENC_5V=0; ENC_5V.set(XMC_GPIO_MODE_OUTPUT_PUSH_PULL);
    ENC_12V=0; ENC_12V.set(XMC_GPIO_MODE_OUTPUT_PUSH_PULL);
    ENC_DIR=0; ENC_DIR.set(XMC_GPIO_MODE_OUTPUT_PUSH_PULL);
    IO7=0; IO7.set(XMC_GPIO_MODE_OUTPUT_PUSH_PULL);
    IO0=0; IO0.set(XMC_GPIO_MODE_OUTPUT_PUSH_PULL);
    IO3=0; IO3.set(XMC_GPIO_MODE_OUTPUT_PUSH_PULL);
    COPRO_POWER=0; COPRO_POWER.set(XMC_GPIO_MODE_OUTPUT_PUSH_PULL);
    ENC_DDIR=0; ENC_DDIR.set(XMC_GPIO_MODE_OUTPUT_PUSH_PULL);
    ENC_CDIR=0; ENC_CDIR.set(XMC_GPIO_MODE_OUTPUT_PUSH_PULL);
    SUPPLY_VOLTAGE=1; SUPPLY_VOLTAGE.set(XMC_GPIO_MODE_OUTPUT_PUSH_PULL);

    // Turn traceport on.
    TRACECLK.set(XMC_GPIO_HWCTRL_PERIPHERAL1);
    TRACECLK.set(XMC_GPIO_OUTPUT_STRENGTH_STRONG_SHARP_EDGE);
    LED0.set(XMC_GPIO_HWCTRL_PERIPHERAL1);
    LED0.set(XMC_GPIO_OUTPUT_STRENGTH_STRONG_SHARP_EDGE);
    LED1.set(XMC_GPIO_HWCTRL_PERIPHERAL1);
    LED1.set(XMC_GPIO_OUTPUT_STRENGTH_STRONG_SHARP_EDGE);
    LED2.set(XMC_GPIO_HWCTRL_PERIPHERAL1);
    LED2.set(XMC_GPIO_OUTPUT_STRENGTH_STRONG_SHARP_EDGE);
    LED3.set(XMC_GPIO_HWCTRL_PERIPHERAL1);
    LED3.set(XMC_GPIO_OUTPUT_STRENGTH_STRONG_SHARP_EDGE);
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
    eth0.add_udp_receiver(&drive_io,ntohs(motion_ns::port));
    eth0.add_udp_receiver(&syncer,ntohs(sync_ns::port));
    eth0.add_udp_receiver(&drive_config,ntohs(config_ns::port));

    FCE->CLC=0; // Enable CRC engine

    SysTick_Config(1000000);
    SysTick->CTRL&=~SysTick_CTRL_TICKINT_Msk;
    NVIC_DisableIRQ(SysTick_IRQn);

    //PPB->SCR=1;

    // Start XMC1300
    bsl_init(COPRO_POWER,COPRO_TXD,COPRO_RXD);
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
	(hr.enable_trap(), ...);
	((hr=0.25f), ...);
    }, hr_out);

    std::get<0>(hr_out)->ccu8->INTE=CCU8_CC8_INTE_PME_Msk;
    std::get<0>(hr_out)->ccu8->SRS=bitfield<CCU8_CC8_SRS_POSR_Msk>(2);
    NVIC_SetPriority(std::get<0>(hr_out).irq<2>(), 1);
    NVIC_EnableIRQ(std::get<0>(hr_out).irq<2>());

    std::apply(ccu8::shadow_transfer,hr_out);
    std::apply(ccu8::start,hr_out);

    //std::apply([](auto& ... hr) { (hr.enable_trap(), ...); }, hr_out);

    ////////////////////////////////////////////////////////////////////////////
    // ADC
    ////////////////////////////////////////////////////////////////////////////
    adc::init();
    adc::global_class<0>(XMC_VADC_CONVMODE_12BIT,8);
    adc::channel_control<0>(ENC_COS,   XMC_VADC_CHANNEL_CONV_GLOBAL_CLASS0, 0);
    adc::channel_control<0>(AN_IN5,    XMC_VADC_CHANNEL_CONV_GLOBAL_CLASS0, 1);
    adc::channel_control<1>(ENC_SIN_A, XMC_VADC_CHANNEL_CONV_GLOBAL_CLASS0, 0);
    adc::channel_control<1>(TPOWER,    XMC_VADC_CHANNEL_CONV_GLOBAL_CLASS0, 1);
    adc::queue_config<0>( XMC_VADC_GATEMODE_IGNORE, 0,
	XMC_VADC_REQ_TR_CCU80_SR2, XMC_VADC_TRIGGER_EDGE_RISING
    );
    adc::queue_config<1>( XMC_VADC_GATEMODE_IGNORE, 0,
	XMC_VADC_REQ_TR_CCU80_SR2, XMC_VADC_TRIGGER_EDGE_RISING
    );

    for(int i: {0,1}) {
	adc::vadc.G[i].ARBPR=
	    bitfield<VADC_G_ARBPR_PRIO0_Msk>(3) |
	    VADC_G_ARBPR_CSM0_Msk |
	    VADC_G_ARBPR_ASEN0_Msk;
	adc::vadc.G[i].ARBCFG=
	    bitfield<VADC_G_ARBCFG_ANONC_Msk>(3) |
	    bitfield<VADC_G_ARBCFG_ANONS_Msk>(3);
    }
    adc::queue<0>(ENC_COS, adc::EXTERNAL_TRIGGER| adc::REFILL);
    adc::queue<0>(AN_IN5, adc::REFILL);
    adc::queue<1>(ENC_SIN_A, adc::EXTERNAL_TRIGGER| adc::REFILL);
    adc::queue<1>(TPOWER, adc::REFILL);
    ////////////////////////////////////////////////////////////////////////////

    init_encoder();

    std::atomic_thread_fence(std::memory_order_release);

    auto old_led=led;
    for(;;) {
	//asm volatile ("WFI");
	if(led!=old_led) {
	    old_led=led;
	    LED0=old_led&1;
	    LED1=(old_led>>1)&1;
	    LED2=(old_led>>2)&1;
	    LED3=(old_led>>3)&1;
	}
	counter++;
	std::apply([](auto& ... hr) {
	    if(trap_enable&1)
		(hr.disable_trap(), ...);
	    if(trap_enable&2)
		(hr.enable_trap(), ...);
	    if(trap_enable&4)
		(hr.set_trap(), ...);
	    if(trap_enable&8)
		(hr.clear_trap(), ...);
	}, hr_out);
	if(drive_config->new_data) {
	    led=drive_config->led;
	    limit=drive_config->limit_r + 1.0if*drive_config->limit_i;
	    drive_config->new_data=0;
	}
	if(drive_config->encoder) {
	    locker_t change_encoder(spinlock);
	    if(change_encoder) {
		set_encoder(drive_config->encoder, drive_config->poles);
		drive_config->encoder=0;
	    }
	}
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

void debug() {}
