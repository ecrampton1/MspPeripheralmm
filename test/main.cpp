#include "msp430/msp_sys.hpp"
#include "msp430/msp_gpio.hpp"
#include "msp430/msp_uart.hpp"
#include <limits.h>

extern "C" {
#include <stdlib.h>
}


namespace Periph = McuPeripheral;

typedef Periph::McuPin<McuPort1,BIT0> Led;
typedef Periph::McuPin<McuPort1,BIT6> Led2;
typedef Periph::McuUart<Periph::BaudRate::BAUD_115200,Speed::SPEED_16MHZ, BIT1,BIT0> uart;

int main()
{
	Periph::McuSystem<Speed::SPEED_16MHZ>::init();
	Led::output();
	Led2::output();
	Led::clear();
	uart::init();


	while(1) {
		Led::toggle();
		Led2::toggle();
		__delay_cycles(200000);
		uart::sendStream("Test String \n\r");
	}
	return 0;
}



