#ifndef _RF_RELAY_CONTOL_HPP_
#define _RF_RELAY_CONTOL_HPP_
#include "message.hpp"

//_closed , true if gpio high to close relay else false if gpio low to close relay.
//this should init open
template <class _relay, class _sys, class _handler, bool _momentary=false, uint32_t _momenttimems=500 >
class RfRelayControl
{
public:

	static void init()
	{
		_relay::init();
	}

	static void handleMsg(PeripheralMessages::SwitchRequestMsg& switchMsg, const uint16_t calling_id)
	{
		bool state = switchMsg.get_message_payload()->state;
		if(_momentary) {
			if(state) {
				//because this is momentary send true and then reset to false.
				sendResponseEvent(state,calling_id);
				_relay::momentary(_momenttimems);
				state = false;
			}
		}
		else {
			if(state) {
				_relay::close();
			}
			else {
				_relay::open();
			}
		}
		sendResponseEvent(state,calling_id);

	}

	static void sendResponseEvent(const bool state, const uint16_t calling_id)
	{
		uint8_t buffer[8];
		PeripheralMessages::SwitchEventMsg msg(buffer,sizeof(buffer),true);
		msg.get_message_payload()->state=state;
		_handler::publishMessage(msg,calling_id);
	}

};


#endif //_RF_RELAY_CONTOL_HPP_
