#ifndef _MSP_GPIO_HPP
#define _MSP_GPIO_HPP
#include <stdint.h>
#include <msp430.h>
#include "utilities/utilities.hpp"

namespace McuPeripheral {

template<uint8_t _input, uint8_t _select2>
class McuPort 
{
public:
	static const uint8_t mInputReg = _input;
	static const uint8_t mOutputReg = _input+1;
	static const uint8_t mDirectionReg = _input+2;
	static const uint8_t mIntFlagReg = _input+3;
	static const uint8_t mIntEdgeSelReg = _input+4;
	static const uint8_t mIntEnableReg = _input+5;
	static const uint8_t mSelectReg = _input+6;
	static const uint8_t mResistorReg = _input+7;
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
	static void pullUp()
	{
		*((volatile uint8_t*)_port::mResistor) |= _pin;
		set();
	}
	static void pullDown()
	{
		*((volatile uint8_t*)_port::mResistor) |= _pin;
		clear();
	}
	static void pullOff()
	{
		*((volatile uint8_t*)_port::mResistor) &= ~_pin;
	}
	static void intEnable()
	{
		*((volatile uint8_t*)_port::mIntEnableReg) |= _pin;
	}
	static void intDisable()
	{
		*((volatile uint8_t*)_port::mIntEnableReg) &= ~_pin;
	}
	static void edgeHighToLow()
	{
		*((volatile uint8_t*)_port::mIntEdgeSelReg) |= _pin;
	}
	static void edgeLowToHigh()
	{
		*((volatile uint8_t*)_port::mIntEdgeSelReg) &= ~_pin;
	}
	static bool irqFlag()
	{
		return *((volatile uint8_t*)_port::mInputReg) & _pin;
	}
	static void selectOff()
	{
		*((volatile uint8_t*)_port::mSelectReg) &= ~_pin;
	}
	static void select2Off()
	{
		*((volatile uint8_t*)_port::mSelect2Reg) &= ~_pin;
	}
	static void selectOn()
	{
		*((volatile uint8_t*)_port::mSelectReg) |= _pin;
	}
	static void select2On()
	{
		*((volatile uint8_t*)_port::mSelect2Reg) |= _pin;
	}
	static void setPinIrqHandler(callback_t cb, callback_args_t args)
	{
		mPinHandler = cb;
		mPinArgs = args;
	}

	static callback_t mPinHandler;
	static callback_args_t mPinArgs;

};

template < class _port, uint8_t _pin >
callback_t McuPin<_port,_pin>::mPinHandler;
template < class _port, uint8_t _pin >
callback_args_t McuPin<_port,_pin>::mPinArgs;


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
	static void selectOn()
	{
		return;
	}
	static void select2On()
	{
		return;
	}
};



}

using McuPort1 =  McuPeripheral::McuPort<P1IN_,P1SEL2_>;
using McuPort2 =  McuPeripheral::McuPort<P2IN_,P2SEL2_>;
#endif //_MSP_GPIO_HPP
