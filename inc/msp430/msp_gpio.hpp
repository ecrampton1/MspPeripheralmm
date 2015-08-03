#ifndef _MSP_GPIO_HPP
#define _MSP_GPIO_HPP
#include "mcu_gpio.hpp"
#include <stdint.h>
#include <msp430.h>

namespace McuPeripheral {

template<uint8_t _input, uint8_t _output, uint8_t _direction, uint8_t _resistor>
class McuPort : public Port
{
public:
	static const uint8_t mInputReg = _input;
	static const uint8_t mOutputReg = _output;
	static const uint8_t mDirectionReg = _direction;
	static const uint8_t mResistorReg = _resistor;
};


//TODO make pin a class
template < typename _port, uint8_t _pin >
class McuPin : public Pin<_port,_pin>
{
public:

	static void set()
	{
		*((volatile uint8_t*)_port::mOutputReg) |= _pin;
	}
	static void clear()
	{
		*((volatile uint8_t*)_port::mOutputReg) &= ~_pin;
	}
	static void toggle()
	{
		*((volatile uint8_t*)_port::mOutputReg) ^= _pin;
	}
	static void output()
	{
		*((volatile uint8_t*)_port::mDirectionReg) |= _pin;
	}
	static void input()
	{
		*((volatile uint8_t*)_port::mDirectionReg) &= ~_pin;
	}
	static void pullup()
	{
		*((volatile uint8_t*)_port::mResistor) |= _pin;
		set();
	}
	static void pulldown()
	{
		*((volatile uint8_t*)_port::mResistor) |= _pin;
		clear();
	}
	static void pulloff()
	{
		*((volatile uint8_t*)_port::mResistor) &= ~_pin;
	}
};
}

typedef McuPeripheral::McuPort<P1IN_,P1OUT_,P1DIR_,P1REN_> McuPort1;
typedef McuPeripheral::McuPort<P2IN_,P2OUT_,P2DIR_,P2REN_> McuPort2;
#endif //_MSP_GPIO_HPP
