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
};

#endif
