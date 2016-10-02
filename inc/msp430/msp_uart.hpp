#ifndef _MSP_UART_HPP
#define _MSP_UART_HPP
#include "mcu_uart.hpp"
#include "msp_periph.hpp"
#include "utilities/utilities.hpp"
#include <stdint.h>
#include <msp430.h>
#include "msp_sys.hpp"
#include "msp_gpio.hpp"
#include "utilities/circfifo.hpp"
#include <math.h>
#include "msp430/msp_periph.hpp"

namespace McuPeripheral
{

enum class UartClockSource : uint8_t {  SMCLK = UCSSEL_2, ACLK = UCSSEL_1 };


template< class _rxpin, class _txpin, class _irq, uint8_t _ctl0, uint8_t _iflag, uint8_t _txflag,  uint8_t _rxflag>
class UartControl
{
public:

	static void queueByte(const uint8_t data)
	{
		if(_irq::isInterrupt()) {
			while(false == _irq::mTxBuffer.push(data));
			_irq::enableTxInterrupt();
		}
		else {
			loadTxReg(data);
		}
	}

	static bool readByte(uint8_t& data)
	{
		if(_irq::isInterrupt()) {
			return _irq::mRxBuffer.pop(data);
		}
		else {
			return readRxReg(data);
		}
	}


	static const void loadTxReg(const uint8_t data)
	{
		//Ensure we are ready
		while (!(REG_8(_iflag) & _txflag));
		REG_8(txbuf) = data;
		while (!(REG_8(_iflag) & _txflag));
	}

	static const bool readRxReg(uint8_t& data)
	{
		bool ret = false;
		if((REG_8(_iflag) & _rxflag)) {
			data = REG_8(rxbuf);
			ret = true;
		}
		return ret;
	}

	static void init(const uint8_t baudRate, uint8_t mod, UartClockSource source )
	{
		_rxpin::input(); _rxpin::selectOn(); _rxpin::select2On();
		_txpin::output(); _txpin::selectOn(); _txpin::select2On();


		REG_8(ctl1) |= UCSWRST;
		REG_8(ctl0) |= UCMODE_0;
		REG_8(br0) = baudRate;
		REG_8(mctl) = mod;
		REG_8(ctl1) |= static_cast<uint8_t>(source);
		REG_8(ctl1) &= ~UCSWRST;
		_irq::init();
		_irq::mRxBuffer.init();
		_irq::mTxBuffer.init();
		_irq::enableRxInterrupt();
	}

private:

	static constexpr uint8_t ctl0 = _ctl0;
	static constexpr uint8_t ctl1 = ctl0 + 1;
	static constexpr uint8_t br0 = ctl1 +1;
	static constexpr uint8_t br1 = br0 +1;
	static constexpr uint8_t mctl = br1 +1;
	static constexpr uint8_t stat = mctl +1;
	static constexpr uint8_t rxbuf = stat +1;
	static constexpr uint8_t txbuf = rxbuf +1;

};

//This macro is returning the number of arguments based on the placeholder N
//example is _VA_ARGS__ is 3 arguments that takes up _1,_2_3, then _4 through _8
//are taken up by 8-4 in the arguments returning 3.
#define VA_NARGS_IMPL(_1,_2,_3,_4,_5,_6,_7,_8,N,...) N
#define VA_NARGS(...) VA_NARGS_IMPL(__VA_ARGS__, 8, 7, 6, 5, 4, 3, 2, 1)


#define VARARG_IMPL2(base, count, ...) base##count(__VA_ARGS__)
#define VARARG_IMPL(base, count, ...) VARARG_IMPL2(base, count, __VA_ARGS__)
#define VARARG(base, ...) VARARG_IMPL(base, VA_NARGS(__VA_ARGS__), __VA_ARGS__)


#define PRINT(...) VARARG(Print,__VA_ARGS__)


#define Print1(_1) \
    uart::send(_1);
#define Print2(_1,_2) \
		Print1(_1) Print1(_2)
#define Print3(_1,_2,_3) \
		Print1(_1) Print2(_2,_3)
#define Print4(_1,_2,_3,_4) \
		Print1(_1) Print3(_2,_3,_4)
#define Print5(_1,_2,_3,_4,_5) \
		Print1(_1) Print4(_2,_3,_4,_5)
#define Print6(_1,_2,_3,_4,_5,_6) \
		Print1(_1) Print5(_2,_3,_4,_5,_6)
#define Print7(_1,_2,_3,_4,_5,_6,_7) \
		Print1(_1) Print6(_2,_3,_4,_5,_6,_7)
#define Print8(_1,_2,_3,_4,_5,_6,_7,_8) \
		Print1(_1) Print7(_2,_3,_4,_5,_6,_7,_8)

#define ENDL "\n"

template<typename X,typename Y>
constexpr uint8_t divide(X x, Y y) { return static_cast<uint32_t>(x) / static_cast<uint32_t>(y);  }
//constexpr uint8_t mod_calc(uint32_t x, uint16_t y) { return ((x * 16 / (uint16_t) y) - (x / (uint16_t) y * 16) + 1) / 2; }
template<typename X,typename Y>
constexpr uint8_t mod_calc(X x, Y y) { return (((static_cast<uint32_t>(x) / static_cast<uint32_t>(y)) - divide(x ,  y)) * 8) << 1; }

template<class _uart, BaudRate _rate, Speed _clock >
class McuUart
{
public:

	static const uint8_t BAUDREGISTER = (_rate == BaudRate::BAUD_9600) ? 3 : divide( _clock , _rate );
	static const uint8_t MODVALUE = (_rate == BaudRate::BAUD_9600) ? (UCBRS1 + UCBRS0) :mod_calc( _clock,  _rate );
	static const UartClockSource CLOCKSOURCE = (_rate == BaudRate::BAUD_9600) ? UartClockSource::ACLK : UartClockSource::SMCLK;

	static void init() {
			_uart::init(BAUDREGISTER, MODVALUE, CLOCKSOURCE);
	}

	template<class T>
	static const void send( const T data, const Base base=Base::BASE_HEX )
	{
		static char buf[sizeof(T)*3 +1];
		itoa(data,buf, base);
		send(static_cast<const char*>(buf));
	}

	static const void send( float data, int precision=2)
	{
		//TODO implement (look into fixed point) ??

	}

	static const void send(uint8_t* const data,const  int numOfBytes)
	{
		for(int i = 0; i < numOfBytes; ++i) {
			_uart::queueByte(data[i]);
		}
	}

	static const void send(const char* data)
	{
		int i = 0;
		while(data[i] != 0) {
			_uart::queueByte(data[i++]);
		}
	}

	static const void sendLine(const char* data=0)
	{
		if(data != 0)
			send(data);
		send("\n");
	}

	static const bool readByte(uint8_t& data)
	{
			return _uart::readByte(data);
	}

private:

};

}


using rx =  McuPeripheral::McuPin<McuPort1,BIT1>;
using tx =  McuPeripheral::McuPin<McuPort1,BIT2>;

//I hate to have to add this macro, but no other way could easily set this from config file
//and have the interrupt vector know.
//DEFAULT is interrupts off!
#ifdef UARTA0_ENABLE_INT
using UartA0_Irq = McuPeripheral::Interrupts<IE2_,UCA0TXIE, UCA0RXIE>;
#else
using UartA0_Irq = McuPeripheral::FakeInterupts<false>;
#endif

using UartA0 = McuPeripheral::UartControl<rx, tx, UartA0_Irq, UCA0CTL0_, IFG2_, UCA0TXIFG, UCA0RXIFG >;

#endif //_MSP_UART_HPP
