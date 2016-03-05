#include "msp430/msp_uart.hpp"

namespace McuPeripheral {



template<>
__attribute__((__interrupt__(USCIAB0TX_VECTOR))) void UartControl<rx, tx, UCA0CTL0_, UCA0CTL1_, UCA0BR0_, UCA0BR1_, UCA0MCTL_, UCA0RXBUF_, UCA0TXBUF_, UCA0STAT_, IE2_, IFG2_>::Uart_Tx(void)
{
	uint8_t data = 0xAB;
	bool popped = mTxBuffer.pop(data);
	if(popped) {
		UCA0TXBUF = data;
	}
	else {
		IE2 &= ~UCA0TXIE;
	}

}



template<>
__attribute__((__interrupt__(USCIAB0RX_VECTOR))) void UartControl<rx, tx, UCA0CTL0_, UCA0CTL1_, UCA0BR0_, UCA0BR1_, UCA0MCTL_, UCA0RXBUF_, UCA0TXBUF_, UCA0STAT_, IE2_, IFG2_>::Uart_Rx(void)
{
	uint8_t data = UCA0RXBUF;
	if(mRxBuffer.full()) {
		uint8_t throw_away;
		mRxBuffer.pop(throw_away);
	}
	mRxBuffer.push(data);
}


}
