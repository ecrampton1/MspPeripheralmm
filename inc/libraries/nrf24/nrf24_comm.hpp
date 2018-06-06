#ifndef _NRF24_COMM_HPP
#define _NRF24_COMM_HPP

#include "nrf24/nRF24L01.h"

enum class Nrf24Pipe : uint8_t
{
	TX_PIPE = RF24_TX_ADDR,
	RX_PIPE0 = RF24_RX_ADDR_P0,
	RX_PIPE1 = RF24_RX_ADDR_P1,
	RX_PIPE2 = RF24_RX_ADDR_P2,
	RX_PIPE3 = RF24_RX_ADDR_P3,
	RX_PIPE4 = RF24_RX_ADDR_P4,
	RX_PIPE5 = RF24_RX_ADDR_P5
};

enum class Nrf24AddressWidth : uint8_t
{
	WIDTH_3 = 3,
	WIDTH_4 = 4,
	WIDTH_5 = 5
};

enum class Nrf24Crc : uint8_t
{
	CRC_NONE = 0,
	CRC_8 = RF24_EN_CRC,
	CRC_16 = RF24_EN_CRC | RF24_CRCO
};

enum class Nrf24Speed : uint8_t
{
	RF_SPEED_250KBPS = RF24_RF_DR_LOW,
	RF_SPEED_1MBPS = 0,
	RF_SPEED_2MBPS = RF24_RF_DR_HIGH,
	RF_SPEED_RESET = RF24_RF_DR_LOW | RF24_RF_DR_HIGH
};


template< class _spi, class _cs >
class Nrf24Comm
{
public:

	static void init()
	{
		_cs::output();
		_cs::set();
		_spi::init();
		_spi::send((uint8_t)0x00);

	}

	static uint8_t readConfig()
	{
		return readRegister(RF24_CONFIG);
	}

	static uint8_t readEnAA()
	{
		return readRegister(RF24_EN_AA);
	}

	static uint8_t readAW()
	{
		return readRegister(RF24_SETUP_AW);
	}

	static uint8_t readRfSetup()
	{
		return readRegister(RF24_RF_SETUP);
	}

	static uint8_t readFeature()
	{
		return readRegister(RF24_FEATURE);
	}

	static uint8_t readEnRxAddr()
	{
		return readRegister(RF24_EN_RXADDR);
	}

	static uint8_t readFifoStatus()
	{
		return readRegister(RF24_FIFO_STATUS);
	}

	static uint8_t readDynPd()
	{
		return readRegister(RF24_DYNPD);
	}

	static uint8_t readRfCh()
	{
		return readRegister(RF24_RF_CH);
	}

	static uint8_t readStatus()
	{
		mRfStatus = readRegister(RF24_STATUS);
		return mRfStatus;
	}

	static uint8_t getLastStatus()
	{
		return mRfStatus;
	}

	static bool checkRegisterFlags(uint8_t address, uint8_t flags)
	{
		return readRegister(address) & flags;
	}

	static void flushTx()
	{
		writeRegister(0,0,0,RF24_FLUSH_TX);
	}

	static void flushRx()
	{
		writeRegister(0,0,0,RF24_FLUSH_RX);
	}

	static void reuseTxPL()
	{
		writeRegister(0,0,0,RF24_REUSE_TX_PL);
	}

	static void activate()
	{
		uint8_t activate = 0x73;
		writeRegister(0,&activate,1,RF24_ACTIVATE);
	}

	static uint8_t readRxPw(Nrf24Pipe pipe)
	{
		if(pipe >= Nrf24Pipe::TX_PIPE || pipe < Nrf24Pipe::RX_PIPE0) {
			return 0; //invalid
		}
		uint8_t ret = readRegister(static_cast<uint8_t>(pipe) + 7); //pipe address plus 7 gives us the rx pw address
		return ret;
	}

	static uint8_t readTopFifoRxWidth()
	{
		return readRegister(0,RF24_R_RX_PL_WID);
	}

	static int readPayload(uint8_t *buf, int size)
	{
		return readRegister(0, buf,size,RF24_R_RX_PAYLOAD);
	}

	static int readPipeAddr( Nrf24Pipe pipe, uint8_t* ret_address, Nrf24AddressWidth width)
	{
		uint8_t w = static_cast<uint8_t>(width);
		uint8_t i = 0;
		if(static_cast<uint8_t>(pipe) > static_cast<uint8_t>(Nrf24Pipe::RX_PIPE1) && static_cast<uint8_t>(Nrf24Pipe::TX_PIPE) > static_cast<uint8_t>(pipe))
		{
			w = 1;
		}
		readRegister(static_cast<uint8_t>(pipe),&ret_address[i],w);
		return w;
	}

	static uint8_t readRegister(uint8_t address, uint8_t command = RF24_R_REGISTER)
	{
		uint8_t ret;
		readRegister(address, &ret,sizeof(ret),command);
		return ret;

	}

	//return number of byes read
	static int readRegister(uint8_t address, uint8_t* ret_buf, int size, uint8_t command = RF24_R_REGISTER )
	{
		//possible improvement to create a static buffer to send or  make spi send
		//a byte over and over
		_cs::clear();
		mRfStatus = _spi::exchange( (uint8_t)(command | address) );
		int i =0;
		while(i < size) {
			ret_buf[i++] = _spi::exchange((uint8_t)RF24_NOP);
		}
		_cs::set();

		return i;
	}


	static void writeFeature(uint8_t data)
	{
		writeRegister(RF24_FEATURE,data);
	}

	static void writeStatus(uint8_t data)
	{
		writeRegister(RF24_STATUS,data);
	}

	static void writeRfSetup(uint8_t data)
	{
		writeRegister(RF24_RF_SETUP,data);
	}

	static void writeConfig(uint8_t data)
	{
		writeRegister(RF24_CONFIG,data);
	}

	static void writeEnAA(uint8_t data)
	{
		writeRegister(RF24_EN_AA,data);
	}

	static void writeAw(Nrf24AddressWidth width)
	{
		writeRegister(RF24_SETUP_AW,static_cast<uint8_t>(width)-2);
	}

	static void writeEnRxAddr(uint8_t data)
	{
		writeRegister(RF24_EN_RXADDR, data);
	}

	static void writeFifoStatus(uint8_t data)
	{
		writeRegister(RF24_FIFO_STATUS,data);
	}

	static void writeDynPd(uint8_t data)
	{
		writeRegister(RF24_DYNPD,data);
	}

	static void writeSetupRetr(uint8_t data)
	{
		writeRegister(RF24_SETUP_RETR,data);
	}

	static void writeRfCh(uint8_t data)
	{
		if(data > 125)
			data = 125;
		writeRegister(RF24_RF_CH,data);
	}

	static void writeRxPw(Nrf24Pipe pipe, uint8_t size)
	{
		if(pipe >= Nrf24Pipe::TX_PIPE || pipe < Nrf24Pipe::RX_PIPE0) {
			return; //invalid
		}
		if(size > 0x20) {
			size = 0x20;
		}
		writeRegister(static_cast<uint8_t>(pipe) + 7, size); //pipe address plus 7 gives us the rx pw address

	}

	static int writePipeAddr( Nrf24Pipe pipe, uint8_t* address,Nrf24AddressWidth width)
	{
		uint8_t w = static_cast<uint8_t>(width);
		uint8_t i = 0;
		/*if(Nrf24Pipe::TX_PIPE == pipe) {
			writePipeAddr( Nrf24Pipe::RX_PIPE0, address, width);
		}*/
		if(static_cast<uint8_t>(pipe) > static_cast<uint8_t>(Nrf24Pipe::RX_PIPE1) && static_cast<uint8_t>(Nrf24Pipe::TX_PIPE) > static_cast<uint8_t>(pipe))
		{
			w = 1;
		}
		return writeRegister(static_cast<uint8_t>(pipe),address,static_cast<uint8_t>(w));

	}

	static int writePayload(uint8_t *buf, int size)
	{
		return writeRegister(0, buf,size,RF24_W_TX_PAYLOAD);
	}

	static int writeAckPayload(uint8_t *buf, int size)
	{
		return writeRegister(0, buf,size,RF24_W_ACK_PAYLOAD);
	}

	static int writeNoTxAck(uint8_t *buf, int size)
	{
		return writeRegister(0, buf,size,RF24_W_TX_PAYLOAD_NOACK);
	}

	static void writeRegister(uint8_t address, uint8_t data)
	{
		_cs::clear();
		writeRegister(address,&data,sizeof(data));
		_cs::set();
	}

	//return number fo byes read
	static int writeRegister(uint8_t address,uint8_t* buf, int size, uint8_t command = RF24_W_REGISTER)
	{
		//possible improvement to create a static buffer to send or  make spi send
		//a byte over and over
		_cs::clear();
		mRfStatus = _spi::exchange( (uint8_t)(command | address) );
		 int i = _spi::send(buf,size);
		_cs::set();

		return i;
	}

private:

	static uint8_t mRfStatus;

};

template< class _spi, class _cs >
uint8_t Nrf24Comm<_spi, _cs >::mRfStatus;


#endif //_NRF24_COMM_HPP
