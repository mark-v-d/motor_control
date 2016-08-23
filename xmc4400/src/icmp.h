namespace packet {

template <typename T>
inline T hton(T i)
{
    union { T b; uint8_t l[sizeof(T)]; } in, out;
    in.b=i;
    for(int x=0;x<sizeof(T);x++)
	out.l[x]=in.l[sizeof(T)-1-x];
    return out.b;
}


struct __attribute__ ((__packed__)) ethernet {
    uint8_t destination[6];
    uint8_t source[6];
    uint16_t type;

    void swap(void) {
	uint8_t t[6];
	memcpy(t,source,6);
	memcpy(source,destination,6);
	memcpy(destination,t,6);
    }
};

struct __attribute__ ((__packed__)) ipv4:public ethernet {
    uint8_t version_headerlength;
    uint8_t services;
    uint16_t length;
    uint16_t id;
    uint16_t flags_fragment_offset; 
    uint8_t ttl;
    uint8_t protocol;
    uint16_t checksum;
    uint32_t source;
    uint32_t destination;

    void swap(void) {
	ethernet::swap();
	uint32_t t=source;
	source=destination;
	destination=t;
    }
};

struct __attribute__ ((__packed__)) icmp:public ipv4 {
    uint8_t type;
    uint8_t code;
    uint16_t checksum;
    uint16_t id;
    uint16_t seq;

    int isEchoRequest(void) {
	return type==8 && ipv4::protocol==1 && ethernet::type==8;
    }
};

}
