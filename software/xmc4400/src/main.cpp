/*
    PQ23,
	30.5mH, 7.5 Ohm @ 400Hz u->v, w open
	23.0mH, 5.2 Ohm @ 400Hz u->v+w

    FIXME:
	Allow a default state for digout when lock is lost.
*/
#include "vadc.h"

#include <atomic>
#include <complex>

constexpr float pi=acos(-1);

#include "hardware.h"
#include "ethernet.h"
#include "icmp.h"
#include "ccux.h"
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
volatile bool copro_working=false;
int copro_not_working=0;

std::atomic<uint32_t> sleep_counter(0);

icmpProcessing icmp;

Ethernet eth0;

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
    C integrator;
    float P=0.2;
    float I=5e-3;
    float L=1;
public:

    C compute(C error,float voltage) {
	if(voltage<=10.0f) {
	    integrator=0;
	    return 0;
	}
	auto result=(P*error+integrator)/voltage;
	integrator+=I*error;

	C limited{
	    abs(real(result))>real(limit)?
		std::copysign(real(limit),real(result)): real(result),
	    abs(imag(result))>imag(limit)?
		std::copysign(imag(limit),imag(result)): imag(result)
	};

	if(result!=limited) {
	    auto correction=limited-result;
	    integrator+=correction*L;
	}
	return limited;
    }

    void set_limit(C l) { limit=l; }
    void set_gain(float p, float i, float l) { P=p; I=i; L=l; }
};

complex_PI Kcurrent;

static volatile int subsample;

motion_ns::to_host report;
C override=0;
C limit{0.44, 0.8};
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

float sync_Kp=4e-3;
float sync_Ki=5e-6;

constexpr float scale_Vservo=(9900.0+27)/27/4096*3.3;

extern "C" void CCU43_1_IRQHandler()
{

    return;
}
extern "C" void CCU80_2_IRQHandler(void)
{
    static_assert(std::get<0>(hr_out).UNIT==0, "Wrong interrupt handler");
    static uint8_t trace_info;
    static bool overcurrent_latch=false;

    constexpr char data=0x05a;
    if(copro_working)
	copro.tx(data);
    itm.PORT[0].u8=trace_info=subsample;
    if(subsample==1) {
	auto t=syncer.sync(&eth0,50ns,sync_Kp,sync_Ki,20us);
	if(t!=0s) {
	    std::apply([=](auto ...x) { (x.period(t+pwm_time),...);}, hr_out);
	}
	report.timer_delta=t/1ns;
	report.timer_error=syncer.last_error()/1ns;
	encoder->trigger();
	itm.PORT[0].u8=(trace_info|=0x40);
    }
    if(++subsample>3)
	subsample=0;

    FCE_KE2->CFG=0;
    FCE_KE2->CRC=0xffff;
    uint16_t d;
    if(copro_working) {
	for(int rxd_counter=0; copro->TRBSR & USIC_CH_TRBSR_RBFLVL_Msk;) {
	    d>>=8;
	    d|=copro->OUTR<<8;
	    report.rx_data[rxd_counter/2]=rx_data[rxd_counter/2]=d;
	    rxd_counter++;
	    if(!(rxd_counter&1)) {
		FCE_KE2->IR=std::byteswap(d);
	    }
	    report.rx_counter=rxd_counter;
	}
	if(report.rx_counter!=12 && ++copro_not_working>10)
	    copro_working=0;
    } else
	copro_not_working=0;

    float Vservo=rx_data[2]*scale_Vservo+1e-6;

    RELAY0=drive_io->digout&1;
    RELAY1=drive_io->digout&2;
    bool enable=drive_io->digout&drive_io->DRIVE_ENABLE;
    overcurrent_latch&=enable;
    bool overvoltage=Vservo>drive_config->overvoltage;
    enable&=!overcurrent_latch && !overvoltage;

    C setpoint=0;
    bool locked=syncer.locked(&eth0) && drive_io.age(&eth0)<2ms;
    if(drive_io.age(&eth0)<0ms)
	drive_io.clear_timestamp();
    else if(locked && enable) {
	setpoint=drive_io->Iset[0]+1if*drive_io->Iset[1];
	ccu8::clear_trap(hr_out);	// enable outputs
	Kcurrent.set_limit(drive_io->limit[0]+1.0if*drive_io->limit[1]);
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

    auto [position, angle]=encoder->get_position_angle();
    angle+=angle_offset;
    if(angle_override!=0.0f)
	angle=angle_override;
    IO0=report.position==position;
    report.position=position;
    report.index=encoder->index();
    report.angle=angle;
    report.encoder_missing=encoder->get_missing();
    report.encoder_invalid=encoder->get_invalid();

    auto Istator=current_scale*(
	    clarke[0]*float(rx_data[0])+
	    clarke[1]*float(rx_data[1])+
	    clarke[2]*float(rx_data[3]));
    overcurrent_latch|=abs(Istator)>drive_config->overcurrent;
    auto rotate=std::polar(1.0f, angle);
    auto Irotor=rotate*Istator;
    auto Vrotor=Kcurrent.compute(setpoint-Irotor, Vservo);
    auto Vstator=conj(rotate)*Vrotor;
    hr_out=space_vector_mapping(Vstator);

    if constexpr(0) {	// average for reporting
	static C Iavg[4], Irot;
	static int Iavg_counter=0;
	Irot-=Iavg[Iavg_counter];
	Iavg[Iavg_counter]=Irotor;
	Irot+=Iavg[Iavg_counter];
	if(++Iavg_counter>3)
	    Iavg_counter=0;
	Irotor=0.25f*Irot;
    }

    report.Irotor[0]=real(Irotor);
    report.Irotor[1]=imag(Irotor);
    report.Vrotor[0]=real(Vrotor);
    report.Vrotor[1]=imag(Vrotor);
    report.Vservo=Vservo;
    report.position2=glass_scale.count();
    report.index2=glass_scale.index();
    report.tpower=(0xffff&adc::vadc.G[1].RES[1]);
    report.offset=(0xffff&adc::vadc.G[0].RES[1]);
    report.ADC[0]=(0xffff&adc::vadc.G[0].RES[0])-report.offset;
    report.ADC[1]=(0xffff&adc::vadc.G[1].RES[0])-report.offset;
    report.digin=IO1 | (IO2<<1)
	| (encoder->get_age()<<2)
	| (enable? drive_io->DRIVE_ENABLE:0)
	| (overvoltage? drive_io->OVERVOLTAGE:0)
	| (overcurrent_latch? drive_io->OVERCURRENT:0);
    if(locked && subsample==0) {
	itm.PORT[0].u8=(trace_info|=0x80);
	static int32_t old_pos;
	IO3=old_pos==report.position;
	drive_io->new_data=0;
	drive_io.transmit(&eth0,report);
	report.counter++;
	old_pos=report.position;
    } else {
	if(drive_io.age(&eth0)>10ms)
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

auto fan_timer=hrpwm0::center_aligned(FAN);

inline void start_copro()
{
    bsl_init(COPRO_POWER,COPRO_TXD,COPRO_RXD);
    copro.SetBaudrate(uart::Baudrate(4e6));
    uart::fifo_configure<0,16>(copro);
    copro_working=true;
    copro->BYPCR=USIC_CH_BYPCR_BDVTR_Msk |
	bitfield<USIC_CH_BYPCR_BDEN_Msk>(2) |
	bitfield<USIC_CH_BYPCR_BWLE_Msk>(8);
    copro->TBUF[0]=0x05a;
}

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
    IO1.set(XMC_GPIO_MODE_INPUT_PULL_UP);
    IO2.set(XMC_GPIO_MODE_INPUT_PULL_UP);
    RELAY0=0; RELAY0.set(XMC_GPIO_MODE_OUTPUT_PUSH_PULL);
    RELAY1=0; RELAY1.set(XMC_GPIO_MODE_OUTPUT_PUSH_PULL);
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

    TRACESWO.set(XMC_GPIO_HWCTRL_DISABLED);

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

    start_copro();

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

    {
	auto &c=std::get<0>(hr_out)->ccu8;
	c->INTE=CCU8_CC8_INTE_CMU2E_Msk;
	c->SRS=bitfield<CCU8_CC8_SRS_CM2SR_Msk>(2);
	c->CR2S=c->PRS-200;
	NVIC_SetPriority(c.irq<2>(), 1);
	NVIC_EnableIRQ(c.irq<2>());
    }

    std::apply(ccu8::shadow_transfer,hr_out);
    std::apply(ccux::start,hr_out);

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

    fan_timer.init();
    fan_timer.period(100us);
    fan_timer=0.5f;
    ccu8::start(fan_timer);
    ccu8::shadow_transfer(fan_timer);

    //eru::dev1.EXISEL

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
	    Kcurrent.set_gain(drive_config->current_P, drive_config->current_I,
		drive_config->current_L);
	    if(drive_config->encoder&0x40000000)
		glass_scale.negative_index();
	    else
		glass_scale.positive_index();
	    angle_offset=drive_config->angle_offset;
	    drive_config->new_data=0;
	    drive_config.transmit(&eth0,config_ns::to_host{});
	}
	if(drive_config->encoder) {
	    locker_t change_encoder(spinlock);
	    if(change_encoder) {
		set_encoder(drive_config->encoder, drive_config->poles);
		drive_config->encoder=0;
	    }
	}
	if(!copro_working)
	    start_copro();
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
