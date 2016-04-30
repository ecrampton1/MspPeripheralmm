#include "mcu_config.hpp"
//#include <limits.h>
//#include "msp430/msp_timer.hpp"
#include <msp430.h>
//#include <legacymsp430.h>
//#include "msp430/msp_timer.hpp"

extern "C" {
#include <stdlib.h>
}


int main()
{

	sys::init();
	sys::enableWatchDog(); //starts counting for system time
	uart::init();
	uart::send("Start Timer Test\n");

	while(1) {

		sys::sleepInMs(1000);
		uart::send( static_cast<int32_t>(sys::millis()),Periph::Base::BASE_DEC );
		uart::sendLine();
		uart::send( static_cast<int32_t>(sys::micros()), Periph::Base::BASE_DEC );
		uart::sendLine();
		uart::sendLine();

	}
	return 0;

	//while(1);
}

