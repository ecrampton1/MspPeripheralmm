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
	PRINT("1: ", test_num1, ENDL)
	PRINT("2: ", test_num2, ENDL)
	PRINT("3: ", test_num3, ENDL)
	PRINT("4: ", test_num4, ENDL)
	PRINT("5: ", test_num5, ENDL)


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

	int8_t* buffer = NULL;
	buffer = (int8_t*)malloc(20);

	PRINT("Buffer: ", (int)buffer, ENDL)

	if(buffer != NULL) {

		for(int i = 0; i < 20; ++i) {
			buffer[i] = i;
			uart::send(buffer[i]);
		}

	}

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

