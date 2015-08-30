#include "mcu_config.hpp"
#include <msp430.h>


extern "C" {
#include <stdlib.h>
}


static volatile bool timerElapsed = false;
void periodicPrint(void* args)
{
	timerElapsed = true;

}

int main()
{

	sys::init();
	//sys::enabletWatchDog(); //starts counting for system time
	//led0::output();
	//led1::output();
	uart::init();
	uart::send("Hello world!\n");

	//mcuTimer0::stop();
	//mcuTimer0::set_callback(&periodicPrint,0);
	//int ret = mcuTimer0::start();

	//_EINT();

	//if(ret < 0)
		//uart::sendLine("Failed to start timer");
#if 0
	while(1) {
		led0::toggle();
		led1::toggle();

		//while(!timerElapsed) { }
		uart::send( static_cast<int64_t>(sys::millis()) );
		uart::sendLine();
		uart::send( static_cast<int64_t>(sys::micros()) );
		uart::sendLine();
		uart::sendLine();
		__delay_cycles(5000000);
		//timerElapsed = false;
	}
	return 0;
#endif
	while(1) {
		//led0::toggle();
		//led1::toggle();

		while(!timerElapsed) { }

		uart::sendLine("C");
		timerElapsed = false;
	}

	return 1;
}

