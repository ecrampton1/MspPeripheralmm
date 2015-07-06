#include "msp430/msp_sys.hpp"
#include "msp430/msp_gpio.hpp"
#include "msp430/msp_uart.hpp"
#include <limits.h>

extern "C" {
#include <stdlib.h>
}


namespace Periph = McuPeripheral;


#define  uart_rx BIT1
#define  uart_tx BIT2

typedef Periph::McuPin<McuPort1,BIT0> led0;
typedef Periph::McuPin<McuPort1,BIT6> led1;
typedef Periph::McuUart<Periph::BaudRate::BAUD_115200,Speed::SPEED_16MHZ, uart_rx, uart_tx> uart;
typedef Periph::McuSystem<Speed::SPEED_16MHZ> sys;

int main()
{
	sys::init();
	led0::output();
	led1::output();
	uart::init();


	while(1) {
		led0::toggle();
		led1::toggle();
		__delay_cycles(200000);
		uart::sendString("Test String \n\r");
		uart::sendInt(12345);
	}
	return 0;
}



