#ifndef _MCU_UART_HPP
#define _MCU_UART_HPP
#include <stdint.h>


namespace McuPeripheral
{

enum class BaudRate
{
	BAUD_9600 = 96,
	BAUD_19200 = 192,
	BAUD_38400 = 384,
	BAUD_57200 = 572,
	BAUD_115200 = 1152
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
