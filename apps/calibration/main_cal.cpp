#include "mcu_config.hpp"
#include "calibration.hpp"
#include <msp430.h>


extern "C" {
#include <stdlib.h>
}

int main()
{
	sys::disableWatchDog();
	uart::init();
	uart::send("Starting DCO calibration!\n");
	led1::output();
	__delay_cycles(100000);
	Calibration::start();

	while(1) {
		led1::toggle();
	}

	return 1;
}



