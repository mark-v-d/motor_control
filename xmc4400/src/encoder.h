#ifndef ENCODER_H
#define ENCODER_H
#include <stdint.h>

/* encoder 4500Hz,
    0 0101 1000 --> 0x1a
    2.5Mb/s
*/

struct position_t {
    uint8_t start;
    uint8_t state;
    uint32_t encoder;
    float angle;
    uint32_t rotation;
    uint8_t crc;
};

void position_MFS13_13(position_t *pos, uint8_t *p);
void position_HC_PQ23(position_t *pos, uint8_t *p);

int init_mitsubishi(void);

#endif // ENCODER_H
