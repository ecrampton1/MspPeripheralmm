#include "mcu_config.hpp"
#include <msp430.h>


extern "C" {
#include <stdlib.h>
}


int main()
{
	uint8_t test_num1 = 0x20;
	int8_t test_num2 = -32;
	uint16_t test_num3 = 0x5555;
	int16_t test_num4 = -25000;
	uint32_t test_num5 = 0x500000;
	int32_t test_num6 = -1000000;
	uint64_t test_num7 = 0x123456789ABCDE;
	int64_t test_num8 = -12345678910;

	sys::init();
	//sys::enabletWatchDog(); //starts counting for system time
	led0::output();
	led1::output();
	uart::init();

	uart::send("Starting Uart Test!!!\n");
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
	uart::send("Test 7\n");
	uint32_t num = (test_num7 >> 32) & 0xFFFFFFFF;
	uart::send(num);
	uart::sendLine();
	uart::send("Test 8\n");
	uart::send(test_num8);
	uart::sendLine();


	while(1) {
		led0::toggle();
		__delay_cycles(500000);
		led1::toggle();
		__delay_cycles(500000);
	}

	return 1;
}

