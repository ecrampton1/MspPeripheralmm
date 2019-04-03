#ifndef _MSP_SYS_HPP
#define _MSP_SYS_HPP

#include "mcu_sys.hpp"
#include <msp430.h>
#include "msp430/msp_gpio.hpp"

extern volatile uint32_t mWatchDogCounter;

namespace McuPeripheral {

constexpr uint32_t ROLLOVER_OFFSET = UINT32_MAX / 1000000ULL;
constexpr uint32_t ROLLOVER_BIT_CHECK = 0x80000000ULL;

constexpr uint16_t watchdog_control(McuSpeed m)
{
	return (m == McuSpeed::SPEED_1MHZ) ? WDT_MDLY_0_5 : WDT_MDLY_8;
}

constexpr uint16_t watchdog_interval_us(McuSpeed m)
{
	return (m == McuSpeed::SPEED_1MHZ) ? 512 : 8192.0 / ((float)m / 1000000.0);
}


template<McuSpeed _speed>
class McuSystem
{
public:
	/** @brief Initializes the clock and disables the watchdog.
	 *  @details This should be called as soon as main is entered.
	 */
	static void init();

	/**
	 *  @brief Call from main loop in order to keep an up time in seconds
	 *	@returns the current uptime in seconds
	 */
	static uint32_t updateUpTimeInSeconds()
	{
		static bool rolloverPending = false;
		static uint32_t uptimeRolloverCounter = 0;

		SystemTime us = micros();

		if(rolloverPending) {
			if(!(ROLLOVER_BIT_CHECK & us)){
				++uptimeRolloverCounter;
				rolloverPending = false;
			}
		}
		else {
			if(ROLLOVER_BIT_CHECK & us) {
				rolloverPending = true;
			}
		}

		return (ROLLOVER_OFFSET * uptimeRolloverCounter) + (us / 1000000);
	}

	static void delayInUs(uint32_t time);

	static void inline enableWatchDog()
	{
		mWatchDogCounter = 0;
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

	/** @brief Blocks the specified time in milliseconds
	 */
	/*static void delayInMs(SystemTime time)
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
	}*/

	static void delayInMs(SystemTime time)
	{
		//SystemTime current = millis();
		SystemTime start = micros(); //Fix this next time you run!

		//ensure we running
		if((WDTCTL & WDTHOLD) == 0) {
			while(time)
			{
				if((micros() - start) >= 1000) {
					--time;
					start += 1000;
				}
				//_BIS_SR(LPM0_bits+GIE);
			}
		}
	}
private:

};

}

using Speed =  McuPeripheral::McuSpeed;
#endif //_MSP_SYS_HPP
