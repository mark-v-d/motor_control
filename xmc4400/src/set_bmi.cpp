#include <xmc_device.h>

volatile uint32_t &flash(uint32_t a)
{
    return *reinterpret_cast<volatile uint32_t*>(a);
}

void __attribute__((section(".ram_code"))) ProgramUCB(const uint32_t *data)  
{
    *reinterpret_cast<uint32_t*>(0xc005554)=0xf5;	// clear status
    *reinterpret_cast<uint32_t*>(0xc005554)=0x50;	// enter page mode

    // load page, 256 bytes==64 words.
    for(int i=0;i<64;) {	
	*reinterpret_cast<uint32_t*>(0xc0055f0)=data[i++];
	*reinterpret_cast<uint32_t*>(0xc0055f4)=data[i++];
    }

    *reinterpret_cast<volatile uint32_t*>(0xc005554)=0xaa;
    *reinterpret_cast<volatile uint32_t*>(0xc00aaa8)=0x55;
    *reinterpret_cast<volatile uint32_t*>(0xc005554)=0xc0;
    *reinterpret_cast<volatile uint32_t*>(0xc000900)=0xaa;

    while(FLASH0->FSR&FLASH_FSR_PBUSY_Msk)
	;
    flash(0xc005554)=0xf0;
}

void __attribute__((section(".ram_code"))) EraseUCB(void)
{
    flash(0xc005554)=0xaa;
    flash(0xc00aaa8)=0x55;
    flash(0xc005554)=0x80;
    flash(0xc005554)=0xaa;
    flash(0xc00aaa8)=0x55;
    flash(0xc000800)=0xc0;
    while(FLASH0->FSR&FLASH_FSR_PBUSY_Msk)
	;
    flash(0xc005554)=0xf0;
}

struct __attribute__ ((__packed__)) {
    uint32_t bmi_word;
    uint8_t mac[6];
    uint8_t ipv4[4];
    uint8_t ipv6[16];
    uint8_t usb[4];
    uint32_t xor_check[4];
} bmi={
    .bmi_word=0x70a0,
    .mac={MAC},
    .ipv4={IPV4}
};

volatile uint32_t oper=0;

int main()
{
    for(;;) {
	switch(oper) {
	case 0: continue;
	case 1: EraseUCB(); break;
	case 2: ProgramUCB(reinterpret_cast<uint32_t*>(&bmi)); break;
	}
	oper=0;
    }
}

extern "C" void _exit(int i) { for(;;); }

namespace __gnu_cxx {
/* This removes a lot of code from the binary
*/
void __verbose_terminate_handler(void)
{
    for(;;) { 
    }
}

}
