#ifndef HARDWARE_H
#define HARDWARE_H

#include <chrono>
#include <atomic>
#include <stdint.h>
#include "gpio.h"

constexpr float trigger_HZ=4500.0;

extern std::atomic<uint32_t> sleep_counter;

template <class T>
inline void sleep(T time)
{
    using tick_t=std::chrono::duration<uint32_t,std::ratio<1,int(trigger_HZ)>>;
    auto ticks=std::chrono::duration_cast<tick_t>(time);
    sleep_counter=0;
    while(sleep_counter<ticks.count())
	;
}

#include "gpio.h"

#if (UC_DEVICE == XMC4500)
iopin::input<1,14> BUTTON1;
iopin::output<1,0> LED0;
iopin::output<1,1> LED1;
iopin::output<1,2> LED2;
iopin::output<1,3> LED3;
iopin::input<15,8> CLK_RMII;
iopin::input<15,9> CRS_DV;
iopin::input<2,2> RXD0;
iopin::input<2,3> RXD1;
iopin::input<2,4> RXER;
iopin::ETH0_MDC<2,7> MDC;
iopin::ETH0_MDO<2,0> MDIO;
iopin::ETH0_TXD0<2,8> TXD0;
iopin::ETH0_TXD1<2,9> TXD1;
iopin::ETH0_TX_EN<2,5> TX_EN;
#else
static iopin::output<0,7> LED0=1;
static iopin::output<0,11> LED1=1;
static iopin::output<0,4> LED2=1;
static iopin::output<0,2> LED3=1;
static iopin::output<0,9> LED4=1;
static iopin::input<2,1> CLK_RMII;
static iopin::input<0,1> CRS_DV;
static iopin::input<2,2> RXD0;
static iopin::input<2,3> RXD1;
static iopin::input<2,4> RXER;
static iopin::ETH0_MDC<2,7> MDC;
static iopin::ETH0_MDO<2,0> MDIO;
static iopin::ETH0_TXD0<2,8> TXD0;
static iopin::ETH0_TXD1<2,9> TXD1;
static iopin::ETH0_TX_EN<2,5> TX_EN;
static iopin::output<1,0> ETH_RESET=1;

static iopin::CCU80_OUT20<0,3> HB1;
static iopin::CCU80_OUT00<0,5> HB0;
static iopin::CCU80_OUT30<0,6> HB2;
static iopin::output<0,10> HBEN=1;

static iopin::output<0,8> ENC_5V=0;
static iopin::output<1,9> ENC_12V=0;
static iopin::output<1,15> ENC_DIR=0;
static iopin::U0C0_DOUT0<1,5> ENC_TXD; // FIXME, HWCTRL should only be used fo SSI
static iopin::input<1,4> ENC_RXD;
static iopin::input<0,0> ENC_RXD2;

static iopin::input<14,7> ENC_SIN;
static iopin::input<14,6> ENC_COS;

static iopin::input<2,6> MDAT;
static iopin::DSD_MCLK1<1,8> MCLK;

static iopin::input<1,3> ENC_A;
static iopin::input<1,2> ENC_B;
static iopin::input<1,1> ENC_Z;

#endif

#endif
