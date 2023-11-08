#ifndef DRIVE_PACKET_H
#define DRIVE_PACKET_H

#include "packet.h"

namespace sync_ns {
    struct __attribute__ ((__packed__)) to_host {
	uint32_t tx_seconds;
	uint32_t tx_nanoseconds;
	uint32_t rx_seconds;
	uint32_t rx_nanoseconds;
	int32_t timer;
	float integrator;
    };

    using to_drive=to_host;
}

namespace motion_ns {
    struct  __attribute__ ((__packed__)) to_host {
	int32_t position;
	float angle;
	int32_t valid;
	float Irotor[2];
	float Vrotor[2];
	int32_t glass_counter;
	int32_t glass_index;
	int32_t tpower;
	int32_t offset;
	int32_t ADC[2];
	int32_t invalid;
	int32_t	timer_delta;
	uint32_t rx_counter;
	uint16_t rx_data[16];
    };

    struct  __attribute__ ((__packed__)) to_drive {
	uint32_t new_data;
	float Iset[2];
    };
};

#endif
