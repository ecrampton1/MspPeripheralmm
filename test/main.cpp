#include "mcu_config.hpp"
#include <limits.h>
#include "msp430/msp_timer.hpp"
#include <msp430.h>
#include <legacymsp430.h>

extern "C" {
#include <stdlib.h>
}

typedef Periph::McuTimer< TimerSrc::SMCLK, timerA0, 100000 , (int32_t) Speed::SPEED_16MHZ > mcuTimer;

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

	uart::sendLine();
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
		__delay_cycles(500000);
		//timerElapsed = false;
	}
	return 0;
}






//__attribute__((__interrupt__(TIMER0_A0_VECTOR)))
//#pragma vector=TIMER0_A0_VECTOR
//__interrupt void Timer_A0(void)
//void Timer_A (void)
/*interrupt (TIMER0_A1_VECTOR) TA0_isr(void)
{
	static uint32_t mRollOverCount = 0;
	switch( TAIV )
	{
		case  2: break;                          // CCR1 not used
		case  4: break;                          // CCR2 not used
		case 10: break;           				// overflow
	}
	McuPeripheral::callbackTimerA0(0);

}*/

