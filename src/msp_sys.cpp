#include "msp430/msp_sys.hpp"
#include <msp430.h>

namespace McuPeripheral {

void enable_irq()  { __eint(); }
void disable_irq()  { __dint(); }

//This define will loop through all of our defined speeds
//which allows for each specialized template function to
//be defined here.
#define INIT_FUNC( S ) \
template<> \
void McuSystem<Speed::SPEED_##S>::init() \
{ \
	WDTCTL = WDTPW | WDTHOLD; \
	BCSCTL1 = CALBC1_##S; \
	DCOCTL = CALDCO_##S; \
	mSpeed = static_cast<uint32_t>(Speed::SPEED_##S)*1000000ULL ; \
}
FOR_ALL_SPEEDS( INIT_FUNC );
#undef INIT_FUNC

}
