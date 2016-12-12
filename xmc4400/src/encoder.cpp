#include "encoder.h"
#include "gpio.h"

void position_MFS13_13(position_t *pos, uint8_t *p)
{
    const float conv=2.0*3.14159265358979/262144.0;
    pos->start=p[0];
    pos->state=p[1];
    pos->encoder=p[2]+256*p[3]+65536*p[4];
    pos->angle=conv*pos->encoder;
    pos->rotation=p[5]+0x100*p[6]+0x10000*p[7];
    pos->crc=0;
    for(int i=0;i<9;i++)
	pos->crc^=p[i];
}

void position_HC_PQ23(position_t *pos, uint8_t *p) 
{
    const float PI=3.14159265358979;
    pos->start=p[0];
    pos->state=p[1];
    pos->encoder=(p[2]+256*p[3]);
    pos->angle=float(2*PI/1024.0)*pos->encoder;
    pos->rotation=p[5]+0x100*p[6];
    pos->crc=0;
    for(int i=0;i<9;i++)
	pos->crc^=p[i];
}
