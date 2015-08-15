#include "msp430/msp_sys.hpp"

volatile uint32_t mWatchDogCounter = 0;
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
	disableWatchDog(); \
	BCSCTL1 = CALBC1_##S; \
	DCOCTL = CALDCO_##S; \
	mSpeed = static_cast<uint32_t>(Speed::SPEED_##S) ; \
}
FOR_ALL_SPEEDS( INIT_FUNC );
#undef INIT_FUNC




}


//__attribute__((__interrupt__(WDT_VECTOR)))
//void watchDog (void)
#pragma vector=WDT_VECTOR
__attribute__((__interrupt__)) void watchDog(void)
{
	++mWatchDogCounter;
}
