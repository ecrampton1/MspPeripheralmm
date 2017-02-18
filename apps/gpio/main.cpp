#include "mcu_config.hpp"
#include <msp430.h>
#include <stdint.h>

extern "C" {
#include <stdlib.h>
}

void pinHandle (void* args)
{
	led0::toggle();
}

int main()
{
	sys::init();
	sys::enableWatchDog(); //starts counting for system time
	led0::output();
	led1::output();
	Periph::callback_t cb = pinHandle;
	button::setPinIrqHandler(cb,(Periph::callback_args_t)0);
	button::clear();
	button::input();
	button::pullUp();
	button::edgeHighToLow();
	button::intEnable();
	uart::init();
	uart::send("Start Gpio Test\n");
	int en = P1IE;
	P1IFG = 0;
	uart::send(en);
	uart::send("\n");

	while(1) {
		led1::toggle();
		__delay_cycles(50000000);
		int flag = P1IFG;
		uart::send(flag);
		uart::send("\n");
	}

	return 1;
}

