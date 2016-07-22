#include <initializer_list>

#if defined(__x86_64__)
#include <stdint.h>
#include <stdio.h>
#include <assert.h>

#define __IO

struct GPIO_TypeDef {
  __IO uint32_t MODER;        /*!< GPIO port mode register,                                  Address offset: 0x00 */
  __IO uint16_t OTYPER;       /*!< GPIO port output type register,                           Address offset: 0x04 */
  uint16_t RESERVED0;         /*!< Reserved,                                                                 0x06 */
  __IO uint32_t OSPEEDR;      /*!< GPIO port output speed register,                          Address offset: 0x08 */
  __IO uint32_t PUPDR;        /*!< GPIO port pull-up/pull-down register,                     Address offset: 0x0C */
  __IO uint16_t IDR;          /*!< GPIO port input data register,                            Address offset: 0x10 */
  uint16_t RESERVED1;         /*!< Reserved,                                                                 0x12 */
  __IO uint16_t ODR;          /*!< GPIO port output data register,                           Address offset: 0x14 */
  uint16_t RESERVED2;         /*!< Reserved,                                                                 0x16 */
  __IO uint32_t BSRR;         /*!< GPIO port bit set/reset registerBSRR,                     Address offset: 0x18 */
  __IO uint32_t LCKR;         /*!< GPIO port configuration lock register,                    Address offset: 0x1C */
  __IO uint32_t AFR[2];       /*!< GPIO alternate function low register,                Address offset: 0x20-0x24 */
  __IO uint16_t BRR;          /*!< GPIO bit reset register,                                  Address offset: 0x28 */
  uint16_t RESERVED3;         /*!< Reserved,                                                                 0x2A */

  GPIO_TypeDef(char const *p) { name=p; }
  ~GPIO_TypeDef() {
    printf("%s->MODER=0x%08x\n",name,MODER);
    printf("%s->OTYPER=0x%04x\n",name,OTYPER);
    printf("%s->OSPEEDR=0x%08x\n",name,OSPEEDR);
    printf("%s->PUPDR=0x%08x\n",name,PUPDR);
    printf("%s->AFR[0]=0x%08x\n",name,AFR[0]);
    printf("%s->AFR[1]=0x%08x\n",name,AFR[1]);
  }
private:
    char const *name;
} ;

GPIO_TypeDef A("GPIOA"),B("GPIOB"); 
GPIO_TypeDef *GPIOA=&A;
GPIO_TypeDef *GPIOB=&B;

main()
{
}
#else
#include "stm32f30x.h"
#include "stm32f30x_rcc.h"
#endif

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
    AFa=0xa2,
    AFb=0xb2,
    AFc=0xc2,
    AFd=0xd2,
    AFe=0xe2,
    AFf=0xf2,
    ANA=3
};

struct io_config {
    io_type type;
    io_pullup pupdr;
    io_speed speed;
    io_function function;
};

class io_init {
public:
    io_init(GPIO_TypeDef *p, std::initializer_list<io_config> a) { 
	//assert(a.size()==16);

	uint32_t moder;
	uint16_t otyper;
	uint64_t afr; 
	uint32_t ospeedr, pupdr;
	for(auto t=a.begin();t!=a.end();t++) {
	    moder>>=2; 	moder|=t->function<<30;
	    otyper>>=1;	otyper|=t->type<<15;
	    afr>>=4;	afr|=(uint64_t(t->function&0xf0)<<56);
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
