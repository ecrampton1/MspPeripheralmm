#include "mcu_config.hpp"
#include <msp430.h>
#include "nrf24/nRF24L01.h"

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
	spi::send((uint8_t)RF24_SETUP_AW | (uint8_t)RF24_R_REGISTER);
	uart::send("2");
	spi::send((uint8_t)RF24_NOP);
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

