#include "mcu_config.hpp"
#include "calibration.hpp"
#include <msp430.h>


extern "C" {
#include <stdlib.h>
}

int main()
{
	uint16_t currentBC1 = CALBC1_16MHZ;
	uint16_t currentDC = CALDCO_16MHZ;

	uart::send((int64_t)currentBC1);
	uart::sendLine();
	uart::send((int64_t)currentDC);
	uart::sendLine();

	sys::disableWatchDog();
	uart::init();
	uart::send("Starting DCO calibration!\n");
	led1::output();
	__delay_cycles(100000);
	Calibration::start();
	const Calibration::CalConstants& constants = Calibration::getConstants(3);

	uart::send((int64_t)constants.bcsctl1);
	uart::sendLine();
	uart::send((int64_t)constants.dcoctl);
	uart::sendLine();

	uart::send((int64_t)currentBC1);
	uart::sendLine();
	uart::send((int64_t)currentDC);
	uart::sendLine();

	while(1) {
		led1::toggle();
	}

	return 1;
}



