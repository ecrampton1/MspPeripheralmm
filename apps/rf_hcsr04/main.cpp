#include "mcu_config.hpp"
#include "rfm69_message_handler.hpp"
#include "message.hpp"
#include <stdlib.h>

#define NODE_ADDRESS 9

void setup() {
	sys::init();
	sys::enableWatchDog(); //starts counting for system time
	uart::init();
	Handler::begin(NODE_ADDRESS);
	rfPulse::init();
	PRINT("SETUP!",ENDL)
}

void loop() {

	Handler::serviceOnce();
	rfPulse::serviceOnce();
}


int main(void)
{
	setup();

	while(1)
	{
		loop();
	}
}



