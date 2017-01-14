#ifndef ARPA_INET_H
#define ARPA_INET_H
#include <stdint.h>

inline uint16_t ntohs(uint16_t i)
{
    return (i<<8)|(i>>8);
}

#endif
