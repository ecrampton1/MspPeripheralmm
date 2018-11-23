#include "mcu_config.hpp"
#include "rfm69_message_handler.hpp"
#include "message.hpp"
#include <stdlib.h>




#define PINGER
//McuPeripheral::SystemTime StartTime;

using Handler = PeripheralMessages::RFM69Handler;

#ifdef PINGER
//unfortunately malloc doesnt work on msp430, still even after compiling newlib...
static uint8_t ping_buffer[8];
static PeripheralMessages::PingPongDataMsg ping(ping_buffer,sizeof(ping_buffer),true);
#else
static uint8_t pong_buffer[8];
static PeripheralMessages::PingPongQueryMsg pong(pong_buffer,sizeof(pong_buffer),true);
#endif

uint16_t FailCount;


uint8_t PongerAddress = 9;
uint8_t PingerAddress = 10;

#ifdef PINGER
void handle_pong(void* args, void*  msg, uint16_t calling_id)
{
	PeripheralMessages::PingPongDataMsg* pong_ptr =
			static_cast<PeripheralMessages::PingPongDataMsg*>(msg);

	if(pong_ptr->get_message_payload()->count == ping.get_message_payload()->count) {
		led0::toggle();
		++ping.get_message_payload()->count;
	}

	Handler::publish_message(ping,PongerAddress);
}
#else
void handle_ping(void* args, void*  msg, uint16_t calling_id)
{
	PeripheralMessages::PingPongQueryMsg* ping_ptr =
			static_cast<PeripheralMessages::PingPongQueryMsg*>(msg);
	pong.get_message_payload()->count = ping_ptr->get_message_payload()->count;
	Handler::publish_message(pong);
	led0::toggle();
}
#endif


void initializeIncomingMessages()
{
#ifdef PINGER
	PeripheralMessages::PingPongDataMsg::set_callback(&handle_pong);
#else
	PeripheralMessages::PingPongQueryMsg::set_callback(&handle_ping);
#endif

}


void setup() {
	FailCount = 0;
	sys::init();
	sys::enableWatchDog(); //starts counting for system time
	led0::output();
	uart::init();
	initializeIncomingMessages();
#ifdef PINGER
	Handler::begin(PingerAddress);
	ping.get_message_payload()->count = 0;
	PRINT(".")
	Handler::publish_message(ping,PongerAddress);
#else
	Handler::begin(PongerAddress);
	PRINT("SETUP!\n")
#endif


}



void loop() {

	Handler::serviceOnce();

}


int main(void)
{
	setup();

	while(1)
	{
		loop();
	}
}


