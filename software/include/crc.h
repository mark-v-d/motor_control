#ifndef CRC_H
#define CRC_H

namespace crc {
template <typename T, T polynome, T init>
class byte {
    static constexpr int shift=8*sizeof(T)-8;
    static constexpr T mask=1<<(8*sizeof(T)-1);
    struct table_t {
	static constexpr T byte(T v, int r) {
	    return r==0? v:byte((v<<1) ^ (v&mask? polynome:0), r-1);
	}
	static constexpr T byte(T v) {
	    v<<=shift;
	    return byte((v<<1) ^ (v&mask? polynome:0), 7);
	}
	std::array<T,256> tbl;
	template<T ...Is>
	constexpr table_t(std::integer_sequence<T, Is...>):tbl{byte(Is)...} {}
	constexpr table_t():table_t(std::make_integer_sequence<T, 256>()) {}
	constexpr T operator[](int i) const { return tbl[i]; }
    };
    static constexpr table_t table{};
    T crc=init;
public:
    void start(void) { crc=init; }
    T compute(uint8_t data) {
	uint8_t index=data^(crc>>shift);
	crc=table[index]^(crc<<8);
	return crc;
    }
    operator T(void) { return crc; }
};

template <typename T, T polynome, T init>
class bit {
    static constexpr int shift=8*sizeof(T)-8;
    static constexpr T mask=1<<(8*sizeof(T)-1);
    T crc=init;
public:
    void start(void) { crc=init; }

    T compute(uint8_t byte) {
	crc^=(T)byte<<shift;
	for(int i=0;i<8;i++)
	    crc=(crc<<1)^((crc&mask)? polynome:0);
	return crc;
    }
    operator T(void) { return crc; }
};
}

#endif // CRC_H
