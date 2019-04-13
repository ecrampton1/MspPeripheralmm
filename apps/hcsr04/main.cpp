#include "mcu_config.hpp"
#include <msp430.h>
#include <stdint.h>

static volatile bool mOverFlow = false;
static volatile bool mPulseFound = false;

void handleOverFlow(void* args)
{
	mOverFlow = true;
	mPulseFound = true;
}

void handlePulseWidth(void* args)
{
	mPulseFound = true;
	pulseWidthMeasure::stop();
}




void loop()
{
	mPulseFound = false;
	mOverFlow = false;
	sys::delayInMs(500);
	trig_pin::set();
	sys::delayInUs(10);
	trig_pin::clear();

	pulseWidthMeasure::start();

	while(false == mPulseFound);

	if(mOverFlow) {
		PRINT("Overflow", ENDL)
	}
	PRINT("Width cm: ", (pulseWidthMeasure::getPulseWidthNs()*172)/1000000, ENDL)


}

int main()
{
	sys::init();
	sys::enableWatchDog(); //starts counting for system time
	uart::init();
	led0::output();
	led0::set();
	trig_pin::output();

	PRINT("Start HCSR04 Test",ENDL);
	pulseWidthMeasure::init();
	pulseWidthMeasure::setCallback(&handlePulseWidth);
	pulseWidthMeasure::intEnable();
	//continuousTimer::setCallback(&handleOverFlow);
	//continuousTimer::intEnable();



	while(1) {
		loop();

	}

	return 1;
}

