#ifndef _MSP_PERIPH_HPP
#define _MSP_PERIPH_HPP

#include <msp430.h>
#include "msp430/msp_sys.hpp"

#define REG_16(x) *((volatile uint16_t*)x)
#define REG_8(x) *((volatile uint8_t*)x)

namespace McuPeripheral {


struct DisableAllInterrupts {
	void operator () (){
		__dint();
	}
};

struct EnableAllInterrupts {
	void operator () (){
		__eint();
	}
};

template<uint8_t _ienable, uint8_t  _iflag>
struct DisableInterrupt {
	void operator () (){
		REG_8(_ienable) &= ~_iflag;
	}
};

template<uint8_t _ienable, uint8_t  _iflag>
struct EnableInterrupt {
	void operator () (){
		REG_8(_ienable) |= _iflag;
	}
};


}

#endif //_MSP_PERIPH_HPP
