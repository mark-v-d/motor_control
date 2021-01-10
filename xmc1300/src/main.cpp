#include "gpio.h"

gpio::output<0,0> DAC;

int main(int argc, char **argv)
{
    DAC.set(gpio::OUTPUT);
    for(;;) {
	DAC=0;
	DAC=1;
    }
    return 0;
}
