#ifndef MISC_H
#define MISC_H
#include <chrono>
#include <xmc_device.h>

static constexpr int64_t frequency=120000000;

extern SysTick_Type systick;

/* Don't call this from an interrupt, intended for initialisation */
template <class T>
inline void main_sleep(T time)
{
    std::chrono::duration<float,std::ratio<1,frequency>> ticks=time;
    systick.LOAD=ticks.count();
    systick.VAL=0;
    while(!(systick.CTRL&0x10000))
	;
}

template <class T>
class location_t {
    intptr_t addr;
public:
    constexpr location_t(T *p):addr(uint32_t(p)) {}
    constexpr location_t(int a):addr(a) { }
    constexpr operator uint32_t() const { return addr; }
    operator T*() const { return reinterpret_cast<T*>(addr);}
    T* operator->() const { return operator T*(); }
};


#if UC_FAMILY == XMC4
/******************************************************************************
    Registers, so we can examine them in the debugger

    Can't debug with xmc1301 anyway
******************************************************************************/
struct POSIF_PADDED_t:public POSIF_GLOBAL_TypeDef {
  __I  uint32_t  RESERVED6[4031];
};

struct VADC_complete_t:public VADC_GLOBAL_TypeDef
{
    __I uint32_t RESERVED5[3];
    struct group:public VADC_G_TypeDef
    {
	__I uint32_t RESERVED[16];
    } G[4];
};

struct CCU4_complete_t:public CCU4_GLOBAL_TypeDef {
    __I uint32_t RESERVED2[31];
    struct cc4_t:public CCU4_CC4_TypeDef {
	__I uint32_t RESERVED2[19];
    } cc[4];
};

struct DSD_complete_t:public DSD_GLOBAL_TypeDef {
    __I uint32_t RESERVED6[6];
    struct dsd_ch_t:public DSD_CH_TypeDef {
	__I uint32_t RESERVED10[21];
    } ch[4];
};

extern XMC_DMA_t dma0;
extern XMC_USIC_CH_t u0c0;
extern XMC_USIC_CH_t u0c1;
extern XMC_USIC_CH_t u1c0;
extern XMC_USIC_CH_t u1c1;
extern POSIF_PADDED_t posif[2];
extern VADC_complete_t vadc;
extern CCU4_complete_t ccu40;
extern CCU4_complete_t ccu41;
extern CCU4_complete_t ccu42;
extern CCU4_complete_t ccu43;
extern DSD_complete_t dsd;
extern ETH_GLOBAL_TypeDef eth;
#endif

template <uint32_t i>
constexpr uint32_t find_lsb(void)
{
    return i&1? 0:(find_lsb<i/2>()+1);
}

template <>
constexpr uint32_t find_lsb<0>(void)
{
    return 0;
}
template <uint32_t mask>
uint32_t bitfield(uint32_t value)
{
    static_assert(mask!=0, "Invalid bitfield");
    constexpr uint32_t shift=find_lsb<mask>();
    static_assert(shift<31, "Invalid shift");
    return (value<<shift)&mask;
}

#endif
