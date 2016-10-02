#ifndef _MCU_UART_HPP
#define _MCU_UART_HPP
#include <stdint.h>


namespace McuPeripheral
{

enum class BaudRate : uint32_t
{
	BAUD_9600 = 9600,
	BAUD_19200 = 19200,
	BAUD_38400 = 38400,
	BAUD_57200 = 57200,
	BAUD_115200 = 115200
};

}
#endif //_MCU_UART_HPP
