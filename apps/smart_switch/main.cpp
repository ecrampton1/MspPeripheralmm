#include "mcu_config.hpp"
#include "rf24_message_handler.hpp"
#include "message.hpp"
#include <stdlib.h>
#include "dht/dht11.hpp"
#include <cstdint>




using Handler = PeripheralMessages::RF24MessageHandler;
static constexpr uint32_t INTERVAL = 60000000; //in US
McuPeripheral::SystemTime UpdateTime;
static constexpr uint32_t SW_INTERVAL = 50000; //in US
McuPeripheral::SystemTime SwTime0;
McuPeripheral::SystemTime SwTime1;
static const uint8_t SWITCH0 = SENSOR_SWITCH0;
static const uint8_t SWITCH1 = SENSOR_SWITCH1;

constexpr uint8_t PUSH_BUTTON_0 = 0x01;
constexpr uint8_t PUSH_BUTTON_1 = 0x02;

uint8_t ButtonUpdate = 0;


using dht = DHT<dht_data_pin,msp430_timer,sys,clock_speed>;

void trigger_relay( uint8_t relay, bool state, uint16_t response_node )
{
	uint8_t buffer_event[sizeof(PeripheralMessages::SwitchMessage) + sizeof(PeripheralMessages::PayloadHeader)];
	PeripheralMessages::SwitchEventMsg switch_event(buffer_event,sizeof(buffer_event),true,SENSOR_SWITCH0);
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
			relay0::close();
		}
		else {
			relay0::open();
		}
		switch_event.get_message_payload()->state = relay1::isClosed();
	}
	Handler::publish_message(switch_event,response_node);
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
	PeripheralMessages::SwitchQueryMsg* ptr =
			static_cast<PeripheralMessages::SwitchQueryMsg*>(msg);
	uint8_t buffer_data[sizeof(PeripheralMessages::SwitchMessage) + sizeof(PeripheralMessages::PayloadHeader)];
	PeripheralMessages::SwitchDataMsg switch_data(buffer_data,sizeof(buffer_data),true,SENSOR_SWITCH0);
	//PRINT("Switch Query: ",ptr->get_message_header()->mNodeSensorId, "is ", switch_data.get_message_payload()->state )
	if(ptr->get_message_header()->mNodeSensorId == SENSOR_SWITCH0) {
		switch_data.get_message_payload()->state = relay0_pin::read();

	}
	else if(ptr->get_message_header()->mNodeSensorId == SENSOR_SWITCH1) {
		switch_data.get_message_payload()->state = relay1_pin::read();

	}
	Handler::publish_message(switch_data);
}


void handle_button_press(void* args)
{
	uint8_t* sw = static_cast<uint8_t*>(args);
	if(*sw == SENSOR_SWITCH0) {
		SwTime0= sys::micros() + SW_INTERVAL;
		sw0_pin::intDisable();
		ButtonUpdate |= PUSH_BUTTON_0;
	}
	else {
		SwTime1= sys::micros() + SW_INTERVAL;
		sw1_pin::intDisable();
		ButtonUpdate |= PUSH_BUTTON_1;
	}
}


void initialize_switches()
{
	sw0_pin::pullUp();
	sw1_pin::pullUp();
	sw0_pin::input();
	sw0_pin::setPinIrqHandler(handle_button_press,(void *)&SWITCH0);
	sw1_pin::input();
	sw1_pin::setPinIrqHandler(handle_button_press,(void *)&SWITCH1);
	sw0_pin::edgeHighToLow();
	sw1_pin::edgeHighToLow();
	sw0_pin::intEnable();
	sw1_pin::intEnable();
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

	initializeIncomingMessages();
	sys::delayInUs(20000);
	Handler::begin();
	UpdateTime = 0;
	uart::send("Start Smart Switch\n");
	initialize_switches();
}



void loop()
{
	uint32_t temp = sys::micros();
	Handler::serviceOnce();
	if(UpdateTime < temp) {
		UpdateTime = temp + INTERVAL;
		uart::send("S\n");
		dht::sendStartBit();
	}

	if(dht::serviceOnce()) {
			uart::send("C\n");
			uint8_t buffer_temp[sizeof(PeripheralMessages::TemperatureMessage) + sizeof(PeripheralMessages::PayloadHeader)];
			PeripheralMessages::TemperatureDataMsg temp_data(buffer_temp,sizeof(buffer_temp),true,SENSOR_TEMP0);
			temp_data.get_message_payload()->temperature = dht::getData().TemperatureReal;
			Handler::publish_message(temp_data);
	}
	if( SwTime0 >= temp && (ButtonUpdate & PUSH_BUTTON_0)) {
		ButtonUpdate &= ~PUSH_BUTTON_0;
		if( sw0_pin::read() == false) {
			trigger_relay(SENSOR_SWITCH0,!relay0_pin::read(),0);
		}
		sw0_pin::intEnable();
	}
	if( SwTime1 >= temp && (ButtonUpdate & PUSH_BUTTON_1)) {
		ButtonUpdate &= ~PUSH_BUTTON_1;
		if( sw1_pin::read() == false) {
			trigger_relay(SENSOR_SWITCH1,!relay1_pin::read(),0);
		}
		sw1_pin::intEnable();
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



