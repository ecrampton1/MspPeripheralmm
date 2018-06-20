#ifndef _PUSH_BUTTON_CONTROL_HPP_
#define _PUSH_BUTTON_CONTROL_HPP_
#include "mcu_sys.hpp"


enum class ButtonId : uint8_t
{
	BUTTON0 = 0x01,
	BUTTON1 = 0x02,
	BUTTON2 = 0x04,
	BUTTON3 = 0x08,
	BUTTON4 = 0x10,
	BUTTON5 = 0x20,
	BUTTON6 = 0x40,
	BUTTON7 = 0x80,
};

//_pullup, false if edge is low to high and gpio is pulled
//high true if edge id high to low and pulled low
template <bool _pullup, class _gpio, class _sys, ButtonId _id, uint32_t _debounceinus=50000>
class PushButtonControl
{
public:

	static void init()
	{
		if(_pullup) {
			_gpio::pullUp();
			_gpio::edgeHighToLow();
		}
		else {
			_gpio::pullDown();
			_gpio::edgeLowToHigh();
		}
		_gpio::input();
		_gpio::setPinIrqHandler(&handlePinIrq, 0);
		_gpio::intEnable();
	}

	//Call from main loop to check if this has been triggered.
	static bool checkTriggered()
	{
		bool ret = false;
		if(mDebounce < _sys::micros() && !_gpio::isIntEnabled()){
			if(_gpio::read() == !_pullup) {
				ret = true;
			}
			_gpio::intEnable();
		}
		return ret;
	}

private:

	static void handlePinIrq(void *args)
	{
		_gpio::intDisable();
		mDebounce = _sys::micros() + _debounceinus;
	}

	static McuPeripheral::SystemTime mDebounce;
};

template <bool _pullup, class _gpio, class _sys, ButtonId _id, uint32_t _debounceinus>
McuPeripheral::SystemTime PushButtonControl<_pullup,_gpio,_sys,_id,_debounceinus>::mDebounce;

#endif //_PUSH_BUTTON_CONTROL_HPP_
