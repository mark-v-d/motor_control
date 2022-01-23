#ifndef MISC_H
#define MISC_H
#include <chrono>
#include <xmc_device.h>
#include <xmc_scu.h>

#if UC_FAMILY!=XMC1

struct DWT_Compare_type{
__IO uint32_t comp;      /*!< Offset: 0x020 (R/W)  Comparator Register */
__IO uint32_t mask;      /*!< Offset: 0x024 (R/W)  Mask Register */
__IO uint32_t function;  /*!< Offset: 0x028 (R/W)  Function Register */
   uint32_t RESERVED[1];
};

struct DWT_type
{
  __IO uint32_t CTRL;    /*!< Offset: 0x000 (R/W)  Control Register */
  __IO uint32_t CYCCNT;  /*!< Offset: 0x004 (R/W)  Cycle Count Register */
  __IO uint32_t CPICNT;  /*!< Offset: 0x008 (R/W)  CPI Count Register */
  __IO uint32_t EXCCNT;  /*!< Offset: 0x00C (R/W)  Exception Overhead Count */
  __IO uint32_t SLEEPCNT;/*!< Offset: 0x010 (R/W)  Sleep Count Register */
  __IO uint32_t LSUCNT;  /*!< Offset: 0x014 (R/W)  LSU Count Register */
  __IO uint32_t FOLDCNT; /*!< Offset: 0x018 (R/W)  Folded-instruction Count */
  __I  uint32_t PCSR;    /*!< Offset: 0x01C (R/ )  Program Counter Sample */
  DWT_Compare_type c[4];
};

typedef struct
{
  __OM  union
  {
    __OM  uint8_t    u8;         /*!< 0x000 ( /W)  Stimulus Port 8-bit */
    __OM  uint16_t   u16;        /*!< 0x000 ( /W)  Stimulus Port 16-bit */
    __OM  uint32_t   u32;        /*!< 0x000 ( /W)  Stimulus Port 32-bit */
    __OM  float   f;        /*!< 0x000 ( /W)  Stimulus Port 32-bit */
  }  PORT [32U];                 /*!< 0x000 ( /W)  Stimulus Port Registers */
        uint32_t RESERVED0[864U];
  __IOM uint32_t TER;            /*!< 0xE00 (R/W)  Trace Enable Register */
        uint32_t RESERVED1[15U];
  __IOM uint32_t TPR;            /*!< 0xE40 (R/W)  Trace Privilege Register */
        uint32_t RESERVED2[15U];
  __IOM uint32_t TCR;            /*!< 0xE80 (R/W)  Trace Control Register */
        uint32_t RESERVED3[32U];
        uint32_t RESERVED4[43U];
  __OM  uint32_t LAR;            /*!< 0xFB0 ( /W)  Lock Access Register */
  __IM  uint32_t LSR;            /*!< 0xFB4 (R/ )  Lock Status Register */
        uint32_t RESERVED5[6U];
  __IM  uint32_t PID4;           /*!< 0xFD0 (R/ )  Peripheral Identification Register #4 */
  __IM  uint32_t PID5;           /*!< 0xFD4 (R/ )  Peripheral Identification Register #5 */
  __IM  uint32_t PID6;           /*!< 0xFD8 (R/ )  Peripheral Identification Register #6 */
  __IM  uint32_t PID7;           /*!< 0xFDC (R/ )  Peripheral Identification Register #7 */
  __IM  uint32_t PID0;           /*!< 0xFE0 (R/ )  Peripheral Identification Register #0 */
  __IM  uint32_t PID1;           /*!< 0xFE4 (R/ )  Peripheral Identification Register #1 */
  __IM  uint32_t PID2;           /*!< 0xFE8 (R/ )  Peripheral Identification Register #2 */
  __IM  uint32_t PID3;           /*!< 0xFEC (R/ )  Peripheral Identification Register #3 */
  __IM  uint32_t CID0;           /*!< 0xFF0 (R/ )  Component  Identification Register #0 */
  __IM  uint32_t CID1;           /*!< 0xFF4 (R/ )  Component  Identification Register #1 */
  __IM  uint32_t CID2;           /*!< 0xFF8 (R/ )  Component  Identification Register #2 */
  __IM  uint32_t CID3;           /*!< 0xFFC (R/ )  Component  Identification Register #3 */
} ITM_Type2;


extern DWT_type dwt;
extern ITM_Type2 itm;
extern TPI_Type tpi;
//extern DBGMCU_TypeDef dbgmcu;
#endif

static constexpr int64_t frequency=FREQUENCY;

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

inline void UngateClock(XMC_SCU_PERIPHERAL_CLOCK const &clock)
{
#if defined(CLOCK_GATING_SUPPORTED)
#if UC_FAMILY==XMC1
    SCU_CLK->CGATCLR0 |= (uint32_t)clock;
    while ((SCU_CLK->CLKCR) & SCU_CLK_CLKCR_VDDC2LOW_Msk)
	;
#else
    XMC_SCU_CLOCK_UngatePeripheralClock(XMC_SCU_PERIPHERAL_CLOCK_USIC0);
    while(XMC_SCU_CLOCK_IsPeripheralClockGated(XMC_SCU_PERIPHERAL_CLOCK_USIC0))
	;
#endif
#endif
}

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
constexpr uint32_t bitfield(uint32_t value)
{
    static_assert(mask!=0, "Invalid bitfield");
    constexpr uint32_t shift=find_lsb<mask>();
    static_assert(shift<31, "Invalid shift");
    return (value<<shift)&mask;
}

#endif
