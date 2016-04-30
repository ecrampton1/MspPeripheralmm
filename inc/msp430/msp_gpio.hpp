#ifndef _MSP_GPIO_HPP
#define _MSP_GPIO_HPP
#include <stdint.h>
#include <msp430.h>

namespace McuPeripheral {

template<uint8_t _input, uint8_t _output, uint8_t _direction,
uint8_t _resistor, uint8_t _select, uint8_t _select2>
class McuPort 
{
public:
	static const uint8_t mInputReg = _input;
	static const uint8_t mOutputReg = _output;
	static const uint8_t mDirectionReg = _direction;
	static const uint8_t mResistorReg = _resistor;
	static const uint8_t mSelectReg = _select;
	static const uint8_t mSelect2Reg = _select2;
};



template < class _port, uint8_t _pin >
class McuPin 
{
public:

	static void set()
	{
		*((volatile uint8_t*)_port::mOutputReg) |= _pin;
	}
	static bool read()
	{
		return *((volatile uint8_t*)_port::mInputReg) & _pin;
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
	static void pull_up()
	{
		*((volatile uint8_t*)_port::mResistor) |= _pin;
		set();
	}
	static void pull_down()
	{
		*((volatile uint8_t*)_port::mResistor) |= _pin;
		clear();
	}
	static void pull_off()
	{
		*((volatile uint8_t*)_port::mResistor) &= ~_pin;
	}
	static void select_off()
	{
		*((volatile uint8_t*)_port::mSelectReg) &= ~_pin;
	}
	static void select2_off()
	{
		*((volatile uint8_t*)_port::mSelect2Reg) &= ~_pin;
	}
	static void select_on()
	{
		*((volatile uint8_t*)_port::mSelectReg) |= _pin;
	}
	static void select2_on()
	{
		*((volatile uint8_t*)_port::mSelect2Reg) |= _pin;
	}
};



class FakePin
{
public:

	static void set()
	{
		return;
	}
	static bool read()
	{
		return true;
	}
	static void clear()
	{
		return;
	}
	static void toggle()
	{
		return;
	}
	static void output()
	{
		return;
	}
	static void input()
	{
		return;
	}
	static void pull_up()
	{
		return;
	}
	static void pull_down()
	{
		return;
	}
	static void pull_off()
	{
		return;
	}
	static void select_off()
	{
		return;
	}
	static void select2_off()
	{
		return;
	}
	static void select_on()
	{
		return;
	}
	static void select2_on()
	{
		return;
	}
};



}

using McuPort1 =  McuPeripheral::McuPort<P1IN_,P1OUT_,P1DIR_,P1REN_,P1SEL_,P1SEL2_>;
using McuPort2 =  McuPeripheral::McuPort<P2IN_,P2OUT_,P2DIR_,P2REN_,P2SEL_,P2SEL2_>;
#endif //_MSP_GPIO_HPP
