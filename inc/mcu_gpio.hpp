#ifndef _MCU_GPIO_HPP
#define _MCU_GPIO_HPP
#include <stdint.h>
namespace McuPeripheral {

template < typename port, uint8_t pin>
class Pin
{
public:

	static void set();
	static bool read();
	static void clear();
	static void toggle();
	static void output();
	static void input();
	static void pullup();
	static void pulldown();
	static void pulloff();
};

class Port
{
public:
};

}

#endif //_MCU_GPIO_HPP
