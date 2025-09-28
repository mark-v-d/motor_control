#ifndef CRC_LUT
#define CRC_LUT

#include <array>
#include <cstdint>

template <size_t crc_bits, size_t word_bits, uint32_t generator,class T>
constexpr auto crc_make_lut()
{
    static_assert((generator & ~((1<<crc_bits)-1))==0, "Generator incorrect");
    std::array<T,1<<(crc_bits+word_bits)> table;
    for(T state=0; state<(1<<crc_bits); state++) {
	for(int data=0; data<(1<<word_bits); data++) {
	    T crc=state;
	    auto d=data;
	    for(int i=0; i<word_bits; i++, d>>=1) {
		crc<<=1;
		crc|=d&1;
		if(crc&(1<<crc_bits))
		    crc^=generator | (1<<crc_bits);
	    }
	    table[(state<<word_bits)|data]=crc;
	}
    }
    return table;
}

#endif
