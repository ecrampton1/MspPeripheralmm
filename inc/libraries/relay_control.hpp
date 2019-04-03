#ifndef _RELAY_CONTOL_HPP_
#define _RELAY_CONTOL_HPP_


//_closed , true if gpio high to close relay else false if gpio low to close relay.
//this should init open
template <bool _closedhigh, class _gpio, class _sys, bool _keepopen=true>
class RelayControl
{
public:

	static void init()
	{
		if(_keepopen) {
			open();
		}
		else {
			close();
		}
		_gpio::output();
	}

	static bool isClosed()
	{
		if(_closedhigh) {
			return _gpio::read();
		}
		else {
			return !_gpio::read();
		}
	}

	static void open()
	{
		if(_closedhigh) {
			_gpio::clear();
		}
		else {
			_gpio::set();
		}
	}

	static void close()
	{
		if(_closedhigh) {
			_gpio::set();
		}
		else {
			_gpio::clear();
		}
	}

	static void momentary(const uint32_t contactInMs)
	{
		if(_keepopen) {
			close();
			_sys::delayInMs(contactInMs);
			open();
		}
		else {
			open();
			_sys::delayInMs(contactInMs);
			close();
		}
	}
};


#endif //_RELAY_CONTOL_HPP_
