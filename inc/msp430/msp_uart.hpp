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
		_rxpin::input(); _rxpin::select_on(); _rxpin::select2_on();
		_txpin::output(); _txpin::select_on(); _txpin::select2_on();


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


#if 0
	static FifoBuffer<uint8_t, 8, McuPeripheral::DisableInterrupt<_ienable,_txen>, EnableInterrupt<_ienable,_txen> > mTxBuffer;
	static FifoBuffer<uint8_t, 8, McuPeripheral::DisableInterrupt<_ienable,_rxen>, EnableInterrupt<_ienable,_rxen> > mRxBuffer;

	static DisableInterrupt<_ienable,_txen> disableTxInterrupt;
	static DisableInterrupt<_ienable,_rxen> disableRxInterrupt;
	static EnableInterrupt<_ienable,_txen> enableTxInterrupt;
	static EnableInterrupt<_ienable,_rxen> enableRxInterrupt;
#endif

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


template<typename X,typename Y>
constexpr uint8_t divide(X x, Y y) { return static_cast<uint32_t>(x) / static_cast<uint32_t>(y);  }
//constexpr uint8_t mod_calc(uint32_t x, uint16_t y) { return ((x * 16 / (uint16_t) y) - (x / (uint16_t) y * 16) + 1) / 2; }
template<typename X,typename Y>
constexpr uint8_t mod_calc(X x, Y y) { return (((static_cast<uint32_t>(x) / static_cast<uint32_t>(y)) - divide(x ,  y)) * 8) << 1; }

template<class _uart, BaudRate _rate, Speed _clock >
class McuUart : Uart
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
			queueByte(data[i]);
		}
	}

	static const void send(const char* data)
	{
		int i = 0;
		while(data[i] != 0) {
			queueByte(data[i++]);
		}
	}

	static const void queueByte(const uint8_t data)
	{
		//if() {
			//while(false == _uart::mTxBuffer.push(data));
			//_uart::enableTxInterrupt();
		//}
		//else {
			_uart::loadTxReg(data);
		//}
	}

	static const void sendLine(const char* data=0)
	{
		if(data != 0)
			send(data);
		send("\n");
	}

	static const bool readByte(uint8_t& data)
	{
		//if(_enableInt) {
			//return _uart::mRxBuffer.pop(data);
		//}
		//else {
			return _uart::readRxReg(data);
		//}
	}

private:

};

#if 0
template< class _rxpin, class _txpin, uint8_t _ctl0, uint8_t _ienable, uint8_t _iflag, uint8_t _txflag, uint8_t _rxflag, uint8_t _txen, uint8_t _rxen>
FifoBuffer<uint8_t, 8, McuPeripheral::DisableInterrupt<_ienable,_txen>, EnableInterrupt<_ienable,_txen> > UartControl<_rxpin, _txpin,  _ctl0, _ienable, _iflag, _txflag, _rxflag, _txen, _rxen>::mTxBuffer;
template< class _rxpin, class _txpin, uint8_t _ctl0, uint8_t _ienable, uint8_t _iflag, uint8_t _txflag, uint8_t _rxflag, uint8_t _txen, uint8_t _rxen>
FifoBuffer<uint8_t, 8, McuPeripheral::DisableInterrupt<_ienable,_rxen>, EnableInterrupt<_ienable,_rxen> > UartControl<_rxpin, _txpin,  _ctl0, _ienable, _iflag, _txflag, _rxflag, _txen, _rxen>::mRxBuffer;

template< class _rxpin, class _txpin, uint8_t _ctl0, uint8_t _ienable, uint8_t _iflag, uint8_t _txflag, uint8_t _rxflag, uint8_t _txen, uint8_t _rxen>
DisableInterrupt<_ienable,_txen> UartControl<_rxpin, _txpin,  _ctl0, _ienable, _iflag, _txflag, _rxflag, _txen, _rxen>::disableTxInterrupt;
template< class _rxpin, class _txpin, uint8_t _ctl0, uint8_t _ienable, uint8_t _iflag, uint8_t _txflag, uint8_t _rxflag, uint8_t _txen, uint8_t _rxen>
DisableInterrupt<_ienable,_rxen> UartControl<_rxpin, _txpin,  _ctl0, _ienable, _iflag, _txflag, _rxflag, _txen, _rxen>::disableRxInterrupt;
template< class _rxpin, class _txpin, uint8_t _ctl0, uint8_t _ienable, uint8_t _iflag, uint8_t _txflag, uint8_t _rxflag, uint8_t _txen, uint8_t _rxen>
EnableInterrupt<_ienable,_txen>  UartControl<_rxpin, _txpin,  _ctl0, _ienable, _iflag, _txflag, _rxflag, _txen, _rxen>::enableTxInterrupt;
template< class _rxpin, class _txpin, uint8_t _ctl0, uint8_t _ienable, uint8_t _iflag, uint8_t _txflag, uint8_t _rxflag, uint8_t _txen, uint8_t _rxen>
EnableInterrupt<_ienable,_rxen>  UartControl<_rxpin, _txpin,  _ctl0, _ienable, _iflag, _txflag, _rxflag, _txen, _rxen>::enableRxInterrupt;
#endif
}


using rx =  McuPeripheral::McuPin<McuPort1,BIT1>;
using tx =  McuPeripheral::McuPin<McuPort1,BIT2>;

//I hate to have to add this macro, but no other way could easily set this from config file
//and have the interrupt vector know.
//DEFAULT is interrupts off!
#ifdef UARTA0_ENABLE_INT
using UartA0_Irq = McuPeripheral::Interrupts<IE2_,UCA0TXIE, UCA0RXIE>;
#else
using UartA0_Irq = McuPeripheral::FakeInterupts;
#endif

using UartA0 = McuPeripheral::UartControl<rx, tx, UartA0_Irq, UCA0CTL0_, IFG2_, UCA0TXIFG, UCA0RXIFG >;

#endif //_MSP_UART_HPP
