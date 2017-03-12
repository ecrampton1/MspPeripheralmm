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


#if defined( PORT1_P0_ENABLE_INT) || defined( PORT1_P1_ENABLE_INT) || defined( PORT1_P2_ENABLE_INT) || defined( PORT1_P3_ENABLE_INT) || defined( PORT1_P4_ENABLE_INT) || defined( PORT1_P5_ENABLE_INT) || defined( PORT1_P6_ENABLE_INT) || defined( PORT1_P7_ENABLE_INT)
__attribute__((__interrupt__(PORT1_VECTOR))) void PORT1_Irq(void)
{
	uint8_t flag = P1IFG;
	uint8_t enabled = P1IE;

#if defined( PORT1_P0_ENABLE_INT )
	PIN_INTERRUPT_HANDLER(Port1,BIT0)
#endif
#if defined( PORT1_P1_ENABLE_INT )
	PIN_INTERRUPT_HANDLER(Port1,BIT1)
#endif
#if defined( PORT1_P2_ENABLE_INT )
	PIN_INTERRUPT_HANDLER(Port1,BIT2)
#endif
#if defined( PORT1_P3_ENABLE_INT )
	PIN_INTERRUPT_HANDLER(Port1,BIT3)
#endif
#if defined( PORT1_P4_ENABLE_INT )
	PIN_INTERRUPT_HANDLER(Port1,BIT4)
#endif
#if defined( PORT1_P5_ENABLE_INT )
	PIN_INTERRUPT_HANDLER(Port1,BIT5)
#endif
#if defined( PORT1_P6_ENABLE_INT )
	PIN_INTERRUPT_HANDLER(Port1,BIT6)
#endif
#if defined( PORT1_P7_ENABLE_INT )
	PIN_INTERRUPT_HANDLER(Port1,BIT7)
#endif
	P1IFG &= 0x00;  //TODO is this needed to clear?
}
#endif

#if defined( PORT2_P0_ENABLE_INT) || defined( PORT2_P1_ENABLE_INT) || defined( PORT2_P2_ENABLE_INT) || defined( PORT2_P3_ENABLE_INT) || defined( PORT2_P4_ENABLE_INT) || defined( PORT2_P5_ENABLE_INT) || defined( PORT2_P6_ENABLE_INT) || defined( PORT2_P7_ENABLE_INT)
__attribute__((__interrupt__(PORT2_VECTOR))) void PORT2_Irq(void)
{
	uint8_t flag = P2IFG;
	uint8_t enabled = P2IE;

#if defined( PORT2_P0_ENABLE_INT )
	PIN_INTERRUPT_HANDLER(Port2,BIT0)
#endif
#if defined( PORT2_P1_ENABLE_INT )
	PIN_INTERRUPT_HANDLER(Port2,BIT1)
#endif
#if defined( PORT2_P2_ENABLE_INT )
	PIN_INTERRUPT_HANDLER(Port2,BIT2)
#endif
#if defined( PORT2_P3_ENABLE_INT )
	PIN_INTERRUPT_HANDLER(Port2,BIT3)
#endif
#if defined( PORT2_P4_ENABLE_INT )
	PIN_INTERRUPT_HANDLER(Port2,BIT4)
#endif
#if defined( PORT2_P5_ENABLE_INT )
	PIN_INTERRUPT_HANDLER(Port2,BIT5)
#endif
#if defined( PORT2_P6_ENABLE_INT )
	PIN_INTERRUPT_HANDLER(Port2,BIT6)
#endif
#if defined( PORT2_P7_ENABLE_INT )
	PIN_INTERRUPT_HANDLER(Port2,BIT7)
#endif
	P2IFG &= 0x00;  //TODO is this needed to clear?
}
#endif

#undef PIN_INTERRUPT_HANDLER
}
