#ifndef _MSP_PERIPH_HPP
#define _MSP_PERIPH_HPP

#include <msp430.h>
#include "msp430/msp_sys.hpp"

#define REG_16(x) *((volatile uint16_t*)x)
#define REG_8(x) *((volatile uint8_t*)x)

namespace McuPeripheral {

struct DisableInterrupts {
	void operator () (){
		__dint();
	}
};

//static DisableInterrupts _DisableInterrupts;

struct EnableInterrupts {
	void operator () (){
		__dint();
	}
};

}

#endif //_MSP_PERIPH_HPP
