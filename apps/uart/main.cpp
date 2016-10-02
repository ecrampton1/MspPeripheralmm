#include "mcu_config.hpp"
#include <msp430.h>


extern "C" {
#include <stdlib.h>
}

static const uint8_t test_num1 = 0x20;
static const int8_t test_num2 = -32;
static const uint16_t test_num3 = 0x5555;
static const int16_t test_num4 = -25000;
static const uint32_t test_num5 = 0x1234567;

void print_hex()
{
	
	uart::send("1: ");
	uart::send(test_num1);
	uart::sendLine();
	uart::send("2: ");
	uart::send(test_num2);
	uart::sendLine();
	uart::send("3: ");
	uart::send(test_num3);
	uart::sendLine();
	uart::send("4: ");
	uart::send(test_num4);
	uart::sendLine();
	uart::send("5: ");
	uart::send(test_num5);
	uart::sendLine();

}


using b = McuPeripheral::Base;
void print_dec()
{

	uart::send("1: ");
	uart::send(test_num1, b::BASE_DEC);
	uart::sendLine();
	uart::send("2: ");
	uart::send(test_num2, b::BASE_DEC);
	uart::sendLine();
	uart::send("3: ");
	uart::send(test_num3, b::BASE_DEC);
	uart::sendLine();
	uart::send("4: ");
	uart::send(test_num4, b::BASE_DEC);
	uart::sendLine();
	uart::send("5: ");
	uart::send(test_num5, b::BASE_DEC);
	uart::sendLine();

}


int main()
{

	sys::init();
	sys::enableWatchDog(); //starts counting for system time
	led0::output();
	led1::output();
	uart::init();

	uart::send("Start Uart Test\n");
	print_hex();
	print_dec();

	PRINT("Number 8 ", (int) 8, " Number 16 ", (int) 16, ENDL)

	uint8_t data;
	while(1) {
		while(uart::readByte(data)) {
			uart::send(&data, 1);
			led0::toggle();
		}
	}

	return 1;
}

