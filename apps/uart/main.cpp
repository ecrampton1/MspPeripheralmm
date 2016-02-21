#include "mcu_config.hpp"
#include <msp430.h>


extern "C" {
#include <stdlib.h>
}



void print_hex()
{
	uint8_t test_num1 = 0x20;
	int8_t test_num2 = -32;
	uint16_t test_num3 = 0x5555;
	int16_t test_num4 = -25000;
	uint32_t test_num5 = 0x50000;
	int32_t test_num6 = -1000000;
	uint32_t test_num7 = 0x1234567;
	int32_t test_num8 = -12345;
	
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
        uint8_t test_num1 = 0x20;
        int8_t test_num2 = -32;
        uint16_t test_num3 = 0x5555;
        int16_t test_num4 = -25000;
        uint32_t test_num5 = 0x50000;
        int32_t test_num6 = -1000000;
        uint32_t test_num7 = 0x1234567;
        int32_t test_num8 = -12345;

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

	while(1) {
		led0::toggle();
		sys::sleepInMs((McuPeripheral::SystemTime)500);
		led1::toggle();
		sys::sleepInMs((McuPeripheral::SystemTime)500);
	}

	return 1;
}

