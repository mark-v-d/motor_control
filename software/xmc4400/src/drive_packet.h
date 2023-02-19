#ifndef DRIVE_PACKET_H
#define DRIVE_PACKET_H

#include "packet.h"

namespace sync_ns {
    struct __attribute__ ((__packed__)) to_host:public udp_t {
	uint32_t tx_seconds;
	uint32_t tx_nanoseconds;
	uint32_t rx_seconds;
	uint32_t rx_nanoseconds;
	uint32_t timer;
	float integrator;

	void operator =(udp_t const &o) { udp_t::operator=(o); }
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
    };

    struct  __attribute__ ((__packed__)) to_drive {
	float Iset[2];
    };
};

#endif
