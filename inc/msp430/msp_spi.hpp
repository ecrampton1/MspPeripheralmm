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



template< class _misopin, class _simopin, class _clkpin, class _irq, uint8_t _ctl0, uint8_t _iflag, uint8_t _txflag,  uint8_t _rxflag>
class SpiControl
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

	static void flushTx()
	{
		_irq::mTxBuffer.flush();
	}


	static void flushRx()
	{
		_irq::mRxBuffer.flush();
	}

	static bool isBusy()
	{
		return (REG_8(stat) & UCBUSY);
	}


	static void loadTxReg(const uint8_t data)
	{
		//Ensure we are ready
		while (!(REG_8(_iflag) & _txflag));
		REG_8(txbuf) = data;
		while (!(REG_8(_iflag) & _txflag));
	}

	static bool readRxReg(uint8_t& data)
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
		_misopin::input(); _misopin::selectOn(); _misopin::select2On();
		_simopin::output(); _simopin::selectOn(); _simopin::select2On();
		_clkpin::output(); _clkpin::selectOn(); _clkpin::select2On();


		REG_8(ctl1) |= UCSWRST;
		REG_8(ctl0) |= static_cast<uint8_t>(clock) | static_cast<uint8_t>(order) | static_cast<uint8_t>(master) | UCMODE_0 | UCSYNC;
		REG_8(br0) = divide;
		REG_8(mctl) = 0;
		REG_8(ctl1) |= UCSSEL_2; //smclk
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


template<class _spi, SpiClock _clk,  SpiBitOrder _order, uint8_t _divide, SpiMaster _master=SpiMaster::SPI_MASTER>
class McuSpi
{
public:

	static void init() {
		_spi::init(_divide, _clk, _master, _order);
	}

	template<class T>
	static bool read( T& data )
	{
		bool ret;
		uint8_t* data_ptr = reinterpret_cast<uint8_t*>(&data);
		for(uint16_t i = 0; i < sizeof(T); ++i) {
			ret = _spi::readByte(data_ptr[i]);
			if(ret == false)
				break;
		}
		return ret;
	}

	template<class T>
	static int read( const T* data, int size)
	{
		bool ret;
		int i;
		for(i = 0; i < size; ++i) {
			ret = read(data[i]);
			if(ret == false)
				break;
		}
		return i;
	}



	template<class T>
	static int send( const T* data, int size)
	{
		int i;
		for(i = 0; i < size; ++i) {
			sendMSB(data[i]);
		}
		return i;
	}

	//Be careful sends this out in the endian order
	//TODO make this a bit more robust to endianess and LSB vs MSB
	//Linker error with uint8_t? no idea...
	template<class T>
	static void send( const T data )
	{
		if(sizeof(T) == 1) {
			_spi::queueByte(data);
		}
		else {
			const uint8_t* data_ptr = reinterpret_cast<const uint8_t*>(&data);
			for(uint16_t i = 0; i < sizeof(T); ++i) {
				_spi::queueByte(data_ptr[i]);
			}
		}
	}


	template<class T>
	static T exchange( const T data )
	{
		T return_data = 0;

		const uint8_t* data_ptr = &data;
		uint8_t* ret_ptr = &return_data;
		for(uint16_t i = 0; i < sizeof(data); ++i) {
			_spi::queueByte(data_ptr[i]);
			while(_spi::isBusy());
			if(!_spi::readByte(ret_ptr[i]))
				break;
		}

		return return_data;
	}

	template<class T>
	static int exchange( T* data, T* return_data, int size)
	{
		int i;
		for(i = 0; i < size; ++i) {
			return_data[i] = exchangeMSB(data[i]);
		}
		return i;
	}

	static void flushTx()
	{
		_spi::flushTx();
	}

	static void flushRx()
	{
		_spi::flushRx();
	}


private:

	template<class T>
	static void sendMSB(const T data)
	{
		const uint8_t* data_ptr = reinterpret_cast<const uint8_t*>(&data);
		for(uint16_t i = 0; i < sizeof(T); ++i) {
			_spi::queueByte(data_ptr[i]);
		}
	}

	template<class T>
	static void sendLSB(const T data)
	{
		const uint8_t* data_ptr = reinterpret_cast<const uint8_t*>(&data);
		for(int16_t i = sizeof(T)-1; i >= 0; --i) {
			_spi::queueByte(data_ptr[i]);
		}
	}


	template<class T>
	static int sendMSB( T* data, int size)
	{
		int i;
		for(i = 0; i < size; ++i) {
			sendMSB(data[i]);
		}
		return i;
	}

	template<class T>
	static int sendLSB( T* data, int size)
	{
		int i;
		for(i = size-1; i >= 0; --i) {
			sendLSB(data[i]);
		}
		return size;
	}


	template<class T>
	static T exchangeMSB(T data)
	{
		T return_data = 0;

		uint8_t* data_ptr = static_cast<uint8_t*>(&data);
		uint8_t* ret_ptr = &return_data;
		for(uint16_t i = 0; i < sizeof(data); ++i) {
			_spi::queueByte(data_ptr[i]);
			while(_spi::isBusy());
			if(!_spi::readByte(ret_ptr[i]))
				break;
		}

		return return_data;
	}


	template<class T>
	static T exchangeLSB(T data)
	{
		T return_data = 0;

		uint8_t* data_ptr = static_cast<uint8_t*>(&data);
		uint8_t* ret_ptr = &return_data;
		for(int16_t i = sizeof(data)-1; i >= 0; --i) {
			_spi::queueByte(data_ptr[i]);
			while(_spi::isBusy());
			if(!_spi::readByte(ret_ptr[i]))
				break;
		}

		return return_data;
	}


	template<class T>
	static int exchangeMSB( T* data, T* return_data, int size)
	{
		int i;
		for(i = 0; i < size; ++i) {
			return_data[i] = exchangeMSB(data[i]);
		}
		return i;
	}

	template<class T>
	static int exchangeLSB( T* data, T* return_data, int size)
	{
		int i;
		for(i = size-1; i >= 0; --i) {
			return_data[i] = exchangeMSB(data[i]);
		}
		return size;
	}

};


}


using miso =  McuPeripheral::McuPin<McuPort1,BIT6>;
using mosi =  McuPeripheral::McuPin<McuPort1,BIT7>;
using clk =  McuPeripheral::McuPin<McuPort1,BIT5>;

//I hate to have to add this macro, but no other way could easily set this from config file
//and have the interrupt vector know.
//DEFAULT is interrupts off!
#ifdef SPIB0_ENABLE_INT
using UartB0_Irq = McuPeripheral::Interrupts<IE2_,UCB0TXIE, UCB0RXIE>;
#else
using UartB0_Irq = McuPeripheral::FakeInterupts<false>;
#endif

using SpiB0 = McuPeripheral::SpiControl<miso, mosi, clk, UartB0_Irq, UCB0CTL0_, IFG2_, UCB0TXIFG, UCB0RXIFG >;

#endif //_MSP_SPI_HPP
