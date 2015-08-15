#ifndef _MSP_SYS_HPP
#define _MSP_SYS_HPP

#include "mcu_sys.hpp"
#include <msp430.h>

extern volatile uint32_t mWatchDogCounter;
static volatile uint32_t mWatchDogMilliSeconds = 0;

namespace McuPeripheral {



constexpr uint16_t watchdog_control(McuSpeed m)
{
	return (m == McuSpeed::SPEED_1MHZ) ? WDT_MDLY_0_5 : WDT_MDLY_8;
}

constexpr uint16_t watchdog_interval_us(McuSpeed m)
{
	return (m == McuSpeed::SPEED_1MHZ) ? 512 : 8192.0 / ((float)m / 1000000.0);
}

/*constexpr uint16_t watchdog_ms_inc(McuSpeed m)
{
	return (m == McuSpeed::SPEED_1MHZ) ? 8192 : 8192 / (m / 1000000);
}*/

template<McuSpeed M>
class McuSystem : public SystemBase
{
public:
	/** @brief Initializes the clock and disables the watchdog.
	 *  @details This should be called as soon as main is entered.
	 */
	static void init();

	static void inline enabletWatchDog()
	{
		 WDTCTL = watchdog_control(M);              // WDT as interval timer .5ms (1Mhz) 1ms (8mhz) .666 (12 mhz) .5ms (16mhz)
		 IE1 |= WDTIE;
		 _BIS_SR(GIE);
	}

	static inline void disableWatchDog()
	{
	        /* Diable watchdog timer */
		WDTCTL = WDTPW | WDTHOLD;
	}

	static uint32_t millis()
	{
		return (mWatchDogCounter * watchdog_interval_us(M)) / 1000;
	}

	static uint32_t micros()
	{
		return mWatchDogCounter * 512;
	}

};

}

using Speed =  McuPeripheral::McuSpeed;
#endif //_MSP_SYS_HPP
