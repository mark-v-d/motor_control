#ifndef DRIVE_PACKET_H
#define DRIVE_PACKET_H

#include "packet.h"

namespace sync_ns {
    constexpr uint16_t port=2003;

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
    constexpr uint16_t port=2002;

    struct  __attribute__ ((__packed__)) to_host {
	int32_t position;
	int32_t index;
	float angle;
	int32_t encoder_missing;
	int32_t encoder_invalid;
	float Irotor[2];
	float Vrotor[2];
	int32_t position2;
	int32_t index2;
	float Vservo;
	int32_t tpower;
	int32_t offset;
	int32_t ADC[2];
	int32_t	timer_delta;
	int32_t	timer_error;
	int32_t	counter;
	uint32_t digin;
	uint32_t rx_counter;
	uint16_t rx_data[16];
    };

    struct  __attribute__ ((__packed__)) to_drive {
	static constexpr uint32_t DRIVE_ENABLE=0x80000000;
	static constexpr uint32_t OVERCURRENT=0x40000000;
	static constexpr uint32_t OVERVOLTAGE=0x20000000;
	float Iset[2];
	float limit[2];
	uint32_t digout;
	uint32_t new_data;
    };
};

namespace config_ns {
    constexpr uint16_t port=2004;

    struct  __attribute__ ((__packed__)) to_host {
    };

    struct  __attribute__ ((__packed__)) to_drive {
	int new_data;
	int led;
	int encoder; // 0x40000000 inverted index
	int poles;
	float angle_offset;
	float current_P, current_I, current_L;
	float overvoltage;
	float overcurrent;
    };
};

#endif
