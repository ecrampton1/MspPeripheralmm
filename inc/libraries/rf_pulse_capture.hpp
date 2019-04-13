#ifndef _RF_PULSE_CAPTURE_HPP_
#define _RF_PULSE_CAPTURE_HPP_
#include "message.hpp"
#include "mcu_sys.hpp"


template <class _pulse, class _sys, class _gpio, class _handler >
class RfPulseCapture
{
public:

	static void init()
	{
		_gpio::output();
		_pulse::init();
		_pulse::setCallback(&handlePulseWidth);
		_pulse::intEnable();

		mNextTriggerTime = _sys::micros()+TIME_BETWEEN_READINGS;
	}

	static void serviceOnce()
	{
		if(mPulseFound) {
			uint16_t distanceInCm = (_pulse::getPulseWidthNs()*172)/1000000;
			sendData(distanceInCm);
		}
		else if(mNextTriggerTime >= _sys::micros()) {
			triggerPulse();
			mNextTriggerTime = _sys::micros()+TIME_BETWEEN_READINGS;
		}
		//else return doing nothing
	}

	static void handlePulseWidth(void* args)
	{
		mPulseFound = true;
		_pulse::stop();
	}

	static void triggerPulse()
	{
		_gpio::set();
		_sys::delayInUs(10);
		_gpio::clear();
		mPulseFound = false;
		_pulse::start();
	}

	static void sendData(const uint16_t distanceInCm)
	{
		uint8_t buffer[8];
		PeripheralMessages::DistanceSensorDataMsg msg(buffer,sizeof(buffer),true);
		msg.get_message_payload()->distanceInCm=distanceInCm;
		_handler::publishMessage(msg);
	}

	static McuPeripheral::SystemTime mNextTriggerTime;
	static bool mPulseFound;
	static constexpr McuPeripheral::SystemTime TIME_BETWEEN_READINGS = 500000;
};

template <class _pulse, class _sys, class _gpio, class _handler >
McuPeripheral::SystemTime RfPulseCapture<_pulse, _sys, _gpio, _handler>::mNextTriggerTime=0;
template <class _pulse, class _sys, class _gpio, class _handler  >
bool RfPulseCapture<_pulse, _sys, _gpio, _handler >::mPulseFound=false;

#endif //_RF_PULSE_CAPTURE_HPP_
