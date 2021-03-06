#ifndef _MSP_SYS_HPP
#define _MSP_SYS_HPP

#include "mcu_sys.hpp"
#include <msp430.h>
#include "msp430/msp_gpio.hpp"

extern volatile uint32_t mMillisCounter;
extern volatile uint16_t mMicrosCounter;
extern uint16_t WATCH_DOG_INTERVAL_US;
extern uint16_t WATCH_DOG_MS_INC;

namespace McuPeripheral {

constexpr uint32_t ROLLOVER_OFFSET = UINT32_MAX / 1000ULL;
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

		SystemTime ms = millis();

		if(rolloverPending) {
			if(!(ROLLOVER_BIT_CHECK & ms)){
				++uptimeRolloverCounter;
				rolloverPending = false;
			}
		}
		else {
			if(ROLLOVER_BIT_CHECK & ms) {
				rolloverPending = true;
			}
		}

		return (ROLLOVER_OFFSET * uptimeRolloverCounter) + (ms / 1000);
	}

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
		return mMillisCounter;
	}

	static SystemTime micros()
	{
		return mMillisCounter * 1000 + mMicrosCounter;
	}

	static void delayInMs(SystemTime time)
	{
		//SystemTime current = millis();
		SystemTime end = millis() + time; //Fix this next time you run!

		//ensure we running
		if((WDTCTL & WDTHOLD) == 0) {
			while(end > millis())
			{
				//_BIS_SR(LPM0_bits+GIE);
			}
		}
	}
private:

};

}

using Speed =  McuPeripheral::McuSpeed;
#endif //_MSP_SYS_HPP
