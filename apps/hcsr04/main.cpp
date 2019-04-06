#include "mcu_config.hpp"
#include <msp430.h>
#include <stdint.h>

static uint32_t mStartTime = 0;
static uint32_t mEndTime = 0;
static volatile bool mOverFlow = false;

void handleOverFlow(void* args)
{
	mOverFlow = true;
}




void loop()
{
	sys::delayInMs(500);
	continuousTimer::stop();
	mOverFlow = false;
	trig_pin::set();
	sys::delayInUs(10);
	trig_pin::clear();
	continuousTimer::start();

	while(true == echo_pin::read()) //ensure we see the low to high transition
	{
		if(mOverFlow) {
			PRINT("1",ENDL);
			return;
		}
	}

	//Rising edge
	while(false == echo_pin::read()) //wait
	{
		if(mOverFlow) {
			PRINT("2",ENDL);
			return;
		}
	}

	mStartTime = continuousTimer::getTimerCount();

	//falling edge
	while(true == echo_pin::read()) //wait
	{
		if(mOverFlow) {
			PRINT("3",ENDL);
			return;
		}
	}

	mEndTime = continuousTimer::getTimerCount();


	PRINT("Start: ", mStartTime," End: ", mEndTime, "Diff: ", mEndTime - mStartTime, ENDL)
}

int main()
{
	sys::init();
	sys::enableWatchDog(); //starts counting for system time
	trig_pin::output();
	echo_pin::clear();
	echo_pin::input();
	//trig_pin::pullUp();
	//trig_pin::edgeLowToHigh();
	//trig_pin::intEnable();

	continuousTimer::setCallback(&handleOverFlow,0);
	continuousTimer::intEnable();
	uart::init();
	PRINT("Start HCSR04 Test",ENDL);

	while(1) {
		loop();

	}

	return 1;
}

