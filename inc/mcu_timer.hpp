#ifndef _MCU_TIMER_HPP_
#define _MCU_TIMER_HPP_
#include "mcu_sys.hpp"
#include "utilities/utilities.hpp"

namespace McuPeripheral {


template< class t>
class TimerBase
{
public:
	static void set_callback(callback_t callback, void* args)
	{
		mTimerCallback = callback;
		mTimerCallbackArgs = args;
	}

protected:
	static callback_t mTimerCallback;
	static void* mTimerCallbackArgs;
};

template< class t>
callback_t TimerBase<t>::mTimerCallback;
template< class t>
void* TimerBase<t>::mTimerCallbackArgs;

}
#endif //_MCU_TIMER_HPP_
