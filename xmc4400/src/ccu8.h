#ifndef CCU8_H
#define CCU8_H

extern struct ccu8_t:public CCU8_GLOBAL_TypeDef {
    const uint32_t RESERVED2[31];
    struct cc8:public CCU8_CC8_TypeDef {
	const uint32_t RESERVED2[18];
    } cc[4];
} ccu8[4];

namespace ccu8_ns {
union GCTRL_t {
    struct {
	uint32_t PRBC:3;
	uint32_t const:1;
	uint32_t PCIS:2;
	uint32_t const:2;
	uint32_t SUSCFG:2;
	uint32_t MSE0:1;
	uint32_t MSE1:1;
	uint32_t MSE2:1;
	uint32_t MSE3:1;
	uint32_t MSDE:2;
    };
    uint32_t raw;
    operator uint32_t() const { return raw; }
};

union GIDLC_t {
    struct {
	uint32_t CC0:1;
	uint32_t CC1:1;
	uint32_t CC2:1;
	uint32_t CC3:1;
	uint32_t :4;
	uint32_t PRESCALER:1;
	uint32_t PRESCALER_CLEAR:1;
	uint32_t PARITY_CHECKER:1;
    } fields;
    uint32_t raw;
    operator uint32_t() const { return raw; }
};

union GSTAT_t {
    struct {
	uint32_t S0IDLE:1;
	uint32_t S1IDLE:1;
	uint32_t S2IDLE:1;
	uint32_t S3IDLE:1;
	uint32_t const:4;
    };
    uint32_t raw;
    operator uint32_t() const { return raw; }
};

union TCCLR_t {
    struct {
	uint32_t TRBC:1;
	uint32_t TCC:1;
	uint32_t DITC:1;
	uint32_t DTC1C:1;
	uint32_t DTC2C:1;
    };
    uint32_t raw;
    operator uint32_t() const { return raw; }
};

typedef TCCLR_t TCSET_t ;

union TC_t {
    struct {
	uint32_t CENTER_ALIGNED:1; // Counting Mode (0=edge , 1=center aligned)
	uint32_t TSSM:1;	// Singleshot
	uint32_t CLST:1;	// Shadow Transfer on Clear
	uint32_t CMOD:1;	// Capture Compare Mode (0=compare,1=capture)
	uint32_t ECM:1;		// Extended Capture Mode
	uint32_t CAPC:2;	// Clear on Capture Control
	uint32_t TLS:1;		// Timer Load selector (0=CR1, 1=CR2)
	uint32_t ENDM:2;	// Extended Stop Function Control
	uint32_t SCE:1;		// Equal Capture Event enable
	uint32_t CCS:1;		// Continuous Capture Enable
	uint32_t DITHE:2;	// Dither Enable
	uint32_t DIM:1;		// Dither input selector
	uint32_t FPE:1;		// Floating Prescaler enable
	uint32_t TRAPE0:1;	// TRAP enable for CCU8x.OUTy0
	uint32_t TRAPE1:1;	// TRAP enable for CCU8x.OUTy1
	uint32_t TRAPE2:1;	// TRAP enable for CCU8x.OUTy2
	uint32_t TRAPE3:1;	// TRAP enable for CCU8x.OUTy3
	uint32_t TRPSE:1;	// TRAP Synchronization Enable
	uint32_t TRPSW:1;	// TRAP State Clear Control
	uint32_t EMS:1;		// External Modulation Synchronization
	uint32_t EMT:1;		// External Modulation Type
	uint32_t MCME1:1;	// Multi Channel Mode Enable for Channel 1
	uint32_t MCME2:1;	// Multi Channel Mode Enable for Channel 2
	uint32_t EME1:1;	// External Modulation enable Channel 1
	uint32_t EME2:1;	// External Modulation enable Channel 2
	uint32_t STOS:2;	// Status bit output selector
    };
    uint32_t raw;
    operator uint32_t() const { return raw; }
};


union INTE_t {
    struct {
	uint32_t PERIOD_MATCH:1;	// Period match while counting up
	uint32_t ONE_MATCH:1;		// One match while counting down 
	uint32_t COMPARE1_MATCH_UP:1;	// Ch1 Compare match while counting up 
	uint32_t COMPARE1_MATCH_DOWN:1;	// Ch1 Compare match while counting down
	uint32_t COMPARE2_MATCH_UP:1;	// Ch2 Compare match while counting up 
	uint32_t COMPARE2_MATCH_DOWN:1;	// Ch2 Compare match while counting down
	uint32_t :2;
	uint32_t EVENT0:1;		// Event 0 interrupt enable
	uint32_t EVENT1:1;		// Event 1 interrupt enable
	uint32_t EVENT2:1;		// Event 2 interrupt enable

    };
    uint32_t raw;
    operator uint32_t() const { return raw; }
};

union SRS_t {
    struct {
	uint32_t POSR:2;	// Period/One match Service request selector
	uint32_t CM1SR:2;	// Ch1 Compare match Service request selector
	uint32_t CM2SR:2;	// Ch2 Compare match Service request selector
	uint32_t :2;
	uint32_t E0SR:2;	// Event 0 Service request selector
	uint32_t E1SR:2;	// Event 1 Service request selector
	uint32_t E2SR:2;	// Event 2 Service request selector
    };
    uint32_t raw;
    operator uint32_t() const { return raw; }
};

}

#endif
