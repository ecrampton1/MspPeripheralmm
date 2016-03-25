#include "msp430/msp_uart.hpp"
#include "msp430/msp_spi.hpp"
#include "msp430/msp_periph.hpp"

namespace McuPeripheral {


#if defined( UARTA0_ENABLE_INT ) || defined( UARTB0_ENABLE_INT )
__attribute__((__interrupt__(USCIAB0TX_VECTOR))) void USCIAB0_Tx(void)
{
#if 1
	uint8_t data;
	bool popped = UartA0_Irq::mTxBuffer.pop(data);
	if(popped) {
		UCA0TXBUF = data;
	}
	else {
		IE2 &= ~UCA0TXIE;
	}
#endif
}


__attribute__((__interrupt__(USCIAB0RX_VECTOR))) void USCIAB0_Rx(void)
{
#if 1
	uint8_t data = UCA0RXBUF;
	if(UartA0_Irq::mRxBuffer.full()) {
		uint8_t throw_away;
		UartA0_Irq::mRxBuffer.pop(throw_away);
	}
	UartA0_Irq::mRxBuffer.push(data);
#endif
}
#endif

}
