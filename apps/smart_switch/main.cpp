#include "mcu_config.hpp"
#include "rf24_message_handler.hpp"
#include "message.hpp"
#include <stdlib.h>
#include "dht/dht11.hpp"
#include <cstdint>




using MessageHandler = PeripheralMessages::RF24MessageHandler;
static constexpr uint32_t INTERVAL = 60000000; //in US
McuPeripheral::SystemTime UpdateTime;
McuPeripheral::SystemTime SwTime0;
McuPeripheral::SystemTime SwTime1;

constexpr uint8_t PUSH_BUTTON_0 = 0x01;
constexpr uint8_t PUSH_BUTTON_1 = 0x02;

uint8_t ButtonUpdate = 0;


using dht = DHT<dht_data_pin,msp430_timer,sys,clock_speed>;

void trigger_relay( uint8_t relay, bool state, uint16_t response_node )
{

	uint8_t buffer_event[sizeof(PeripheralMessages::SwitchMessage) + sizeof(PeripheralMessages::PayloadHeader)];
	PeripheralMessages::SwitchEventMsg switch_event(buffer_event,sizeof(buffer_event),true,relay);
	//expand this to a macro to handle more switches?
	if(relay == SENSOR_SWITCH0) {
		if(state ) {
			relay0::close();
		}
		else {
			relay0::open();
		}
		switch_event.get_message_payload()->state = relay0::isClosed();
	}
	else if(relay == SENSOR_SWITCH1) {
		if(state ) {
			relay1::close();
		}
		else {
			relay1::open();
		}
		switch_event.get_message_payload()->state = relay1::isClosed();
	}
	MessageHandler::publish_message(switch_event,response_node);
}


void handle_switch_request(void* args, void*  msg, uint16_t calling_id)
{
	PeripheralMessages::SwitchRequestMsg* ptr =
			static_cast<PeripheralMessages::SwitchRequestMsg*>(msg);
	//unfortunately malloc doesnt work on msp430, still even after compiling newlib...
	trigger_relay(ptr->get_message_header()->mNodeSensorId,ptr->get_message_payload()->state,calling_id);

}


void handle_switch_query(void* args, void*  msg, uint16_t calling_id)
{
	bool send = false;
	PeripheralMessages::SwitchQueryMsg* ptr =
			static_cast<PeripheralMessages::SwitchQueryMsg*>(msg);
	uint8_t buffer_data[sizeof(PeripheralMessages::SwitchMessage) + sizeof(PeripheralMessages::PayloadHeader)];
	PeripheralMessages::SwitchDataMsg switch_data(buffer_data,sizeof(buffer_data),true,ptr->get_message_header()->mNodeSensorId);
	//PRINT("Switch Query: ",ptr->get_message_header()->mNodeSensorId, "is ", switch_data.get_message_payload()->state )
	if(ptr->get_message_header()->mNodeSensorId == SENSOR_SWITCH0) {
		switch_data.get_message_payload()->state = relay0::isClosed();
		send = true;
	}
	else if(ptr->get_message_header()->mNodeSensorId == SENSOR_SWITCH1) {
		switch_data.get_message_payload()->state = relay1::isClosed();
		send = true;
	}
	if(send) {
		MessageHandler::publish_message(switch_data);
	}
}


void initializeIncomingMessages()
{
	PeripheralMessages::SwitchQueryMsg::set_callback(&handle_switch_query);
	PeripheralMessages::SwitchRequestMsg::set_callback(&handle_switch_request);
}


void setup()
{
	sys::init();
	sys::enableWatchDog(); //starts counting for system time
	uart::init();
	relay0::init();
	relay1::init();
	uart::send("Init\n");
	initializeIncomingMessages();
	sys::delayInUs(20000);
	MessageHandler::begin();
	UpdateTime = 0;
	uart::send("Start Smart Switch\n");
	push_button0::init();
	push_button1::init();
	trigger_relay(SENSOR_SWITCH0,false,0);
	trigger_relay(SENSOR_SWITCH1,false,0);
}



void loop()
{
	uint32_t temp = sys::micros();
	MessageHandler::serviceOnce();
	//move into dht class also make this better
	if(UpdateTime < temp ) {
		//temp to handle a overlfow issue need better handling
		if ((( 4294967295 - temp ) > INTERVAL) ^ (UpdateTime >= INTERVAL)) {
			UpdateTime = temp + INTERVAL;
			uart::send("S\n");
			dht::sendStartBit();
		}
	}

	if(dht::serviceOnce()) {
			uart::send("C\n");
			uint8_t buffer_temp[sizeof(PeripheralMessages::TemperatureMessage) + sizeof(PeripheralMessages::PayloadHeader)];
			PeripheralMessages::TemperatureDataMsg temp_data(buffer_temp,sizeof(buffer_temp),true,SENSOR_TEMP0);
			temp_data.get_message_payload()->temperature = dht::getData().TemperatureReal;
			MessageHandler::publish_message(temp_data);
	}

	if(push_button0::checkTriggered()) {
		trigger_relay(SENSOR_SWITCH0,!relay0::isClosed(),0);
	}
	if(push_button1::checkTriggered()) {
		trigger_relay(SENSOR_SWITCH1,!relay1::isClosed(),0);
	}
}


int main(void)
{
	setup();

	while(1)
	{
		loop();
	}
}



