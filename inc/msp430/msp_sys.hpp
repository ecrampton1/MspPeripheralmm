#ifndef _MSP_SYS_HPP
#define _MSP_SYS_HPP

#include "mcu_sys.hpp"
#include <msp430.h>

extern volatile uint32_t mWatchDogCounter;

namespace McuPeripheral {

using SystemTime = uint32_t;

constexpr uint16_t watchdog_control(McuSpeed m)
{
	return (m == McuSpeed::SPEED_1MHZ) ? WDT_MDLY_0_5 : WDT_MDLY_8;
}

constexpr uint16_t watchdog_interval_us(McuSpeed m)
{
	return (m == McuSpeed::SPEED_1MHZ) ? 512 : 8192.0 / ((float)m / 1000000.0);
}


template<McuSpeed _speed>
class McuSystem : public SystemBase
{
public:
	/** @brief Initializes the clock and disables the watchdog.
	 *  @details This should be called as soon as main is entered.
	 */
	static void init();

	static void delayInUs(uint32_t time);

	static void inline enableWatchDog()
	{
		 WDTCTL = watchdog_control(_speed);              // WDT as interval timer .5ms (1Mhz) 1ms (8mhz) .666 (12 mhz) .5ms (16mhz)
		 IE1 |= WDTIE;
		 _BIS_SR(GIE);
	}

	static void inline disableWatchDog()
	{
	    //Diable watchdog timer
		WDTCTL = WDTPW | WDTHOLD;
	}

	static SystemTime millis()
	{
		return (mWatchDogCounter * watchdog_interval_us(_speed)) / 1000;
	}

	static SystemTime micros()
	{
		return mWatchDogCounter * watchdog_interval_us(_speed);
	}

	/** @brief Sleeps the specified time in milliseconds
	 */
	static void sleepInMs(SystemTime time)
	{
		SystemTime current = millis();
		SystemTime end = current + time;

		//ensure we running
		if((WDTCTL & WDTHOLD) == 0) {
			//overflow situation
			if(end < current) {
				while(current > 0) { current = millis(); }
			}
			while(current < end) { current = millis(); }
		}
	}

};

}

using Speed =  McuPeripheral::McuSpeed;
#endif //_MSP_SYS_HPP
