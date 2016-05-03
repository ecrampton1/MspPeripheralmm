#include "mcu_config.hpp"
#include <msp430.h>


extern "C" {
#include <stdlib.h>
}

void pinHandle (void* args)
{
	led1::toggle();
}

int main()
{
	sys::init();
	//sys::enabletWatchDog(); //starts counting for system time
	led0::output();
	led1::output();
	Periph::callback_t cb = pinHandle;
	button::setPinIrqHandler(cb,(Periph::callback_args_t)0);
	button::input();
	button::edgeHighToLow();
	button::intEnable();
	uart::init();
	uart::send("Start Gpio Test\n");


	while(1) {
		led0::toggle();
		__delay_cycles(5000000);
	}

	return 1;
}

