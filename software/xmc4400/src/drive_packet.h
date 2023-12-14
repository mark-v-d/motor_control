#ifndef DRIVE_PACKET_H
#define DRIVE_PACKET_H

#include "packet.h"

namespace sync_ns {
    constexpr uint16_t port=3;

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
    constexpr uint16_t port=2;

    struct  __attribute__ ((__packed__)) to_host {
	int32_t position;
	float angle;
	int32_t valid;
	float Irotor[2];
	float Vrotor[2];
	int32_t position2;
	int32_t index2;
	float Vservo;
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

namespace config_ns {
    constexpr uint16_t port=4;

    struct  __attribute__ ((__packed__)) to_host {
    };

    struct  __attribute__ ((__packed__)) to_drive {
	int new_data;
	int led;
	int encoder;
	int poles;
	float angle_offset;
	float limit_r, limit_i;
    };
};

#endif
