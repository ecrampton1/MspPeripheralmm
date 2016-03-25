#include "mcu_config.hpp"
#include <msp430.h>


extern "C" {
#include <stdlib.h>
}


int main()
{
	sys::init();
	//sys::enabletWatchDog(); //starts counting for system time
	led0::output();
	led1::output();
	button::input();
	uart::init();
	uart::send("Start Gpio Test\n");


	while(1) {
		led0::toggle();
		__delay_cycles(5000000);
		led1::toggle();
		__delay_cycles(5000000);
	}

	return 1;
}

