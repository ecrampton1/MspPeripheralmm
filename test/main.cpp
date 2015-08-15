#include "mcu_config.hpp"
#include <limits.h>
#include "msp430/msp_timer.hpp"
#include <msp430.h>
//#include <legacymsp430.h>

extern "C" {
#include <stdlib.h>
}

using mcuTimer = Periph::McuTimer< TimerSrc::SMCLK, TimerA0, 100000 , (int32_t) Speed::SPEED_16MHZ >;

static volatile bool timerElapsed = false;
void periodicPrint(void* args)
{
	timerElapsed = true;

}

int main()
{

	sys::init();
	sys::enabletWatchDog(); //starts couting for system time
	led0::output();
	led1::output();
	uart::init();

	uart::sendLine();
	uart::send( (int64_t) 1110 );
	uart::sendLine();
	uart::send(0xAAABB, Periph::Base::BASE_HEX);

	//uart::sendLine();
	//mcuTimer::stop();
	//mcuTimer::set_callback(&periodicPrint,0);
	//int ret = mcuTimer::start();
	_EINT();

	//if(ret < 0)
		//uart::sendLine("Failed to start timer");
	while(1) {
		led0::toggle();
		led1::toggle();

		//while(!timerElapsed) { }
		uart::send( sys::millis() );
		uart::sendLine();
		uart::send( sys::micros() );
		uart::sendLine();
		uart::sendLine();
		__delay_cycles(5000000);
		//timerElapsed = false;
	}
	return 0;
}

