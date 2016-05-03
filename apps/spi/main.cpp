#include "mcu_config.hpp"
#include <msp430.h>

extern "C" {
#include <stdlib.h>
}


int main()
{

	sys::init();
	sys::enableWatchDog(); //starts counting for system time
	led0::output();
	nrf_csn::output();
	nrf_ce::output();
	nrf_irq::input();
	uart::init();
	spi::init();

	nrf_csn::set();
	nrf_ce::clear();

	uart::send("0");
	spi::send((uint8_t)0x00);

	uart::send("1");
	nrf_csn::clear();
	spi::send((uint8_t)0x03);
	uart::send("2");
	spi::send((uint8_t)0XFF);
	uart::send("3");
	nrf_csn::set();

	uint8_t data = 0;
	if(spi::read(data)) {
		uart::send(data);
		uart::sendLine();
	}
	else {
		uart::sendLine("No spi data");
	}

	while(1);
	return 1;
}

