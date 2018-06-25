#ifndef _RFM69_HPP
#define _RFM69_HPP
#include "rfm69/rfm69_comm.hpp"
#include <string.h>


//TODO in general better timeout handling and fault handling in this class start with while loop timeouts.

template< class _spi, class _sys,  class _cs, class _irq, CarrierFrequency _freq, uint8_t _node, uint8_t _network, class _uart>
class Rfm69
{

	using rfm69_comm = Rfm69Comm< _spi, _cs, _freq, _network, _uart >;

public:
	static void init()
	{
		rfm69_comm::init();
		setNodeAddress(_node);
		waitForModeReady();
		configInterrupt();
		mPayloadReady = false;
	}


	static void putToSleep()
	{
		enableSleep();
	}

	static void printAllRegisters()
	{
		rfm69_comm::printAllRegisters();
	}

	static void setNodeAddress(uint8_t address)
	{
		rfm69_comm::writeRegisterNodeAddress(address);
		mPacketHeader.Source = address;
	}

	static uint8_t getNodeAddress()
	{
		mPacketHeader.Source = rfm69_comm::readRegisterNodeAddress();
		return mPacketHeader.Source;
	}

	static bool isPayloadReady()
	{
		return mPayloadReady;
	}

	static int readPayload(uint8_t* buf, const int size)
	{
		//disable interrupts?
		mPayloadReady = false;
		enableStandby();
		uint8_t length = rfm69_comm::readFifo(buf,size);
		if(3 > length) {
			forceRestartRx();
		}

		return length;

	}

	//TODO add timeout and noAck
	static bool writePayload(uint8_t* const buf, const int size, uint8_t desitnation_node, bool noAck=false)
	{
		buildPacketHeader(size,desitnation_node,noAck);
		forceRestartRx();
		waitForReadyToSend();

		int ret = rfm69_comm::writePacket(mPacketHeader,buf);
		_uart::send(ret);
		if(ret == (size + sizeof(PacketHeader))) {
			enableTx();
			ret = waitPacketSent();
		}
		return ret;

	}

	//.5dB steps where RSSI = RssiValue/2 dBm
	static int readRssi(bool forceReading=false)
	{
		int reading = 0;
		if(false == forceReading) {
			rfm69_comm::writeRegisterRssiConfig(RF_RSSI_START);
			while(false == isRssiReady());
		}
		reading = rfm69_comm::readRegisterRssiValue();
		reading = -reading>>1;
		return reading;
	}

	static void setEncryptionKey(const char* key)
	{
		enableStandby();
		memcpy(mEncryptKey,key,sizeof(mEncryptKey));
		rfm69_comm::writeRegisterAesKey(reinterpret_cast<unsigned char *>(mEncryptKey),sizeof(mEncryptKey));
		enableEncryption();
	}

	static void enableEncryption()
	{
		rfm69_comm::writeRegisterPacketConfig2(rfm69_comm::readRegisterPacketConfig2() | RF_PACKET2_AES_ON );
	}

	static void disableEncryption()
	{
		rfm69_comm::writeRegisterPacketConfig2(rfm69_comm::readRegisterPacketConfig2() & ~RF_PACKET2_AES_ON );
	}

	static void enableRx()
	{
		forceRestartRx();
		rfm69_comm::writeRegisterOpMode((rfm69_comm::readRegisterOpMode() & 0xE3) | RF_OPMODE_RECEIVER);
		enablePayloadReadyIrq();
		waitForModeReady();
	}

	static void enableSleep()
	{
		rfm69_comm::writeRegisterOpMode((rfm69_comm::readRegisterOpMode() & 0xE3) | RF_OPMODE_SLEEP);
	}

	static bool isRxEnabled()
	{
		return (rfm69_comm::readRegisterOpMode() & RF_OPMODE_RECEIVER);
	}

	static uint8_t getIrqFlags1()
	{
		return rfm69_comm::readRegisterIrqFlags1();
	}

	static uint8_t getIrqFlags2()
	{
		return rfm69_comm::readRegisterIrqFlags2();
	}

	//TODO Listen Mode?
	//TODO add setnetwork id function?

private:

	static void buildPacketHeader(const int size, uint8_t desitnation_node, bool noAck)
	{
		mPacketHeader.Destination = desitnation_node;
		mPacketHeader.Length = size + sizeof(mPacketHeader);
		if(false == noAck) {
			mPacketHeader.Control = static_cast<uint8_t>(Rfm69Control::REQUESTACK);
		}
		else {
			mPacketHeader.Control = static_cast<uint8_t>(Rfm69Control::NOACK);
		}
	}

	static void configInterrupt()
	{
		_irq::input();
		_irq::setPinIrqHandler(&gpioIrqTriggered,0);
		_irq::edgeLowToHigh();
		_irq::intEnable();
		enablePayloadReadyIrq();
	}

	static void gpioIrqTriggered(void* args)
	{
		if(isRxEnabled() && isIrqPayloadReady())
		{
			mPayloadReady = true;
			enableStandby();
		}
	}

	static void forceRestartRx()
	{
		rfm69_comm::writeRegisterPacketConfig2(rfm69_comm::readRegisterPacketConfig2() | RF_PACKET2_RXRESTART);
	}

	static void enablePayloadReadyIrq()
	{
		rfm69_comm::writeRegisterDioMapping1(RF_DIOMAPPING1_DIO0_01);//DIO0 triggers on payload ready in rx mode
	}

	static void enableTx()
	{
		rfm69_comm::writeRegisterOpMode((rfm69_comm::readRegisterOpMode() & 0xE3) | RF_OPMODE_TRANSMITTER);
	}

	static void enableStandby()
	{
		rfm69_comm::writeRegisterOpMode((rfm69_comm::readRegisterOpMode() & 0xE3) | RF_OPMODE_STANDBY);
	}

	static bool waitForModeReady()
	{
		static const uint32_t timeOut = 500;
		uint32_t currentTime = _sys::millis();
		while( false == isReady() ){
			if(currentTime + timeOut < _sys::millis()) {
				return false; //Something not quite right.
			}
		}
		return true;
	}

	static bool waitPacketSent()
	{
		int i = 0;

		while(0 == (rfm69_comm::readRegisterIrqFlags2() & RF_IRQFLAGS2_PACKETSENT)) {
			_sys::delayInUs(1000);
			if(++i > 500) {
				return false;
			}
		}
		return true;
	}

	static bool isReady()
	{
		return (rfm69_comm::readRegisterIrqFlags1() & RF_IRQFLAGS1_MODEREADY);
	}

	static bool isIrqPayloadReady()
	{
		return (rfm69_comm::readRegisterIrqFlags2() & RF_IRQFLAGS2_PAYLOADREADY);
	}

	static bool isRssiReady()
	{
		return (rfm69_comm::readRegisterRssiConfig() & RF_RSSI_DONE);
	}

	static bool waitForReadyToSend()
	{
		static const uint32_t timeOut = 500;
		uint32_t currentTime = _sys::millis();

		enableRx();
		while(false == checkRxRssiLimit()) {
			if(currentTime + timeOut < _sys::millis()) {
				return false; //Something not quite right.
			}
		}

		enableStandby();
		if(false == waitForModeReady()) {
			return false; //Something not quite right.
		}

		return true;
	}

	static bool checkRxRssiLimit()
	{
		bool ret = false;
		static constexpr int channelRssiLimit = -100;
		if(isRxEnabled() && readRssi(true) < channelRssiLimit) {
			ret = true;
		}
		return ret;
	}



	static PacketHeader mPacketHeader;
	static char mEncryptKey[16];
	static bool mPayloadReady;
};

template< class _spi, class _sys,  class _cs, class _irq, CarrierFrequency _freq, uint8_t _node, uint8_t _network, class _uart>
PacketHeader Rfm69<_spi, _sys, _cs, _irq, _freq, _node,_network, _uart>::mPacketHeader;
template< class _spi, class _sys,  class _cs, class _irq, CarrierFrequency _freq, uint8_t _node, uint8_t _network, class _uart>
bool Rfm69<_spi, _sys, _cs, _irq, _freq, _node,_network, _uart>::mPayloadReady = false;
template< class _spi, class _sys,  class _cs, class _irq, CarrierFrequency _freq, uint8_t _node, uint8_t _network, class _uart>
char Rfm69<_spi, _sys, _cs, _irq, _freq, _node,_network, _uart>::mEncryptKey[16] = { 0 };

#endif
