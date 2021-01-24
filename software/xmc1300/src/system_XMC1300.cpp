#include <XMC1300.h>
#include "system_XMC1300.h"
#include "misc.h"
#include "xmc_scu.h"

#define DCO1_FREQUENCY (64000000U)

uint32_t SystemCoreClock;

extern "C" void SystemInit(void)
{
  SystemCoreSetup();
  SystemCoreClockSetup();
}

extern "C" void SystemCoreSetup(void)
{
    SCU_CLK->CLKCR=0;
}

extern "C" void SystemCoreClockSetup(void)
{
    /* Clock setup done during SSW using the CLOCK_VAL1
	and CLOCK_VAL2 defined in vector table
    */
    SCU_GENERAL->PASSWD=192UL;
    while (((SCU_GENERAL->PASSWD) & SCU_GENERAL_PASSWD_PROTS_Msk))
	; /* Loop until the lock is removed */
    SCU_CLK->CLKCR=0x3ff00000;	// 64MHz
    SystemCoreClockUpdate();
}

extern "C" void SystemCoreClockUpdate(void)
{
    static uint32_t IDIV, FDIV;

    IDIV = ((SCU_CLK->CLKCR) & SCU_CLK_CLKCR_IDIV_Msk) >> SCU_CLK_CLKCR_IDIV_Pos;
    FDIV = ((SCU_CLK->CLKCR) & SCU_CLK_CLKCR_FDIV_Msk) >> SCU_CLK_CLKCR_FDIV_Pos;

    if (IDIV != 0) {
	/* Fractional divider is enabled and used */
	SystemCoreClock = ((DCO1_FREQUENCY << 6U) / ((IDIV << 8) + FDIV)) << 1U;
    } else {
	/* Fractional divider bypassed. Simply divide DCO_DCLK by 2 */
	SystemCoreClock = DCO1_FREQUENCY >> 1U;
    }
}

extern "C" void _init(void) {}
extern "C" void end(void) {}
