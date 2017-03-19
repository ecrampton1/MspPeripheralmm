#ifndef _RELAY_CONTOL_HPP_
#define _RELAY_CONTOL_HPP_


//_closed , true if gpio high to close relay else false if gpio low to close relay.
//this should init open
template <bool _closed, class _gpio>
class RelayControl
{
public:

	static void init()
	{
		open();
		_gpio::output();
	}

	static bool isClosed()
	{
		return _gpio::read();
	}

	static void open()
	{
		if(_closed) {
			_gpio::clear();
		}
		else {
			_gpio::set();
		}
	}

	static void close()
	{
		if(_closed) {
			_gpio::set();
		}
		else {
			_gpio::clear();
		}
	}

};


#endif //_RELAY_CONTOL_HPP_
