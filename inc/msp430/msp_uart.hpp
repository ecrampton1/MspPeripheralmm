#ifndef _MSP_UART_HPP
#define _MSP_UART_HPP
#include "mcu_uart.hpp"
#include <stdint.h>
#include <msp430.h>

namespace McuPeripheral
{

//TODO define the registers of the uart used.


template<BaudRate rate, int rxPin, int txPin >
class McuUart : Uart
{
public:
	static void init()
	{
		uint16_t speed = (SystemBase::getSpeed() /100);
		UCA0CTL1 |= UCSWRST;
		UCA0CTL0 = UCMODE_0;
		UCA0CTL1 = UCSSEL_2 | UCSWRST;
		//UCA0BR0 = ( speed / (uint16_t) rate) & 0xff;
		//UCA0BR1 = (speed / (uint16_t) rate) >> 8;
		//uint8_t mod = (freq * 8 / baud) - (freq / baud * 8);
		// Proper rounding
		//uint8_t mod = ((speed * 16 / (uint16_t) rate) - (speed / (uint16_t) rate * 16) + 1) / 2;
		//UCA0MCTL = mod << 1;
		UCA0BR0 = 138;
		UCA0BR1 = 0;
		UCA0MCTL = 0x0E;
		P1SEL |= BIT1 | BIT2;
		P1SEL2 |= BIT1 | BIT2;
		P1DIR &= ~BIT1;
		P1DIR |= BIT2;
		UCA0CTL1 &= ~UCSWRST;
	}
	template<typename T>
	static const void send(T data)
	{
		uint8_t i = 0;
		uint8_t* ptr = reinterpret_cast<uint8_t*>(&data);
		while( i < sizeof(T)) {
			UCA0TXBUF = *(ptr + i++);  //Sends in Little Endian
			while (!(IFG2 & UCA0TXIFG));
		}
	}

	static const void sendStream(uint8_t* data, int numOfBytes)
	{
		for(int i = 0; i < numOfBytes; ++i) {
			send(data[i]);
		}
	}

	static const void sendStream(const char* data)
	{
		int i = 0;
		while(data[i] != 0) {
			send(data[i++]);
		}
	}


};

}

#endif //_MSP_UART_HPP
