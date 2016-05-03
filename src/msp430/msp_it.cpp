#include "msp430/msp_uart.hpp"
#include "msp430/msp_spi.hpp"
#include "msp430/msp_periph.hpp"
#include "msp430/msp_gpio.hpp"

namespace McuPeripheral {


#if defined( UARTA0_ENABLE_INT ) || defined( UARTB0_ENABLE_INT )
__attribute__((__interrupt__(USCIAB0TX_VECTOR))) void USCIAB0_Tx(void)
{
	uint8_t data;
	bool popped = UartA0_Irq::mTxBuffer.pop(data);
	if(popped) {
		UCA0TXBUF = data;
	}
	else {
		IE2 &= ~UCA0TXIE;
	}
}


__attribute__((__interrupt__(USCIAB0RX_VECTOR))) void USCIAB0_Rx(void)
{
	uint8_t data = UCA0RXBUF;
	if(UartA0_Irq::mRxBuffer.full()) {
		uint8_t throw_away;
		UartA0_Irq::mRxBuffer.pop(throw_away);
	}
	UartA0_Irq::mRxBuffer.push(data);
}
#endif

#define PIN_INTERRUPT_HANDLER(PORT,PIN) \
	if((flag & PIN) && (enabled & PIN)) \
	{ \
		McuPin<Mcu##PORT,PIN>::mPinHandler(McuPin<Mcu##PORT,PIN>::mPinArgs); \
	} \


#if defined( PORT1_ENABLE_INT)
__attribute__((__interrupt__(PORT1_VECTOR))) void PORT1_Irq(void)
{
	uint8_t flag = P1IFG;
	uint8_t enabled = P1IE;


	PIN_INTERRUPT_HANDLER(Port1,BIT0)
	PIN_INTERRUPT_HANDLER(Port1,BIT1)
	PIN_INTERRUPT_HANDLER(Port1,BIT2)
	PIN_INTERRUPT_HANDLER(Port1,BIT3)
	PIN_INTERRUPT_HANDLER(Port1,BIT4)
	PIN_INTERRUPT_HANDLER(Port1,BIT5)
	PIN_INTERRUPT_HANDLER(Port1,BIT6)
	PIN_INTERRUPT_HANDLER(Port1,BIT7)

}
#endif

#undef PIN_INTERRUPT_HANDLER
}