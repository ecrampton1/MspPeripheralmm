#ifndef _NRF24_HPP
#define _NRF24_HPP
#include <string.h>
#include "nrf24/nrf24_comm.hpp"

constexpr uint8_t pipeToBit(Nrf24Pipe pipe)
{
	return (1 << (static_cast<uint8_t>(pipe) - 0x0A));
}

constexpr bool enableDynamicPayload(uint8_t size)
{
	return (size > 32 || size == 0) ? true : false;
}

constexpr uint8_t AUTO_ACK_BITS = 0x3F;

template< class _spi, class _sys, class _uart, class _cs, class _ce, class _irq, uint8_t _payloadsize, Nrf24AddressWidth _addrwidth, uint8_t _channel, Nrf24Speed _speed >
class Nrf24
{
public:

	using nrf_comm = Nrf24Comm< _spi, _cs >;

	static void begin()
	{
		nrf_comm::init();

		_ce::output();
		_irq::input();
		_ce::clear();
		_sys::delayInUs(100000);

		turnOnFeatures();
		nrf_comm::writeConfig(0x0);
		mAddressWidth = _addrwidth;
		setAddressWidth();
		setCrc();
		setAutoRetransmit();
		setChannel();
		setSpeed();
		if(enableDynamicPayload(_payloadsize)) {
			enableDynamicPl();
			for(uint8_t i = static_cast<uint8_t>(Nrf24Pipe::RX_PIPE0); i <= static_cast<uint8_t>(Nrf24Pipe::TX_PIPE); ++i)
			{
				enableDynamicPl(static_cast<Nrf24Pipe>(i));
				setPayloadSize(static_cast<Nrf24Pipe>(i),0x20);
			}
		}
		else {
			disableDynamicPl();
			for(uint8_t i = static_cast<uint8_t>(Nrf24Pipe::RX_PIPE0); i <= static_cast<uint8_t>(Nrf24Pipe::TX_PIPE); ++i)
			{
				setPayloadSize(static_cast<Nrf24Pipe>(i),_payloadsize);
			}
		}
		nrf_comm::flushTx();
		nrf_comm::flushRx();
		clearIrq(RF24_IRQ_MASK);
		powerUp();
		enableAutoAck();
		_sys::delayInUs(5000);
	}

	/*
	 * @param ard The delay between auto retransmission in increments of 250us 0x00-0x0F (250us-4000us)
	 * @param arc The number of retries to attempt for a single transmission 0(disabled) -15
	 */
	static void setAutoRetransmit(uint8_t ard = 5, uint8_t arc = 15)
	{
		nrf_comm::writeSetupRetr(ard << 4 | arc );
	}

	static void setCrc(Nrf24Crc crc = Nrf24Crc::CRC_16)
	{
		nrf_comm::writeConfig( nrf_comm::readConfig() | static_cast<uint8_t>(crc));
	}

	static void setChannel(const uint8_t channel = _channel)
	{
		nrf_comm::writeRfCh(channel);
	}

	static void setSpeed(const Nrf24Speed speed = _speed)
	{
		nrf_comm::writeRfSetup( (nrf_comm::readRfSetup() & ~static_cast<uint8_t>(Nrf24Speed::RF_SPEED_RESET)) | static_cast<uint8_t>(speed));
	}

	static void flushTx() { nrf_comm::flushTx(); }
	static void flushRx() { nrf_comm::flushRx(); }

	//Opens a pipe by calling the helper function, this adds an extra
	//element to help store Rx Pipe 0 so when performing a write this
	//will save the pipe0 address and re write it down after the write.
	static int openPipe(const Nrf24Pipe pipe, uint8_t* const address)
	{
		if(pipe == Nrf24Pipe::TX_PIPE)
		{
			memcpy(mTxPipeAddress,address,static_cast<uint8_t>(mAddressWidth));
		}
		else if(pipe == Nrf24Pipe::RX_PIPE0)
		{
			memcpy(mRxPipeAddress0,address,static_cast<uint8_t>(mAddressWidth));
		}
		return openPipeHelper(pipe,address);
	}


	static void enableRxPipe(const Nrf24Pipe pipe)
	{
		nrf_comm::writeEnRxAddr(nrf_comm::readEnRxAddr() | pipeToBit(pipe));
	}

	static void disableRxPipe(const Nrf24Pipe pipe)
	{
		nrf_comm::writeEnRxAddr(nrf_comm::readEnRxAddr() & ~(pipeToBit(pipe)));
	}

	static void disableDynamicPl()
	{
		nrf_comm::writeFeature(nrf_comm::readFeature() & ~RF24_EN_DPL);
	}

	static void enableDynamicPl()
	{
		nrf_comm::writeFeature(nrf_comm::readFeature() | (RF24_EN_DPL ));
	}

	static void enableDynamicPl(const Nrf24Pipe pipe)
	{
		nrf_comm::writeDynPd(nrf_comm::readDynPd() | pipeToBit(pipe));
	}

	static bool disableRx()
	{
		_ce::clear();
		_sys::delayInUs(100);
		uint8_t reg = nrf_comm::readConfig();
		nrf_comm::writeConfig( reg & ~static_cast<uint8_t>(RF24_PRIM_RX));
		return reg & RF24_PRIM_RX; //return if it was in rx mode
	}

	static void enableRx()
	{
		nrf_comm::writeConfig( nrf_comm::readConfig() | static_cast<uint8_t>(RF24_PRIM_RX));
		_ce::set();
	}

	static void disableDynamicPl(const Nrf24Pipe pipe)
	{
		nrf_comm::writeDynPd(nrf_comm::readDynPd() & ~(pipeToBit(pipe)));
	}

	static void setPayloadSize(const  Nrf24Pipe pipe, const uint8_t size)
	{
		nrf_comm::writeRxPw(pipe,size);
	}

	static uint8_t getRxPayloadSize( )
	{
		return nrf_comm::readTopFifoRxWidth();
	}

	static void enableAutoAck()
	{
		nrf_comm::writeEnAA(AUTO_ACK_BITS);
	}

	static void enableAutoAck(const Nrf24Pipe pipe)
	{
		nrf_comm::writeEnAA(nrf_comm::readEnAA() | pipeToBit(pipe));
	}

	static void disableAutoAck()
	{
		nrf_comm::writeEnAA(0x00);
	}

	static void disableAutoAck(const Nrf24Pipe pipe)
	{
		nrf_comm::writeEnAA(nrf_comm::readEnAA() & ~(pipeToBit(pipe)));
	}

	template<class T>
	static bool writePayload(T* const buf,const int size, bool noAck=false)
	{
		/*_uart::send("w: ");
		for(int i = 0; i < size; ++i){
			_uart::send((int)buf[i]);
			_uart::send(",");
		}
		_uart::sendLine();*/
		//if radio was listening before re-enable at the end
		bool rxWasEnabled = disableRx();
		if(!noAck) { //ack enabled
			openPipeHelper(Nrf24Pipe::RX_PIPE0,mTxPipeAddress);
			nrf_comm::writePayload(buf, size);
		}else {//No ack enabled
			nrf_comm::writeNoTxAck(buf, size);
		}

		_ce::set();
		waitForTxFifoEmpty();

		if(rxWasEnabled) {
			openPipeHelper(Nrf24Pipe::RX_PIPE0,mRxPipeAddress0);
			enableRx();
		}
		return mLastTxPassed;
	}

	template<class T>
	static int readPayload(T* const buf, const int size)
	{
		uint8_t length = nrf_comm::readTopFifoRxWidth();
		length = size < nrf_comm::readTopFifoRxWidth() ? size : length;
		int ret = nrf_comm::readPayload(buf, length);
		return ret;
	}

	static bool rxPayloadAvaliable(Nrf24Pipe &pipe)
	{
		if(!isRxEmpty()) {
			uint8_t rx_pipe = nrf_comm::readStatus() & RF24_MASK_RX_P;
			//FIFO Empty = 0x0E Not Used bits 0x0C
			if(rx_pipe == 0x0E || rx_pipe == 0x0C) {
				return false;
			}
			pipe = static_cast<Nrf24Pipe>(rx_pipe + RF24_RX_ADDR_P0);
		}
		else {
			return false;
		}
		return true;
	}

	static bool dynamicPayloadsEnabled()
	{
		return enableDynamicPayload(_payloadsize);
	}

	static bool isRxEmpty()
	{
		uint8_t status = nrf_comm::readFifoStatus();
		return (status & RF24_RX_EMPTY);
	}

	static bool isRxFifoFull()
	{
		uint8_t status = nrf_comm::readFifoStatus();
		return (status & RF24_RX_FULL);
	}

	static bool isTxFifoFull()
	{
		uint8_t status = nrf_comm::readFifoStatus();
		return (status & RF24_FIFO_FULL);
	}

	static void powerUp()
	{
		nrf_comm::writeConfig( nrf_comm::readConfig() | static_cast<uint8_t>(RF24_PWR_UP));
	}

	static void powerDown()
	{
		nrf_comm::writeConfig( nrf_comm::readConfig() & ~static_cast<uint8_t>(RF24_PWR_UP));
	}

	static void checkAndClearIrq()
	{
		readLastIrq();

		if(mLastIRQ & RF24_MASK_TX_DS) {
			mLastTxPassed = true;
		}

		//TX Failed :(
		if(mLastIRQ & RF24_MASK_MAX_RT) {
			mLastTxPassed = false;
		}

		if(mLastIRQ & RF24_MASK_RX_DR) {
			mReceivedRxIrq = true;
		}

		clearIrq(mLastIRQ);
	}

	static void clearIrq(uint8_t flags)
	{
		nrf_comm::writeStatus(flags & RF24_IRQ_MASK);
	}

	static void printStatus()
	{
		int status = nrf_comm::readStatus();
		_uart::send("Status     : RX_DR=");
		_uart::send((status & RF24_MASK_RX_DR) ? 1:0);
		_uart::send(" TX_DS=");
		_uart::send((status & RF24_MASK_TX_DS) ? 1:0);
		_uart::send(" MAX_RT=");
		_uart::send((status & RF24_MASK_MAX_RT) ? 1:0);
		_uart::send(" RX_P_NO=");
		_uart::send(((status >> RF24_RX_P_NO) & 0x07));
		_uart::send(" TX_FULL=");
		_uart::send((status & RF24_TX_FULL) ? 1:0);
		_uart::sendLine();
	}

	static void setIrqCallback()
	{

	}

	static void printInfo()
	{
		uint8_t buffer[static_cast<uint8_t>(_addrwidth)];
				int i = static_cast<uint8_t>(Nrf24Pipe::RX_PIPE0);

		printStatus();


		while(i <= static_cast<uint8_t>(Nrf24Pipe::TX_PIPE )) {

			int size = nrf_comm::readPipeAddr(static_cast<Nrf24Pipe>(i),buffer,mAddressWidth);
			if(size == 0) {
				return;
			}
			if(i == static_cast<uint8_t>(Nrf24Pipe::TX_PIPE)) {
				_uart::send("Tx_Pipe ");
			}
			else {
				_uart::send("Rx_Pipe");
				_uart::send(i - 0x0A);
			}
			_uart::send(" : ");
			for(int j = 0; j < size-1; ++j) {
				_uart::send((int)buffer[j]);
				_uart::send(",");
			}
			_uart::send((int)buffer[size-1]);
			_uart::sendLine();
			++i;
		}

		_uart::send("RX_PW_P0-6 : ");
		uint8_t ret;
		i = static_cast<uint8_t>(Nrf24Pipe::RX_PIPE0);
		while(i < static_cast<uint8_t>(Nrf24Pipe::TX_PIPE )) {
			_uart::send(" ");
			ret = nrf_comm::readRxPw(static_cast<Nrf24Pipe>(i++));
			_uart::send(ret);
		}
		_uart::sendLine();

		//Move this to the nrf comm class?
		_uart::send("EN_AA : ");
		_uart::send(nrf_comm::readEnAA());
		_uart::sendLine();

		_uart::send("EN_RXADDR : ");
		_uart::send(nrf_comm::readEnRxAddr());
		_uart::sendLine();

		_uart::send("Rf Ch : ");
		_uart::send(nrf_comm::readRfCh());
		_uart::sendLine();

		_uart::send("Config : ");
		_uart::send(nrf_comm::readConfig());
		_uart::sendLine();

		_uart::send("dynpd/feat : ");
		_uart::send(nrf_comm::readDynPd());
		_uart::send(" ");
		_uart::send(nrf_comm::readFeature());
		_uart::sendLine();

		_uart::send("RFSetup : ");
		_uart::send(nrf_comm::readRfSetup());
		_uart::sendLine();

		_uart::send("Fifo Status : ");
		_uart::send(nrf_comm::readFifoStatus());
		_uart::sendLine();


	}

private:

	static inline int openPipeHelper(const Nrf24Pipe pipe, uint8_t* const address)
	{
		int ret = nrf_comm::writePipeAddr(pipe,address,mAddressWidth);
		enableRxPipe(pipe);
		return ret;
	}

	static void readLastIrq()
	{
		mLastIRQ = nrf_comm::readStatus() & RF24_IRQ_MASK;
	}

	static void waitForTxFifoEmpty()
	{
		uint32_t currentTime = _sys::millis();
		uint32_t timeout = currentTime + 100;
		bool timeOut = false;
		//IRQ is high

		while(!(nrf_comm::readFifoStatus() & RF24_TX_EMPTY)) {
			if(timeout < currentTime) {
				_ce::clear();
				nrf_comm::flushTx();
				timeOut = true;
				break;
			}
			currentTime = _sys::millis();
		}
		_ce::clear();
		checkAndClearIrq();
		mLastTxPassed = (timeOut == true) ? false : mLastTxPassed;
	}

	static void setAddressWidth()
	{
		nrf_comm::writeAw(mAddressWidth);
	}

	static void turnOnFeatures()
	{
		nrf_comm::writeFeature(RF24_EN_DPL);
		if(nrf_comm::readFeature() == 0) {
			nrf_comm::activate();
		}
	}


	static uint8_t mRxPipeAddress0[5];
	static uint8_t mTxPipeAddress[5];
	static Nrf24AddressWidth mAddressWidth;
	static uint8_t mLastIRQ;
	static bool mReceivedRxIrq;
	static bool mLastTxPassed;


};

template< class _spi, class _sys, class _uart, class _cs, class _ce, class _irq, uint8_t _payloadsize, Nrf24AddressWidth _addrwidth, uint8_t _channel, Nrf24Speed _speed >
uint8_t Nrf24<_spi,_sys, _uart, _cs, _ce,_irq,_payloadsize,_addrwidth,_channel,_speed>::mRxPipeAddress0[5];
template< class _spi, class _sys, class _uart, class _cs, class _ce, class _irq, uint8_t _payloadsize, Nrf24AddressWidth _addrwidth, uint8_t _channel, Nrf24Speed _speed >
uint8_t Nrf24<_spi,_sys, _uart, _cs, _ce,_irq,_payloadsize,_addrwidth,_channel,_speed>::mTxPipeAddress[5];
template< class _spi, class _sys, class _uart, class _cs, class _ce, class _irq, uint8_t _payloadsize, Nrf24AddressWidth _addrwidth, uint8_t _channel, Nrf24Speed _speed >
Nrf24AddressWidth Nrf24<_spi,_sys, _uart, _cs, _ce,_irq,_payloadsize,_addrwidth,_channel,_speed>::mAddressWidth;
template< class _spi, class _sys, class _uart, class _cs, class _ce, class _irq, uint8_t _payloadsize, Nrf24AddressWidth _addrwidth, uint8_t _channel, Nrf24Speed _speed >
uint8_t Nrf24<_spi,_sys, _uart, _cs, _ce,_irq,_payloadsize,_addrwidth,_channel,_speed>::mLastIRQ;
template< class _spi, class _sys, class _uart, class _cs, class _ce, class _irq, uint8_t _payloadsize, Nrf24AddressWidth _addrwidth, uint8_t _channel, Nrf24Speed _speed >
bool Nrf24<_spi,_sys, _uart, _cs, _ce,_irq,_payloadsize,_addrwidth,_channel,_speed>::mReceivedRxIrq;
template< class _spi, class _sys, class _uart, class _cs, class _ce, class _irq, uint8_t _payloadsize, Nrf24AddressWidth _addrwidth, uint8_t _channel, Nrf24Speed _speed >
bool Nrf24<_spi,_sys, _uart, _cs, _ce,_irq,_payloadsize,_addrwidth,_channel,_speed>::mLastTxPassed;



#endif //_NRF24_HPP
