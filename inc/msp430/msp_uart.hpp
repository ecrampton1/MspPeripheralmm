#ifndef _MSP_UART_HPP
#define _MSP_UART_HPP
#include "mcu_uart.hpp"
#include "utilities/utilities.hpp"
#include <stdint.h>
#include <msp430.h>
//#include <stdlib.h>
#include <math.h>

namespace McuPeripheral
{

//TODO define the registers of the uart used.

template<typename X,typename Y>
constexpr uint8_t divide(X x, Y y) { return static_cast<uint32_t>(x) / static_cast<uint32_t>(y);  }
//constexpr uint8_t mod_calc(uint32_t x, uint16_t y) { return ((x * 16 / (uint16_t) y) - (x / (uint16_t) y * 16) + 1) / 2; }
template<typename X,typename Y>
constexpr uint8_t mod_calc(X x, Y y) { return (((static_cast<uint32_t>(x) / static_cast<uint32_t>(y)) - divide(x ,  y)) * 8) << 1; }

template<BaudRate rate, Speed clock, int rxPin, int txPin >
class McuUart : Uart
{
public:

	static const uint8_t BAUDREGISTER = divide( clock , rate );
	static const uint8_t MODVALUE = mod_calc( clock,  rate );
	static void init()
	{
		UCA0CTL1 = UCSWRST;
		UCA0CTL0 = UCMODE_0;
		UCA0BR0 = BAUDREGISTER & 0xFF;
		UCA0BR1 = BAUDREGISTER >> 8;
		UCA0MCTL = MODVALUE;
		P1SEL |= rxPin | txPin;
		P1SEL2 |= rxPin | txPin;
		UCA0CTL1 = UCSSEL_2;
	}

	/*static void init()
	{
		BCSCTL1 |= DIVA_0;				// ACLK/8
		BCSCTL3 |= XCAP_3;

		// Configure UART 0
		//UCA0CTL1 |= UCSWRST;
		UCA0CTL1 = UCSSEL_1;                // Set ACLK = 32768 as UCBRCLK
		UCA0BR0 = 3;                                 // 9600 baud
		UCA0BR1 = 0;
		UCA0MCTL = UCBRS1 + UCBRS0;               // Modulation UCBRSx = 3
		//UCA0MCTL = 0x3;                         // 32768/9600 - INT(32768/9600)=0.41
		                                                                    // UCBRSx value = 0x53 (See UG)

		P1SEL |= rxPin | txPin;
		P1SEL2 |= rxPin | txPin;
		UCA0CTL1 &= ~UCSWRST;                    // release from reset

	}*/

	template<class T>
	static const void send( T data, Base base=Base::BASE_HEX )
	{
		static char buf[sizeof(T)*8 +1];
		itoa((uint64_t)data,buf, base);
		send(static_cast<const char*>(buf));
	}

	static const void send( float data, int precision=2)
	{
		//TODO implement (look into fixed point) ??

	}

	static const void send(uint8_t* data, int numOfBytes)
	{
		for(int i = 0; i < numOfBytes; ++i) {
			sendByte(data[i]);
		}
	}

	static const void send(const char* data)
	{
		int i = 0;
		while(data[i] != 0) {
			sendByte(data[i++]);
		}
	}

	static const void sendByte(uint8_t data)
	{
		UCA0TXBUF = data;  //Sends in Little Endian
		while (!(IFG2 & UCA0TXIFG));
	}

	static const void sendLine(const char* data=0)
	{
		if(data != 0)
			send(data);
		send("\n");
	}

private:


};

}

#endif //_MSP_UART_HPP
