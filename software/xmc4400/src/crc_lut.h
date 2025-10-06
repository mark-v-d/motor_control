#ifndef CRC_LUT
#define CRC_LUT

#include <array>
#include <cstdint>

template <size_t crc_bits, uint32_t generator,class T, size_t byte_bits=8>
class crc_lut_t {
    static constexpr auto make_lut()
    {
	static_assert((generator & ~((1<<crc_bits)-1))==0,
	    "Generator incorrect");
	std::array<T,1<<(crc_bits+byte_bits)> table;
	for(T state=0; state<(1<<crc_bits); state++) {
	    for(int data=0; data<(1<<byte_bits); data++) {
		T crc=state;
		auto d=data;
		for(int i=0; i<byte_bits; i++, d>>=1) {
		    crc<<=1;
		    crc|=d&1;
		    if(crc&(1<<crc_bits))
			crc^=generator | (1<<crc_bits);
		}
		table[(state<<byte_bits)|data]=crc;
	    }
	}
	return table;
    }
    constexpr static auto lut=make_lut();
    T crc;
public:
    crc_lut_t(T init=0):crc(init) {}

    void add_bit(int b) {
	crc<<=1;
	crc|=b&1;
	if(crc&0x20)
	    crc^=(generator|0x20);
    }

    void add_byte(uint8_t b) { crc=lut[(crc<<byte_bits)|b]; }

    operator T() const { return crc; }
};

#endif
