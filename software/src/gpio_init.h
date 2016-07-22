#ifndef GPIO_INIT_H
#define GPIO_INIT_H
#include <initializer_list>

enum io_type {
    PP=0,
    OD=1
};

enum io_pullup {
    NONE=0,
    PULU=1,
    PULD=2
};


enum io_speed {
    SLOW=0,
    MEDI=1,
    FAST=3
};

enum io_function {
    INP=0,
    OUT=1,
    AF0=0x02,
    AF1=0x12,
    AF2=0x22,
    AF3=0x32,
    AF4=0x42,
    AF5=0x52,
    AF6=0x62,
    AF7=0x72,
    AF8=0x82,
    AF9=0x92,
    AF10=0xa2,
    AF11=0xb2,
    AF12=0xc2,
    AF13=0xd2,
    AF14=0xe2,
    AF15=0xf2,
    ANA=3
};

struct io_config {
    io_type type;
    io_pullup pupdr;
    io_speed speed;
    io_function function;
};

class gpio_init {
public:
    __attribute__((always_inline)) 
    gpio_init(GPIO_TypeDef *p, std::initializer_list<io_config> a) { 
	//assert(a.size()==16);

	uint32_t moder=0;
	uint16_t otyper=0;
	uint64_t afr=0;
	uint32_t ospeedr=0, pupdr=0;
	int i=15;
	for(auto t=a.begin();t!=a.end();t++) {
	    moder>>=2; 	moder|=t->function<<30;
	    otyper>>=1;	otyper|=t->type<<15;
	    afr>>=4;	afr|=uint64_t(t->function>>4)<<60;
	    ospeedr>>=2;ospeedr|=t->speed<<30;
	    pupdr>>=2;	pupdr|=t->pupdr<<30;

	}
	p->MODER=moder;
	p->OTYPER=otyper;
	p->AFR[0]=afr&0xffffffff;
	p->AFR[1]=afr>>32;
	p->OSPEEDR=ospeedr;
	p->PUPDR=pupdr;
    }
};


class gpio {
    GPIO_TypeDef *p;
    int pin;
public:
    gpio(GPIO_TypeDef *ip, int inpin) { p=ip; pin=inpin; }
    int operator=(int i) const { p->BSRR=(1<<(pin+(i? 0:16))); return i; }
};

const gpio LED0(GPIOA,6);
const gpio LED1(GPIOA,7);
const gpio LED2(GPIOB,1);
const gpio LED3(GPIOB,0);
const gpio HBEN(GPIOA,11);


#endif
