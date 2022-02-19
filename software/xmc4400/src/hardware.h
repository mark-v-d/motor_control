#ifndef HARDWARE_H
#define HARDWARE_H

#include <chrono>
#include <atomic>
#include <stdint.h>
#include "gpio.h"
#include "ccu8.h"
#include "hrpwm0.h"

constexpr float trigger_HZ=4500.0;

extern std::atomic<uint32_t> sleep_counter;

template <class T>
inline void sleep(T time)
{
    using tick_t=std::chrono::duration<uint32_t,std::ratio<1,int(trigger_HZ)>>;
    auto ticks=std::chrono::duration_cast<tick_t>(time);
    sleep_counter;
    while(sleep_counter<ticks.count())
	;
}

#include "gpio.h"

static gpio::output<3,6> LED0; // TRACED3
static gpio::output<4,0> LED1; // TRACED2
static gpio::output<4,1> LED2; // TRACED1
static gpio::output<1,15> LED3; // TRACED0
static gpio::output<2,14> LED4; // TRACECLK

static gpio::pin<15,8> CLK_RMII;
static gpio::pin<5,2> CRS_DV;
static gpio::pin<5,0> RXD0;
static gpio::pin<5,1> RXD1;
static gpio::pin<0,11> RXER;
static gpio::pin<2,7> MDC;
static gpio::pin<2,0> MDIO;
static gpio::pin<2,8> TXD0;
static gpio::pin<2,9> TXD1;
static gpio::pin<1,12> TX_EN;
static gpio::output<2,10> ETH_RESET;


static hrpwm0::out<0,5> HBL0_HR;
static hrpwm0::out<0,2> HBH0_HR;
static hrpwm0::out<0,8> HBL1_HR;
static hrpwm0::out<0,7> HBH1_HR;
static hrpwm0::out<0,3> HBL2_HR;
static hrpwm0::out<0,4> HBH2_HR;

//static gpio::output<0,10> HBEN;

static gpio::output<1,2> ENC_5V;
static gpio::output<1,3> ENC_12V;
static gpio::output<1,10> ENC_DIR;
static gpio::pin<1,5> ENC_TXD; // FIXME, HWCTRL should only be used fo SSI
static gpio::pin<1,4> ENC_RXD;
//static gpio::pin<0,0> ENC_RXD2;

static gpio::pin<14,7> ENC_SIN;
static gpio::pin<14,6> ENC_COS;

static gpio::pin<2,5> ENC_A;
static gpio::pin<2,4> ENC_B;
static gpio::pin<2,3> ENC_Z;

static gpio::pin<0,0> COPRO_RXD;
static gpio::pin<0,1> COPRO_TXD;

static gpio::output<1,1> IO0;
static gpio::output<3,3> IO7;

/*
    Allocated units:
    CCU80 -> PWM
    CCU40 -> POSIF0
    CCU41 -> POSIF1 (FIXME, do this)
    ERU1,ETLx,OGU2 -> x=pwm.spare_slice(), latch posif counter
*/

#endif
