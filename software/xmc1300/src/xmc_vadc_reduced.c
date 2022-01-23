#include "xmc_vadc.h"

// Copied because we cannot include the full xmc_vadc.c

#if (XMC_VADC_GROUP_AVAILABLE == 1U)

#if (XMC_VADC_MAXIMUM_NUM_GROUPS == 4U)
static VADC_G_TypeDef *const g_xmc_vadc_group_array[XMC_VADC_MAXIMUM_NUM_GROUPS] = {(VADC_G_TypeDef *)(void *)VADC_G0,
                                                                                    (VADC_G_TypeDef *)(void *)VADC_G1,
                                                                                    (VADC_G_TypeDef *)(void *)VADC_G2,
                                                                                    (VADC_G_TypeDef *)(void *)VADC_G3
                                                                                   };
#else
static VADC_G_TypeDef *const g_xmc_vadc_group_array[XMC_VADC_MAXIMUM_NUM_GROUPS] = {(VADC_G_TypeDef * )(void *) VADC_G0,
                                                                                    (VADC_G_TypeDef * )(void *)VADC_G1
                                                                                   };
#endif

#endif

/* API to enable startup calibration feature */
void XMC_VADC_GLOBAL_StartupCalibration(XMC_VADC_GLOBAL_t *const global_ptr)
{
#if (XMC_VADC_GROUP_AVAILABLE == 1U)
  uint8_t i;
  VADC_G_TypeDef *group_ptr;
#endif

  XMC_ASSERT("XMC_VADC_GLOBAL_StartupCalibration:Wrong Module Pointer", (global_ptr == VADC))

  global_ptr->GLOBCFG |= (uint32_t)VADC_GLOBCFG_SUCAL_Msk;

#if (XMC_VADC_GROUP_AVAILABLE == 1U)
  /* Loop until all active groups finish calibration */
  for (i = 0U; i < XMC_VADC_MAXIMUM_NUM_GROUPS; i++)
  {
    group_ptr = g_xmc_vadc_group_array[i];
    if ( (group_ptr->ARBCFG) & (uint32_t)VADC_G_ARBCFG_ANONS_Msk)
    {
      /* This group is active. Loop until it finishes calibration */
#if UC_FAMILY == XMC1
      // wait until calibration is started
      while ((group_ptr->ARBCFG & (uint32_t)VADC_G_ARBCFG_CALS_Msk) == 0)
      {
        __NOP();
      }
#endif
      while ((group_ptr->ARBCFG) & (uint32_t)VADC_G_ARBCFG_CAL_Msk)
      {
        __NOP();
      }
    }
  }
#else

  /* Loop until calibration is started */
  while ((((SHS0->SHSCFG) & (uint32_t)SHS_SHSCFG_STATE_Msk) >> (uint32_t)SHS_SHSCFG_STATE_Pos) !=
         XMC_VADC_SHS_START_UP_CAL_ACTIVE  )
  {
    __NOP();
  }
  /* Loop until it finishes calibration */
  while ((((SHS0->SHSCFG) & (uint32_t)SHS_SHSCFG_STATE_Msk) >> (uint32_t)SHS_SHSCFG_STATE_Pos) ==
         XMC_VADC_SHS_START_UP_CAL_ACTIVE )
  {
    __NOP();
  }
#endif
}

void XMC_SCU_CLOCK_UngatePeripheralClock(
    const XMC_SCU_PERIPHERAL_CLOCK_t peripheral
) {
  SCU_CLK->CGATCLR0 |= (uint32_t)peripheral;
  while ((SCU_CLK->CLKCR) & SCU_CLK_CLKCR_VDDC2LOW_Msk)
  {
    /* Wait voltage suply stabilization */
  }
}

