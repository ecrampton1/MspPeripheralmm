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


class Uart
{
public:
	static const void init();
	template<typename T>
	static const void send(T data);
	//TODO make specialized function
	//static const void send(float data);

	static const void sendStream(uint8_t* data, int numOfBytes);

};

};
#endif //_MCU_UART_HPP
