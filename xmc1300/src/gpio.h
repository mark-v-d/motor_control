#ifndef gpio_h
#define gpio_h

#include <XMC1300.h>

namespace gpio {

enum MODE_t {
    INPUT=0,
    INPUT_PD=1,
    INPUT_PU=2,
    INPUT_XX=3,
    INPUT_INVERT=4,
    INPUT_INVERT_PD=5,
    INPUT_INVERT_PU=6,
    INPUT_INVERT_XX=7,
    OUTPUT=16,
    ALT1=17,
    ALT2=18,
    ALT3=19,
    ALT4=20,
    ALT5=21,
    ALT6=22,
    ALT7=23,
    OUTPUT_OD=24,
    ALT1_OD=25,
    ALT2_OD=26,
    ALT3_OD=27,
    ALT4_OD=28,
    ALT5_OD=29,
    ALT6_OD=30,
    ALT7_OD=31
};

enum HYSTERESIS_t {
    STANDARD_HYST=0,
    LARGE_HYST=1,
};

enum PDIS_t {
    DIGITAL_ENABLE=0,
    DIGITAL_DISABLE=1
};

struct GPIO_padded {
  __IO uint32_t  OUT;          /* Output Register*/
  __O  uint32_t  OMR;          /* Output Modification Register*/
  __I  uint32_t  RESERVED[2];
  __IO uint32_t  IOCR[4];      /* Input/Output Control Register 0*/
  __I  uint32_t  RESERVED1;
  __I  uint32_t  IN;           /* Input Register*/
  __I  uint32_t  RESERVED2[6];
  __IO uint32_t  PHCR[2];      /* Pad Hysteresis Control Register */
  __I  uint32_t  RESERVED3[6];
  __I  uint32_t  PDISC;        /* Pin Function Decision Control Register*/
  __I  uint32_t  RESERVED4[3];
  __IO uint32_t  PPS;          /* Pin Power Save Register*/
  __IO uint32_t  HWSEL;        /* Pin Hardware Select Register*/
    int32_t padding[34];
};

extern GPIO_padded port[3];

template <int port_nr, int pin_nr>
class pin
{
public:
    constexpr static int PORT=port_nr;
    constexpr static int PIN=pin_nr;
    constexpr pin() {
	// We reserve PORT>=100 for internal signals
	static_assert(PORT>=0 && PORT<=15
	    || PORT>=100, "Illegal PORT, should be 0..15");
	static_assert(PIN>=0 && PIN<=15, "Illegal PIN, should be 0..15");
    }

    operator int(void) {
	return (port[PORT].IN>>PIN)&1;
    }

    void set(MODE_t m) {
	auto x=port[PORT].IOCR[PIN/4];
	x&=~(255<<(8*(PIN%4)));
	x|=(m&0x1f)<<(8*(PIN%4)+3);
	port[PORT].IOCR[PIN/4]=x;
    }
    void set(HYSTERESIS_t m) {
	auto x=port[PORT].PHCR[PIN/8];
	x&=~(1<<(4*PIN+2));
	x|=(m&1)<<(4*PIN+2);
	port[PORT].PHCR[PIN/8]=x;
    }
    void set(PDIS_t m) {
	auto x=port[PORT].PDISC;
	x&=~(1<<PIN);
	x|=(m&1)<<PIN;
	port[PORT].PDISC=x;
    }
};

template <int port_nr,int pin_nr>
class output:public pin<port_nr,pin_nr>
{
public:
    constexpr static int PORT=port_nr;
    constexpr static int PIN=pin_nr;
    void operator=(int i) {
	port[PORT].OMR=1<<(PIN + (i? 0:16));
    }
    void toggle(void) { port[PORT].OMR=0x10001<<PIN; }
};

void enable(uint32_t mask);

template <int port_nr, int pin_nr, typename... TAIL>
inline void enable(uint32_t mask,pin<port_nr,pin_nr> head, TAIL ...tail)
{
    enable(mask|(1<<port_nr),tail...);
}

template <int port_nr, int pin_nr, typename... TAIL>
inline void enable(pin<port_nr,pin_nr> head, TAIL ...tail)
{
    enable(1<<port_nr,tail...);
}

}
#endif
