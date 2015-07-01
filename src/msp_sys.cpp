#include "msp430/msp_sys.hpp"
#include <msp430.h>

namespace McuPeripheral {

void enable_irq()  { __eint(); }
void disable_irq()  { __dint(); }

#define INIT_FUNC( S ) \
template<> \
void McuSystem<Speed::SPEED_##S>::init() \
{ \
	WDTCTL = WDTPW | WDTHOLD; \
	BCSCTL1 = CALBC1_##S; \
	DCOCTL = CALDCO_##S; \
	mSpeed = Speed::SPEED_##S; \
}
FOR_ALL_SPEEDS( INIT_FUNC );
#undef INIT_FUNC


#if 0
template<>
void McuSystem<Speed::SPEED_1MHZ>::init()
{
	// Disable watchdog
	WDTCTL = WDTPW | WDTHOLD;
	BCSCTL1 = CALBC1_1MHZ;
	DCOCTL = CALDCO_1MHZ;
	mSpeed = Speed::SPEED_1MHZ;
}

template<>
void McuSystem<Speed::SPEED_8MHZ>::init()
{
	// Disable watchdog
	WDTCTL = WDTPW | WDTHOLD;

	BCSCTL1 = CALBC1_8MHZ;
	DCOCTL = CALDCO_8MHZ;
	mSpeed = Speed::SPEED_8MHZ;
}

template<>
	void McuSystem<Speed::SPEED_12MHZ>::init()
	{
		// Disable watchdog
		WDTCTL = WDTPW | WDTHOLD;

		BCSCTL1 = CALBC1_12MHZ;
		DCOCTL = CALDCO_12MHZ;
		mSpeed = Speed::SPEED_12MHZ;
	}

template<>
	void McuSystem<Speed::SPEED_16MHZ>::init()
	{
		// Disable watchdog
		WDTCTL = WDTPW | WDTHOLD;

		BCSCTL1 = CALBC1_16MHZ;
		DCOCTL = CALDCO_16MHZ;
		mSpeed = Speed::SPEED_16MHZ;
	}
#endif
}
