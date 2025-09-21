#ifndef CCUX_H
#define CCUX_H
#include "ccu4.h"
#include "ccu8.h"

namespace ccux {
inline auto start=[](auto& ... x)
{
    if(uint32_t ccu80_gidlc=(((x.TYPE==8 && x.UNIT==0)? (1<<x.SLICE):0) | ...))
	ccu8::dev[0].GIDLC=ccu80_gidlc | CCU8_GIDLC_SPRB_Msk;
    if(uint32_t ccu81_gidlc=(((x.TYPE==8 && x.UNIT==1)? (1<<x.SLICE):0) | ...))
	ccu8::dev[1].GIDLC=ccu81_gidlc | CCU8_GIDLC_SPRB_Msk;
    if(uint32_t ccu40_gidlc=(((x.TYPE==4 && x.UNIT==0)? (1<<x.SLICE):0) | ...))
	ccu4::dev[0].GIDLC=ccu40_gidlc | CCU4_GIDLC_SPRB_Msk;
    if(uint32_t ccu41_gidlc=(((x.TYPE==4 && x.UNIT==1)? (1<<x.SLICE):0) | ...))
	ccu4::dev[1].GIDLC=ccu41_gidlc | CCU4_GIDLC_SPRB_Msk;
    if(uint32_t ccu42_gidlc=(((x.TYPE==4 && x.UNIT==2)? (1<<x.SLICE):0) | ...))
	ccu4::dev[2].GIDLC=ccu42_gidlc | CCU4_GIDLC_SPRB_Msk;
    if(uint32_t ccu43_gidlc=(((x.TYPE==4 && x.UNIT==3)? (1<<x.SLICE):0) | ...))
	ccu4::dev3.GIDLC=ccu43_gidlc | CCU4_GIDLC_SPRB_Msk;

#if UC_FAMILY == XMC1
    constexpr uint32_t mask=
	((x.TYPE==8? SCU_GENERAL_CCUCON_GSC80_Msk:0) | ...) |
	((x.TYPE==4? SCU_GENERAL_CCUCON_GSC40_Msk:0) | ...);
#else
    uint32_t mask=
	((x.UNIT==0 && x.TYPE==8? SCU_GENERAL_CCUCON_GSC80_Msk:0) | ...) |
	((x.UNIT==1 && x.TYPE==8? SCU_GENERAL_CCUCON_GSC81_Msk:0) | ...) |
	((x.UNIT==0 && x.TYPE==4? SCU_GENERAL_CCUCON_GSC40_Msk:0) | ...) |
	((x.UNIT==1 && x.TYPE==4? SCU_GENERAL_CCUCON_GSC41_Msk:0) | ...) |
	((x.UNIT==2 && x.TYPE==4? SCU_GENERAL_CCUCON_GSC42_Msk:0) | ...) |
	((x.UNIT==3 && x.TYPE==4? SCU_GENERAL_CCUCON_GSC43_Msk:0) | ...);
#endif
    SCU_GENERAL->CCUCON|=mask;
    SCU_GENERAL->CCUCON&=~mask;
};
}

#endif
