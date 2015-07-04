#ifndef _MSP_UART_HPP
#define _MSP_UART_HPP
#include "mcu_uart.hpp"
#include <stdint.h>
#include <msp430.h>

namespace McuPeripheral
{

//TODO define the registers of the uart used.

template<typename X,typename Y>
constexpr uint16_t divide(X x, Y y) { return static_cast<uint32_t>(x) / static_cast<uint32_t>(y);  }
//constexpr uint8_t mod_calc(uint32_t x, uint16_t y) { return ((x * 16 / (uint16_t) y) - (x / (uint16_t) y * 16) + 1) / 2; }
template<typename X,typename Y>
constexpr uint8_t mod_calc(X x, Y y) { return (((static_cast<uint32_t>(x) / static_cast<uint32_t>(y)) - divide(x ,  y)) * 8) << 1; }

template<BaudRate rate, Speed clock, int rxPin, int txPin >
class McuUart : Uart
{
public:

	static const uint16_t BAUDREGISTER = divide( clock , rate );
	static const uint8_t MODVALUE = mod_calc( clock,  rate );
	//static const uint8_t MODVALUE =
	static void init()
	{
		UCA0CTL1 |= UCSWRST;
		UCA0CTL0 = UCMODE_0;
		UCA0CTL1 = UCSSEL_2 | UCSWRST;
		UCA0BR0 = BAUDREGISTER & 0xFF;
		UCA0BR1 = BAUDREGISTER >> 8;
		UCA0MCTL = MODVALUE;
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
