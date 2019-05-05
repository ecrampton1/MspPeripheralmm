#ifndef _RF_HC_SR04_HPP_
#define _RF_HC_SR04_HPP_
#include "message.hpp"
#include "mcu_sys.hpp"


template <class _pulse, class _sys, class _gpio, class _handler >
class RfHcsr04
{
public:

	static void init()
	{
		_gpio::output();
		_pulse::init();
		_pulse::setCallback(&handlePulseWidth);
		_pulse::intEnable();

		mNextTriggerTime = _sys::millis()+TIME_BETWEEN_READINGS;
	}

	static void serviceOnce()
	{
		if(mPulseFound) {
			uint16_t distanceInCm = (_pulse::getPulseWidthNs()*172)/1000000;
			if(distanceInCm > 0)
				sendData(distanceInCm);
			mPulseFound = false;
		}
		else if(mNextTriggerTime <= _sys::millis()) {
			triggerPulse();
			mNextTriggerTime = _sys::millis()+TIME_BETWEEN_READINGS;
		}
		//else return doing nothing
	}
private:
	static void handlePulseWidth(void* args)
	{
		_pulse::stop();
		mPulseFound = true;

	}

	static void triggerPulse()
	{
		_gpio::set();
		_sys::delayInUs(10);
		_gpio::clear();
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
	static constexpr McuPeripheral::SystemTime TIME_BETWEEN_READINGS = 500;
};

template <class _pulse, class _sys, class _gpio, class _handler >
McuPeripheral::SystemTime RfHcsr04<_pulse, _sys, _gpio, _handler>::mNextTriggerTime=0;
template <class _pulse, class _sys, class _gpio, class _handler  >
bool RfHcsr04<_pulse, _sys, _gpio, _handler >::mPulseFound=false;

#endif //_RF_HC_SR04_HPP_
