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



template< class _rxPin, class _txPin, uint8_t _ctl0, uint8_t _ctl1,  uint8_t _br0, uint8_t _br1, uint8_t _mctl, uint8_t _rxbuff, uint8_t _txbuff,  uint8_t _status, uint8_t _ienable, uint8_t _iflag>
class UartControl
{
public:

	static const void loadTxReg(const uint8_t data)
	{
		//Ensure we are ready
		while (!(REG_8(_iflag) & UCA0TXIFG));
		REG_8(_txbuff) = data;
		while (!(REG_8(_iflag) & UCA0TXIFG));
	}

	static const bool readRxReg(uint8_t& data)
	{
		bool ret = false;
		if((REG_8(_iflag) & UCA0RXIFG)) {
			data = REG_8(_rxbuff);
			ret = true;
		}
		return ret;
	}

	static void init(const uint8_t baudRate, uint8_t mod, UartClockSource source )
	{
		_rxPin::input(); _rxPin::select_on(); _rxPin::select2_on();
		_txPin::output(); _txPin::select_on(); _txPin::select2_on();


		REG_8(_ctl1) |= UCSWRST;
		REG_8(_ienable) &= ~(UCA0TXIE | UCA0TXIE);
		REG_8(_ctl0) |= UCMODE_0;
		REG_8(_br0) = baudRate;
		REG_8(_mctl) = mod;
		REG_8(_ctl1) |= static_cast<uint8_t>(source);
		REG_8(_ctl1) &= ~UCSWRST;
		mRxBuffer.init();
		mTxBuffer.init();
	}

	static FifoBuffer<uint8_t, 8, McuPeripheral::DisableInterrupt<_ienable,UCA0TXIE>, EnableInterrupt<_ienable,UCA0TXIE> > mTxBuffer;
	static FifoBuffer<uint8_t, 8, McuPeripheral::DisableInterrupt<_ienable,UCA0RXIE>, EnableInterrupt<_ienable,UCA0RXIE> > mRxBuffer;

	static DisableInterrupt<_ienable,UCA0TXIE> disableTxInterrupt;
	static DisableInterrupt<_ienable,UCA0RXIE> disableRxInterrupt;
	static EnableInterrupt<_ienable,UCA0TXIE> enableTxInterrupt;
	static EnableInterrupt<_ienable,UCA0RXIE> enableRxInterrupt;

private:

	static __attribute__((__interrupt__)) void Uart_Rx(void);
	static __attribute__((__interrupt__)) void Uart_Tx(void);

};


template<typename X,typename Y>
constexpr uint8_t divide(X x, Y y) { return static_cast<uint32_t>(x) / static_cast<uint32_t>(y);  }
//constexpr uint8_t mod_calc(uint32_t x, uint16_t y) { return ((x * 16 / (uint16_t) y) - (x / (uint16_t) y * 16) + 1) / 2; }
template<typename X,typename Y>
constexpr uint8_t mod_calc(X x, Y y) { return (((static_cast<uint32_t>(x) / static_cast<uint32_t>(y)) - divide(x ,  y)) * 8) << 1; }

template<class _uart, BaudRate _rate, Speed _clock, bool _enableInt=false >
class McuUart : Uart
{
public:

	static const uint8_t BAUDREGISTER = (_rate == BaudRate::BAUD_9600) ? 3 : divide( _clock , _rate );
	static const uint8_t MODVALUE = (_rate == BaudRate::BAUD_9600) ? (UCBRS1 + UCBRS0) :mod_calc( _clock,  _rate );
	static const UartClockSource CLOCKSOURCE = (_rate == BaudRate::BAUD_9600) ? UartClockSource::ACLK : UartClockSource::SMCLK;

	static void init() {
			_uart::init(BAUDREGISTER, MODVALUE, CLOCKSOURCE);
			if(_enableInt) {
				_uart::enableRxInterrupt();
			}
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
		if(_enableInt) {
			while(false == _uart::mTxBuffer.push(data));
			_uart::enableTxInterrupt();
		}
		else {
			_uart::loadTxReg(data);
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
		if(_enableInt) {
			return _uart::mRxBuffer.pop(data);
		}
		else {
			return _uart::readRxReg(data);
		}
	}

private:

};


template< class _rxPin, class _txPin, uint8_t _ctl0, uint8_t _ctl1,  uint8_t _br0, uint8_t _br1, uint8_t _mctl, uint8_t _rxbuff, uint8_t _txbuff,  uint8_t _status, uint8_t _ienable, uint8_t _iflag>
FifoBuffer<uint8_t, 8, McuPeripheral::DisableInterrupt<_ienable,UCA0TXIE>, EnableInterrupt<_ienable,UCA0TXIE> > UartControl<_rxPin, _txPin,  _ctl0, _ctl1,   _br0, _br1, _mctl, _rxbuff, _txbuff,  _status, _ienable, _iflag>::mTxBuffer;
template< class _rxPin, class _txPin, uint8_t _ctl0, uint8_t _ctl1,  uint8_t _br0, uint8_t _br1, uint8_t _mctl, uint8_t _rxbuff, uint8_t _txbuff,  uint8_t _status, uint8_t _ienable, uint8_t _iflag>
FifoBuffer<uint8_t, 8, McuPeripheral::DisableInterrupt<_ienable,UCA0RXIE>, EnableInterrupt<_ienable,UCA0RXIE> > UartControl<_rxPin, _txPin,  _ctl0, _ctl1,   _br0, _br1, _mctl, _rxbuff, _txbuff,  _status, _ienable, _iflag>::mRxBuffer;

template< class _rxPin, class _txPin, uint8_t _ctl0, uint8_t _ctl1,  uint8_t _br0, uint8_t _br1, uint8_t _mctl, uint8_t _rxbuff, uint8_t _txbuff,  uint8_t _status, uint8_t _ienable, uint8_t _iflag>
DisableInterrupt<_ienable,UCA0TXIE> UartControl<_rxPin, _txPin,  _ctl0, _ctl1,   _br0, _br1, _mctl, _rxbuff, _txbuff,  _status, _ienable, _iflag>::disableTxInterrupt;
template< class _rxPin, class _txPin, uint8_t _ctl0, uint8_t _ctl1,  uint8_t _br0, uint8_t _br1, uint8_t _mctl, uint8_t _rxbuff, uint8_t _txbuff,  uint8_t _status, uint8_t _ienable, uint8_t _iflag>
DisableInterrupt<_ienable,UCA0RXIE> UartControl<_rxPin, _txPin,  _ctl0, _ctl1,   _br0, _br1, _mctl, _rxbuff, _txbuff,  _status, _ienable, _iflag>::disableRxInterrupt;
template< class _rxPin, class _txPin, uint8_t _ctl0, uint8_t _ctl1,  uint8_t _br0, uint8_t _br1, uint8_t _mctl, uint8_t _rxbuff, uint8_t _txbuff,  uint8_t _status, uint8_t _ienable, uint8_t _iflag>
EnableInterrupt<_ienable,UCA0TXIE> UartControl<_rxPin, _txPin,  _ctl0, _ctl1,   _br0, _br1, _mctl, _rxbuff, _txbuff,  _status, _ienable, _iflag>::enableTxInterrupt;
template< class _rxPin, class _txPin, uint8_t _ctl0, uint8_t _ctl1,  uint8_t _br0, uint8_t _br1, uint8_t _mctl, uint8_t _rxbuff, uint8_t _txbuff,  uint8_t _status, uint8_t _ienable, uint8_t _iflag>
EnableInterrupt<_ienable,UCA0RXIE> UartControl<_rxPin, _txPin,  _ctl0, _ctl1,   _br0, _br1, _mctl, _rxbuff, _txbuff,  _status, _ienable, _iflag>::enableRxInterrupt;

}


using rx =  McuPeripheral::McuPin<McuPort1,BIT1>;
using tx =  McuPeripheral::McuPin<McuPort1,BIT2>;
using UartA0 = McuPeripheral::UartControl<rx, tx, UCA0CTL0_, UCA0CTL1_, UCA0BR0_, UCA0BR1_, UCA0MCTL_, UCA0RXBUF_, UCA0TXBUF_, UCA0STAT_, IE2_, IFG2_>;

#endif //_MSP_UART_HPP
