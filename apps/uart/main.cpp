#include "mcu_config.hpp"
#include <msp430.h>
#include "utilities/circfifo.hpp"

extern "C" {
#include <stdlib.h>
}

static const uint8_t test_num1 = 0x20;
static const int8_t test_num2 = -32;
static const uint16_t test_num3 = 0x5555;
static const int16_t test_num4 = -25000;
static const uint32_t test_num5 = 0x50000;
static const int32_t test_num6 = -1000000;
static const uint32_t test_num7 = 0x1234567;
static const int32_t test_num8 = -12345;

void print_hex()
{
	
	uart::send("Test 1\n");
	uart::send(test_num1);
	uart::sendLine();
	uart::send("Test 2\n");
	uart::send(test_num2);
	uart::sendLine();
	uart::send("Test 3\n");
	uart::send(test_num3);
	uart::sendLine();
	uart::send("Test 4\n");
	uart::send(test_num4);
	uart::sendLine();
	uart::send("Test 5\n");
	uart::send(test_num5);
	uart::sendLine();
	uart::send("Test 6\n");
	uart::send(test_num6);
	uart::sendLine();
	uart::send("Test 7\n");
	uart::send(test_num7);
	uart::sendLine();
	uart::send("Test 8\n");
	uart::send(test_num8);
	uart::sendLine();

}


using b = McuPeripheral::Base;
void print_dec()
{

        uart::send("Test 1\n");
        uart::send(test_num1, b::BASE_DEC);
        uart::sendLine();
        uart::send("Test 2\n");
        uart::send(test_num2, b::BASE_DEC);
        uart::sendLine();
        uart::send("Test 3\n");
        uart::send(test_num3, b::BASE_DEC);
        uart::sendLine();
        uart::send("Test 4\n");
        uart::send(test_num4, b::BASE_DEC);
        uart::sendLine();
        uart::send("Test 5\n");
        uart::send(test_num5, b::BASE_DEC);
        uart::sendLine();
        uart::send("Test 6\n");
        uart::send(test_num6, b::BASE_DEC);
        uart::sendLine();
        uart::send("Test 7\n");
        uart::send(test_num7, b::BASE_DEC);
        uart::sendLine();
        uart::send("Test 8\n");
        uart::send(test_num8, b::BASE_DEC);
        uart::sendLine();

}


int main()
{

	sys::init();
	sys::enableWatchDog(); //starts counting for system time
	led0::output();
	led1::output();
	uart::init();

	uart::send("Starting Uart Test!!!\n");
	print_hex();
	print_dec();

	uint8_t data;
	while(1) {
		while(uart::readByte(data)) {
			uart::send(&data, 1);
		}
	}

	return 1;
}

