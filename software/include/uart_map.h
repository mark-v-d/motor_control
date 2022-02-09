#if 0
    Inputs translated from xmc4_usic_map.h

:'a,'bv/P\d\+_\d\+\s/d
:'a,'bs/0$/A
:'a,'bs/1$/B
:'a,'bs/2$/C
:'a,'bs/3$/D
:'a,'bs/4$/E
:'a,'bs/5$/F
:'a,'bs/.*USIC\(.\)_C\(.\)_\([^_]\+\)_P\([0-9]\+\)_\([0-9]\+\)[^0-9]\+\([A-F]\+\)/\3<\4,\5>()\t{ return info_t{\1,\2,\3\6,\tXMC_GPIO_MODE_INPUT_TRISTATE};}/

    Outputs translated from xmc4_gpio_map.h

:'a,'bv/_U\dC\d_/d
:'a,'bs/P\([0-9]\+\)_\([0-9]\+\)_AF_U\(.\)C\(.\)_\(\w\+\)\s\+\(\S\+\).*/\5<\1,\2>()\t{ return info_t{\3,\4,OUTPUT,\6};}/

#endif

template <int PORT, int PIN> constexpr info_t dx0(void) { static_assert(PORT==-1, "Not an input pin"); return info_t{-1,-1}; }
template <int PORT, int PIN> constexpr info_t dx1(void) { static_assert(PORT==-1, "Not an input pin"); return info_t{-1,-1}; }
template <int PORT, int PIN> constexpr info_t dx2(void) { static_assert(PORT==-1, "Not an input pin"); return info_t{-1,-1}; }
template <int PORT, int PIN> constexpr info_t dx3(void) { static_assert(PORT==-1, "Not an input pin"); return info_t{-1,-1}; }
template <int PORT, int PIN> constexpr info_t dx4(void) { static_assert(PORT==-1, "Not an input pin"); return info_t{-1,-1}; }
template <int PORT, int PIN> constexpr info_t dx5(void) { static_assert(PORT==-1, "Not an input pin"); return info_t{-1,-1}; }

#define DX0 template<> constexpr info_t dx0
#define DX1 template<> constexpr info_t dx1
#define DX2 template<> constexpr info_t dx2
#define DX3 template<> constexpr info_t dx3
#define DX4 template<> constexpr info_t dx4
#define DX5 template<> constexpr info_t dx5

template <int PORT, int PIN> constexpr info_t dout0(void) { static_assert(PORT==-1, "Not an dout0 pin"); return info_t{-1,-1}; }
template <int PORT, int PIN> constexpr info_t mclkout(void) { static_assert(PORT==-1, "Not an mclkout pin"); return info_t{-1,-1}; }
template <int PORT, int PIN> constexpr info_t sclkout(void) { static_assert(PORT==-1, "Not an sclkout pin"); return info_t{-1,-1}; }
template <int PORT, int PIN> constexpr info_t selo0(void) { static_assert(PORT==-1, "Not an selo0 pin"); return info_t{-1,-1}; }
template <int PORT, int PIN> constexpr info_t selo1(void) { static_assert(PORT==-1, "Not an selo1 pin"); return info_t{-1,-1}; }
template <int PORT, int PIN> constexpr info_t selo2(void) { static_assert(PORT==-1, "Not an selo2 pin"); return info_t{-1,-1}; }
template <int PORT, int PIN> constexpr info_t selo3(void) { static_assert(PORT==-1, "Not an selo3 pin"); return info_t{-1,-1}; }
template <int PORT, int PIN> constexpr info_t selo4(void) { static_assert(PORT==-1, "Not an selo4 pin"); return info_t{-1,-1}; }
template <int PORT, int PIN> constexpr info_t hwin0(void) { static_assert(PORT==-1, "Not an hwin0 pin"); return info_t{-1,-1}; }

#define DOUT0 	template<> constexpr info_t dout0
#define MCLKOUT	template<> constexpr info_t mclkout
#define SCLKOUT	template<> constexpr info_t sclkout
#define SELO0	template<> constexpr info_t selo0
#define SELO1	template<> constexpr info_t selo1
#define SELO2	template<> constexpr info_t selo2
#define SELO3	template<> constexpr info_t selo3
#define SELO4	template<> constexpr info_t selo4
#define HWIN0	template<> constexpr info_t hwin0

#if (UC_DEVICE == XMC4400) && (UC_PACKAGE == LQFP100)
DX0<0,0>()  { return info_t{1,1,DX0D,	XMC_GPIO_MODE_INPUT_TRISTATE	};}
DX0<0,4>()  { return info_t{1,0,DX0A,	XMC_GPIO_MODE_INPUT_TRISTATE	};}
DX0<0,5>()  { return info_t{1,0,DX0B,	XMC_GPIO_MODE_INPUT_TRISTATE	};}
DX2<0,6>()  { return info_t{1,0,DX2A,	XMC_GPIO_MODE_INPUT_TRISTATE	};}
DX2<0,7>()  { return info_t{0,0,DX2B,	XMC_GPIO_MODE_INPUT_TRISTATE	};}
DX1<0,8>()  { return info_t{0,0,DX1B,	XMC_GPIO_MODE_INPUT_TRISTATE	};}
DX2<0,9>()  { return info_t{1,1,DX2A,	XMC_GPIO_MODE_INPUT_TRISTATE	};}
DX1<0,10>() { return info_t{1,1,DX1A,	XMC_GPIO_MODE_INPUT_TRISTATE	};}
DX1<0,11>() { return info_t{1,0,DX1A,	XMC_GPIO_MODE_INPUT_TRISTATE	};}
DX2<0,12>() { return info_t{1,1,DX2B,	XMC_GPIO_MODE_INPUT_TRISTATE	};}
DX2<1,0>()  { return info_t{0,0,DX2A,	XMC_GPIO_MODE_INPUT_TRISTATE	};}
DX1<1,1>()  { return info_t{0,0,DX1A,	XMC_GPIO_MODE_INPUT_TRISTATE	};}
DX0<1,4>()  { return info_t{0,0,DX0B,	XMC_GPIO_MODE_INPUT_TRISTATE	};}
DX0<1,5>()  { return info_t{0,0,DX0A,	XMC_GPIO_MODE_INPUT_TRISTATE	};}
DX0<2,2>()  { return info_t{0,1,DX0A,	XMC_GPIO_MODE_INPUT_TRISTATE	};}
DX2<2,3>()  { return info_t{0,1,DX2A,	XMC_GPIO_MODE_INPUT_TRISTATE	};}
DX1<2,4>()  { return info_t{0,1,DX1A,	XMC_GPIO_MODE_INPUT_TRISTATE	};}
DX0<2,5>()  { return info_t{0,1,DX0B,	XMC_GPIO_MODE_INPUT_TRISTATE	};}
DX0<2,14>() { return info_t{1,0,DX0D,	XMC_GPIO_MODE_INPUT_TRISTATE	};}
DX0<2,15>() { return info_t{1,0,DX0C,	XMC_GPIO_MODE_INPUT_TRISTATE	};}
DX1<3,0>()  { return info_t{0,1,DX1B,	XMC_GPIO_MODE_INPUT_TRISTATE	};}
DX2<3,1>()  { return info_t{0,1,DX2B,	XMC_GPIO_MODE_INPUT_TRISTATE	};}
DX0<4,0>()  { return info_t{0,1,DX0E,	XMC_GPIO_MODE_INPUT_TRISTATE	};}
DX1<4,0>()  { return info_t{1,1,DX1C,	XMC_GPIO_MODE_INPUT_TRISTATE	};}
DX0<5,0>()  { return info_t{0,0,DX0D,	XMC_GPIO_MODE_INPUT_TRISTATE	};}

DOUT0<0,1>()	{ return info_t{1,1,OUTPUT,	XMC_GPIO_MODE_OUTPUT_ALT2};}
SELO1<0,2>()	{ return info_t{1,1,OUTPUT,	XMC_GPIO_MODE_OUTPUT_ALT2};}
DOUT0<0,5>()	{ return info_t{1,0,OUTPUT,	XMC_GPIO_MODE_OUTPUT_ALT2};}
SELO0<0,6>()	{ return info_t{1,0,OUTPUT,	XMC_GPIO_MODE_OUTPUT_ALT2};}
SELO0<0,7>()	{ return info_t{0,0,OUTPUT,	XMC_GPIO_MODE_OUTPUT_ALT2};}
SCLKOUT<0,8>()	{ return info_t{0,0,OUTPUT,	XMC_GPIO_MODE_OUTPUT_ALT2};}
SELO0<0,9>()	{ return info_t{1,1,OUTPUT,	XMC_GPIO_MODE_OUTPUT_ALT2};}
SCLKOUT<0,10>()	{ return info_t{1,1,OUTPUT,	XMC_GPIO_MODE_OUTPUT_ALT2};}
SCLKOUT<0,11>()	{ return info_t{1,0,OUTPUT,	XMC_GPIO_MODE_OUTPUT_ALT2};}
SELO0<0,12>()	{ return info_t{1,1,OUTPUT,	XMC_GPIO_MODE_OUTPUT_ALT2};}
SELO0<1,0>()	{ return info_t{0,0,OUTPUT,	XMC_GPIO_MODE_OUTPUT_ALT2};}
SCLKOUT<1,1>()	{ return info_t{0,0,OUTPUT,	XMC_GPIO_MODE_OUTPUT_ALT2};}
MCLKOUT<1,3>()	{ return info_t{0,0,OUTPUT,	XMC_GPIO_MODE_OUTPUT_ALT2};}
DOUT0<1,5>()	{ return info_t{0,0,OUTPUT,	XMC_GPIO_MODE_OUTPUT_ALT2};}
SCLKOUT<1,6>()	{ return info_t{0,0,OUTPUT,	XMC_GPIO_MODE_OUTPUT_ALT2};}
DOUT0<1,7>()	{ return info_t{0,0,OUTPUT,	XMC_GPIO_MODE_OUTPUT_ALT2};}
SELO2<1,7>()	{ return info_t{1,1,OUTPUT,	XMC_GPIO_MODE_OUTPUT_ALT4};}
SELO1<1,8>()	{ return info_t{0,0,OUTPUT,	XMC_GPIO_MODE_OUTPUT_ALT2};}
SCLKOUT<1,8>()	{ return info_t{1,1,OUTPUT,	XMC_GPIO_MODE_OUTPUT_ALT4};}
SCLKOUT<1,9>()	{ return info_t{0,0,OUTPUT,	XMC_GPIO_MODE_OUTPUT_ALT1};}
DOUT0<1,9>()	{ return info_t{1,1,OUTPUT,	XMC_GPIO_MODE_OUTPUT_ALT4};}
SCLKOUT<1,10>()	{ return info_t{0,0,OUTPUT,	XMC_GPIO_MODE_OUTPUT_ALT2};}
SELO0<1,11>()	{ return info_t{0,0,OUTPUT,	XMC_GPIO_MODE_OUTPUT_ALT2};}
SELO3<1,13>()	{ return info_t{0,1,OUTPUT,	XMC_GPIO_MODE_OUTPUT_ALT2};}
SELO2<1,14>()	{ return info_t{0,1,OUTPUT,	XMC_GPIO_MODE_OUTPUT_ALT2};}
DOUT0<1,15>()	{ return info_t{1,0,OUTPUT,	XMC_GPIO_MODE_OUTPUT_ALT4};}
SELO0<2,3>()	{ return info_t{0,1,OUTPUT,	XMC_GPIO_MODE_OUTPUT_ALT2};}
SCLKOUT<2,4>()	{ return info_t{0,1,OUTPUT,	XMC_GPIO_MODE_OUTPUT_ALT2};}
DOUT0<2,5>()	{ return info_t{0,1,OUTPUT,	XMC_GPIO_MODE_OUTPUT_ALT2};}
DOUT0<2,14>()	{ return info_t{1,0,OUTPUT,	XMC_GPIO_MODE_OUTPUT_ALT2};}
SCLKOUT<3,0>()	{ return info_t{0,1,OUTPUT,	XMC_GPIO_MODE_OUTPUT_ALT2};}
SELO0<3,1>()	{ return info_t{0,1,OUTPUT,	XMC_GPIO_MODE_OUTPUT_ALT2};}
SELO1<3,3>()	{ return info_t{1,1,OUTPUT,	XMC_GPIO_MODE_OUTPUT_ALT2};}
SELO2<3,4>()	{ return info_t{1,1,OUTPUT,	XMC_GPIO_MODE_OUTPUT_ALT2};}
SELO3<3,5>()	{ return info_t{1,1,OUTPUT,	XMC_GPIO_MODE_OUTPUT_ALT2};}
DOUT0<3,5>()	{ return info_t{0,1,OUTPUT,	XMC_GPIO_MODE_OUTPUT_ALT4};}
SELO4<3,6>()	{ return info_t{1,1,OUTPUT,	XMC_GPIO_MODE_OUTPUT_ALT2};}
SCLKOUT<3,6>()	{ return info_t{0,1,OUTPUT,	XMC_GPIO_MODE_OUTPUT_ALT4};}
MCLKOUT<4,1>()	{ return info_t{1,1,OUTPUT,	XMC_GPIO_MODE_OUTPUT_ALT2};}
SELO0<4,1>()	{ return info_t{0,1,OUTPUT,	XMC_GPIO_MODE_OUTPUT_ALT4};}
DOUT0<5,1>()	{ return info_t{0,0,OUTPUT,	XMC_GPIO_MODE_OUTPUT_ALT1};}

HWIN0<0,5>()	{ return info_t{1,0,HWSEL,	XMC_GPIO_MODE_INPUT_PULL_UP};}
HWIN0<1,5>()	{ return info_t{0,0,HWSEL,	XMC_GPIO_MODE_INPUT_PULL_UP};}
#endif

#if (UC_DEVICE == XMC1301) && (UC_PACKAGE == TSSOP16)
DX0<0,14>()	{ return info_t{0,0,DX0A,	XMC_GPIO_MODE_INPUT_TRISTATE};}
DX0<0,15>()	{ return info_t{0,0,DX0B,	XMC_GPIO_MODE_INPUT_TRISTATE};}
DX0<2,0>()	{ return info_t{0,0,DX0E,	XMC_GPIO_MODE_INPUT_TRISTATE};}
DX1<0,14>()	{ return info_t{0,0,DX1A,	XMC_GPIO_MODE_INPUT_TRISTATE};}
DX1<0,7>()	{ return info_t{0,0,DX1C,	XMC_GPIO_MODE_INPUT_TRISTATE};}
DX1<0,8>()	{ return info_t{0,0,DX1B,	XMC_GPIO_MODE_INPUT_TRISTATE};}
DX1<2,0>()	{ return info_t{0,0,DX1E,	XMC_GPIO_MODE_INPUT_TRISTATE};}
DX2<0,0>()	{ return info_t{0,0,DX2A,	XMC_GPIO_MODE_INPUT_TRISTATE};}
DX2<0,9>()	{ return info_t{0,0,DX2B,	XMC_GPIO_MODE_INPUT_TRISTATE};}
DX3<2,10>()	{ return info_t{0,0,DX3C,	XMC_GPIO_MODE_INPUT_TRISTATE};}
DX3<2,6>()	{ return info_t{0,0,DX3E,	XMC_GPIO_MODE_INPUT_TRISTATE};}
DX3<2,8>()	{ return info_t{0,0,DX3D,	XMC_GPIO_MODE_INPUT_TRISTATE};}
DX4<2,10>()	{ return info_t{0,0,DX4C,	XMC_GPIO_MODE_INPUT_TRISTATE};}
DX4<2,6>()	{ return info_t{0,0,DX4E,	XMC_GPIO_MODE_INPUT_TRISTATE};}
DX4<2,8>()	{ return info_t{0,0,DX4D,	XMC_GPIO_MODE_INPUT_TRISTATE};}
DX5<2,7>()	{ return info_t{0,0,DX5C,	XMC_GPIO_MODE_INPUT_TRISTATE};}
DX5<2,9>()	{ return info_t{0,0,DX5A,	XMC_GPIO_MODE_INPUT_TRISTATE};}
DX0<0,6>()	{ return info_t{0,1,DX0C,	XMC_GPIO_MODE_INPUT_TRISTATE};}
//DX0<0,7>()	{ return info_t{0,1,DX0D,	XMC_GPIO_MODE_INPUT_TRISTATE};}
DX0<2,10>()	{ return info_t{0,1,DX0F,	XMC_GPIO_MODE_INPUT_TRISTATE};}
DX0<2,11>()	{ return info_t{0,1,DX0E,	XMC_GPIO_MODE_INPUT_TRISTATE};}
//DX1<0,7>()	{ return info_t{0,1,DX1C,	XMC_GPIO_MODE_INPUT_TRISTATE};}
//DX1<0,8>()	{ return info_t{0,1,DX1B,	XMC_GPIO_MODE_INPUT_TRISTATE};}
DX1<2,11>()	{ return info_t{0,1,DX1E,	XMC_GPIO_MODE_INPUT_TRISTATE};}
//DX2<0,0>()	{ return info_t{0,1,DX2A,	XMC_GPIO_MODE_INPUT_TRISTATE};}
//DX2<0,9>()	{ return info_t{0,1,DX2B,	XMC_GPIO_MODE_INPUT_TRISTATE};}
DX2<2,0>()	{ return info_t{0,1,DX2F,	XMC_GPIO_MODE_INPUT_TRISTATE};}
DX3<2,7>()	{ return info_t{0,1,DX3D,	XMC_GPIO_MODE_INPUT_TRISTATE};}
DX3<2,9>()	{ return info_t{0,1,DX3B,	XMC_GPIO_MODE_INPUT_TRISTATE};}
DX4<2,7>()	{ return info_t{0,1,DX4D,	XMC_GPIO_MODE_INPUT_TRISTATE};}
DX4<2,9>()	{ return info_t{0,1,DX4B,	XMC_GPIO_MODE_INPUT_TRISTATE};}
DX5<2,6>()	{ return info_t{0,1,DX5D,	XMC_GPIO_MODE_INPUT_TRISTATE};}
DX5<2,8>()	{ return info_t{0,1,DX5C,	XMC_GPIO_MODE_INPUT_TRISTATE};}

SELO0<0,0>()	{ return info_t{0,0,OUTPUT,XMC_GPIO_MODE_OUTPUT_ALT6};}
//SELO0<0,0>()	{ return info_t{0,1,OUTPUT,XMC_GPIO_MODE_OUTPUT_ALT7};}
MCLKOUT<0,6>()	{ return info_t{0,1,OUTPUT,XMC_GPIO_MODE_OUTPUT_ALT6};}
DOUT0<0,6>()	{ return info_t{0,1,OUTPUT,XMC_GPIO_MODE_OUTPUT_ALT7};}
SCLKOUT<0,7>()	{ return info_t{0,0,OUTPUT,XMC_GPIO_MODE_OUTPUT_ALT6};}
DOUT0<0,7>()	{ return info_t{0,1,OUTPUT,XMC_GPIO_MODE_OUTPUT_ALT7};}
SCLKOUT<0,8>()	{ return info_t{0,0,OUTPUT,XMC_GPIO_MODE_OUTPUT_ALT6};}
//SCLKOUT<0,8>()	{ return info_t{0,1,OUTPUT,XMC_GPIO_MODE_OUTPUT_ALT7};}
SELO0<0,9>()	{ return info_t{0,0,OUTPUT,XMC_GPIO_MODE_OUTPUT_ALT6};}
//SELO0<0,9>()	{ return info_t{0,1,OUTPUT,XMC_GPIO_MODE_OUTPUT_ALT7};}
DOUT0<0,14>()	{ return info_t{0,0,OUTPUT,XMC_GPIO_MODE_OUTPUT_ALT6};}
SCLKOUT<0,14>()	{ return info_t{0,0,OUTPUT,XMC_GPIO_MODE_OUTPUT_ALT7};}
DOUT0<0,15>()	{ return info_t{0,0,OUTPUT,XMC_GPIO_MODE_OUTPUT_ALT6};}
MCLKOUT<0,15>()	{ return info_t{0,1,OUTPUT,XMC_GPIO_MODE_OUTPUT_ALT7};}
DOUT0<2,0>()	{ return info_t{0,0,OUTPUT,XMC_GPIO_MODE_OUTPUT_ALT6};}
SCLKOUT<2,0>()	{ return info_t{0,0,OUTPUT,XMC_GPIO_MODE_OUTPUT_ALT7};}
DOUT0<2,10>()	{ return info_t{0,1,OUTPUT,XMC_GPIO_MODE_OUTPUT_ALT7};}
SCLKOUT<2,11>()	{ return info_t{0,1,OUTPUT,XMC_GPIO_MODE_OUTPUT_ALT6};}
DOUT0<2,11>()	{ return info_t{0,1,OUTPUT,XMC_GPIO_MODE_OUTPUT_ALT7};}
#endif


template <int PORT, int PIN>
constexpr info_t dout0(gpio::pin<PORT,PIN> const&)
{
    return dout0<PORT,PIN>();
}

template <int PORT, int PIN>
constexpr info_t dx0(gpio::pin<PORT,PIN> const&)
{
    return dx0<PORT,PIN>();
}
