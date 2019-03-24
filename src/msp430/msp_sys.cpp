#include "msp430/msp_sys.hpp"

volatile uint32_t mWatchDogCounter;
namespace McuPeripheral {

//default speed is 16MHZ
//uint32_t SystemBase::mSpeed = static_cast<uint32_t>(Speed::SPEED_16MHZ);

void enable_irq()  { __eint(); }
void disable_irq()  { __dint(); }

//This define will loop through all of our defined speeds
//which allows for each specialized template function to
//be defined here.  Note turns on Aclk external crystal (assumes 32.768 khz)
#define INIT_FUNC( S ) \
template<> \
void McuSystem<Speed::SPEED_##S>::init() \
{ \
	disableWatchDog(); \
	BCSCTL1 = CALBC1_##S; \
	DCOCTL = CALDCO_##S; \
	BCSCTL3 |= XCAP_3; \
}
FOR_ALL_SPEEDS( INIT_FUNC )
#undef INIT_FUNC


static void __inline__ brief_pause(register unsigned int n)
{
	__asm__ __volatile__ (
			/* 1 instruction */
				"1: nop \n"
			/* 1 instruction */
				" dec        %[n] \n"
			/* 2 instructions */
				" jne        1b \n"
		: [n] "+r"(n));
	/* total 4 instructions */
}

template<>
void McuSystem<Speed::SPEED_1MHZ>::delayInUs(uint32_t time)
{
	//return imediately for 1/2us delay
	if(time < 3)
		return;

	brief_pause(time >> 2);
}

template<>
void McuSystem<Speed::SPEED_8MHZ>::delayInUs(uint32_t time)
{
	//return imediately for 1/2us delay
	brief_pause(time << 1);
}

template<>
void McuSystem<Speed::SPEED_12MHZ>::delayInUs(uint32_t time)
{
	brief_pause(time << 1 + time);
}

template<>
void McuSystem<Speed::SPEED_16MHZ>::delayInUs(uint32_t time)
{
	brief_pause(time << 2);
}

}

