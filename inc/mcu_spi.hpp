#ifndef _MCU_SPI_HPP
#define _MCU_SPI_HPP
#include <stdint.h>


namespace McuPeripheral
{

enum class SpiClock : uint8_t
{
	SPI_EDGE1_LOW = 0x00,
	SPI_EDGE2_LOW = UCCKPH,
	SPI_EDGE1_HIGH = UCCKPL,
	SPI_EDGE2_HIGH = UCCKPH | UCCKPL
};

enum class SpiMaster : uint8_t
{
	SPI_SLAVE = 0x00,
	SPI_MASTER = UCMST
};

enum class SpiBitOrder : uint8_t
{
	SPI_LSB = 0x00,
	SPI_MSB = UCMSB
};

}
#endif //_MCU_UART_HPP
