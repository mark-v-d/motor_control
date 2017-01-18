#include <algorithm>
#include <arpa/inet.h>

struct __attribute__ ((__packed__)) ethernet_t {
    uint8_t dst_mac[6];
    uint8_t src_mac[6];
    uint16_t type;

};

struct __attribute__ ((__packed__)) ipv4_header_t {
    enum { UDP=0x11 };
    uint8_t version_headerlength;
    uint8_t services;
    uint16_t length;
    uint16_t id;
    uint16_t flags_fragment_offset; 
    uint8_t ttl;
    uint8_t protocol;
    uint16_t ipv4_checksum;
    uint8_t src_ip[4];
    uint8_t dst_ip[4];
};

struct __attribute__ ((__packed__)) ipv4_t:
    public ethernet_t, 
    public ipv4_header_t 
{
};

struct __attribute__ ((__packed__)) icmp_t:public ipv4_t {
    enum { UNREACHABLE=3, ECHO=8 };
    uint8_t type;
    uint8_t code;
    uint16_t icmp_checksum;
    uint16_t id;
    uint16_t seq;
};

struct __attribute__ ((__packed__)) icmp_echo_t:public icmp_t {
    uint8_t payload[56];
};

struct __attribute__ ((__packed__)) icmp_unreachable_t:public icmp_t {
    ipv4_header_t ipv4; 
};

struct __attribute__ ((__packed__)) udp_header_t {
    uint16_t src_port;
    uint16_t dst_port;
    uint16_t udp_length;
    uint16_t checksum;
};

struct __attribute__ ((__packed__)) udp_t:public ipv4_t, public udp_header_t {
};

struct __attribute__ ((__packed__)) icmp_unreachable_udp_t:
    public icmp_unreachable_t
{
    udp_header_t udp;
};

struct __attribute__ ((__packed__)) ptp_v2_t:public udp_t {
    uint8_t	transport_id;
    uint8_t	version;
    uint16_t	ptp_length;
    uint8_t	subdomain;
    uint8_t	reserved;
    uint16_t	flags;
    uint8_t	correction[8]; 	// not aligned
    uint8_t	clock_id[8];	// not aligned
    uint16_t	source_port;
    uint16_t	sequence;
    uint8_t	control;
    uint8_t	log_period;
    uint8_t	seconds[6];
    uint8_t	nano_seconds[4];	// not aligned
    uint16_t	utc_offset;
    uint8_t	priority;
    uint8_t	grandmasterClockClass;
    uint8_t	grandmasterClockAccuracy;
    uint16_t	grandmasterClockVariance;
    uint8_t	priority2;
    uint8_t	grandmasterClockIdentity[8]; 
    uint8_t	localStepsRemoved[2];	// not aligned
    uint8_t	timeSource;
};

struct __attribute__ ((__packed__)) udp_payload_t:public udp_t {
    uint8_t data[1500];
};


union packet {
    ethernet_t ethernet;
    ipv4_t ipv4;
    icmp_t icmp;
    icmp_echo_t icmp_echo;
    icmp_unreachable_t icmp_unreachable;
    icmp_unreachable_udp_t icmp_unreachable_udp;
    udp_t udp;
    // tcp_t tcp;
    ptp_v2_t ptp_v2;
    udp_payload_t udp_payload;
};

inline uint16_t hton(uint16_t i)
{
    return (i>>8)|(i<<8);
}
