#ifndef _MSP_SPI_HPP
#define _MSP_SPI_HPP
#include "mcu_spi.hpp"
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



template< class _misopin, class _simopin, class _clkpin, uint8_t _ctl0, uint8_t _ienable, uint8_t _iflag, uint8_t _txflag,  uint8_t _rxflag, uint8_t _txen, uint8_t _rxen>
class SpiControl
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

	static void init(const uint8_t divide, SpiClock clock, SpiMaster master, SpiBitOrder order )
	{
		_misopin::input(); _misopin::select_on(); _misopin::select2_on();
		_simopin::output(); _simopin::select_on(); _simopin::select2_on();
		_clkpin::output(); _clkpin::select_on(); _clkpin::select2_on();


		REG_8(ctl1) |= UCSWRST;
		REG_8(_ienable) &= ~(_rxen | _txen);
		REG_8(ctl0) |= static_cast<uint8_t>(clock) | static_cast<uint8_t>(order) | static_cast<uint8_t>(master) | UCMODE_0 | UCSYNC;
		REG_8(br0) = divide;
		REG_8(mctl) = 0;
		REG_8(ctl1) |= UCSSEL_2; //smclk
		REG_8(ctl1) &= ~UCSWRST;
		mRxBuffer.init();
		mTxBuffer.init();
	}

	static FifoBuffer<uint8_t, 8, McuPeripheral::DisableInterrupt<_ienable,_txen>, EnableInterrupt<_ienable,_txen> > mTxBuffer;
	static FifoBuffer<uint8_t, 8, McuPeripheral::DisableInterrupt<_ienable,_rxen>, EnableInterrupt<_ienable,_rxen> > mRxBuffer;

	static DisableInterrupt<_ienable,_txen> disableTxInterrupt;
	static DisableInterrupt<_ienable,_rxen> disableRxInterrupt;
	static EnableInterrupt<_ienable,_txen> enableTxInterrupt;
	static EnableInterrupt<_ienable,_rxen> enableRxInterrupt;

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


template<class _spi, SpiClock _clk, SpiMaster _master, SpiBitOrder _order, uint8_t _clkdivide, bool _ienable=false >
class McuSpi : Spi
{
public:

	static const uint8_t BAUDREGISTER = _clkdivide;

	static void init() {
		_spi::init(BAUDREGISTER, _clk, _master, _order);
		if(_ienable) {
			_spi::enableRxInterrupt();
		}
	}

	template<class T>
	static const void send( const T data )
	{
		const uint8_t* data_ptr = reinterpret_cast<const uint8_t*>(&data);
		for(int i = 0; i < sizeof(data); ++i) {
			queueByte(data_ptr[i]);
		}
	}


	static const void queueByte(const uint8_t data)
	{
		if(_ienable) {
			while(false == _spi::mTxBuffer.push(data));
			_spi::enableTxInterrupt();
		}
		else {
			_spi::loadTxReg(data);
		}
	}

	template<class T>
	static const bool read(T& data)
	{
		if(_ienable) {
			return _spi::mRxBuffer.pop(data);
		}
		else {
			return _spi::readRxReg(data);
		}
	}

private:

};


template< class _misopin, class _simopin, class _clkpin, uint8_t _ctl0, uint8_t _ienable, uint8_t _iflag, uint8_t _txflag,  uint8_t _rxflag, uint8_t _txen, uint8_t _rxen>
FifoBuffer<uint8_t, 8, McuPeripheral::DisableInterrupt<_ienable,_txen>, EnableInterrupt<_ienable,_txen> > SpiControl<_misopin, _simopin, _clkpin,  _ctl0, _ienable, _iflag, _txflag, _rxflag, _txen, _rxen>::mTxBuffer;
template< class _misopin, class _simopin, class _clkpin, uint8_t _ctl0, uint8_t _ienable, uint8_t _iflag, uint8_t _txflag,  uint8_t _rxflag, uint8_t _txen, uint8_t _rxen>
FifoBuffer<uint8_t, 8, McuPeripheral::DisableInterrupt<_ienable,_rxen>, EnableInterrupt<_ienable,_rxen> > SpiControl<_misopin, _simopin, _clkpin,  _ctl0, _ienable, _iflag, _txflag, _rxflag, _txen, _rxen>::mRxBuffer;

template< class _misopin, class _simopin, class _clkpin, uint8_t _ctl0, uint8_t _ienable, uint8_t _iflag, uint8_t _txflag,  uint8_t _rxflag, uint8_t _txen, uint8_t _rxen>
DisableInterrupt<_ienable,_txen> SpiControl<_misopin, _simopin, _clkpin,  _ctl0, _ienable, _iflag, _txflag, _rxflag, _txen, _rxen>::disableTxInterrupt;
template< class _misopin, class _simopin, class _clkpin, uint8_t _ctl0, uint8_t _ienable, uint8_t _iflag, uint8_t _txflag,  uint8_t _rxflag, uint8_t _txen, uint8_t _rxen>
DisableInterrupt<_ienable,_rxen> SpiControl<_misopin, _simopin, _clkpin,  _ctl0, _ienable, _iflag, _txflag, _rxflag, _txen, _rxen>::disableRxInterrupt;
template< class _misopin, class _simopin, class _clkpin, uint8_t _ctl0, uint8_t _ienable, uint8_t _iflag, uint8_t _txflag,  uint8_t _rxflag, uint8_t _txen, uint8_t _rxen>
EnableInterrupt<_ienable,_txen>  SpiControl<_misopin, _simopin, _clkpin,  _ctl0, _ienable, _iflag, _txflag, _rxflag, _txen, _rxen>::enableTxInterrupt;
template< class _misopin, class _simopin, class _clkpin, uint8_t _ctl0, uint8_t _ienable, uint8_t _iflag, uint8_t _txflag,  uint8_t _rxflag, uint8_t _txen, uint8_t _rxen>
EnableInterrupt<_ienable,_rxen>  SpiControl<_misopin, _simopin, _clkpin,  _ctl0, _ienable, _iflag, _txflag, _rxflag, _txen, _rxen>::enableRxInterrupt;

}


using miso =  McuPeripheral::McuPin<McuPort1,BIT6>;
using mosi =  McuPeripheral::McuPin<McuPort1,BIT7>;
using clk =  McuPeripheral::McuPin<McuPort1,BIT5>;
using SpiB0 = McuPeripheral::SpiControl<miso, mosi, clk, UCB0CTL0_, IE2_, IFG2_, UCB0TXIFG, UCB0RXIFG, UCB0TXIE, UCB0RXIE >;

#endif //_MSP_SPI_HPP
