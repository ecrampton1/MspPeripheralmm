#include "mcu_config.hpp"
#include "rfm69_message_handler.hpp"
#include "message.hpp"
#include <stdlib.h>

#define NODE_ADDRESS 9

void handleSwitchRequest(void* args, void*  msg, uint16_t calling_id)
{
	PeripheralMessages::SwitchRequestMsg& req =
				*static_cast<PeripheralMessages::SwitchRequestMsg*>(msg);

	rfRelay::handleMsg(req,calling_id);
}


void initializeIncomingMessages()
{
	PeripheralMessages::SwitchRequestMsg::set_callback(&handleSwitchRequest);
}


void setup() {
	sys::init();
	sys::enableWatchDog(); //starts counting for system time
	uart::init();
	Handler::begin(NODE_ADDRESS);
	rfHcsr04::init();
	rfRelay::init();
	initializeIncomingMessages();
	PRINT("SETUP!",ENDL)
}

void loop() {

	Handler::serviceOnce();
	rfHcsr04::serviceOnce();
}


int main(void)
{
	setup();

	while(1)
	{
		loop();
	}
}



