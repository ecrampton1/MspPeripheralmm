#include "msp430/msp_uart.hpp"
#include "msp430/msp_spi.hpp"
#include "msp430/msp_periph.hpp"
#include "msp430/msp_gpio.hpp"
#include "msp430/msp_timer.hpp"

namespace McuPeripheral {

__attribute__((interrupt(WDT_VECTOR)))
void watchDog (void)
{
	++mWatchDogCounter;
}

#if 0
__attribute__((interrupt(TRAPINT_VECTOR))) void TRAPINT_Irq(void)
{
    __no_operation();
    while(1);
}


__attribute__((interrupt(NMI_VECTOR))) void NMI_Irq(void)
{
	__no_operation();
	while(1);
}
#endif

#if defined( UARTA0_ENABLE_INT ) || defined( UARTB0_ENABLE_INT )
__attribute__((interrupt(USCIAB0TX_VECTOR))) void USCIAB0_Tx(void)
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


__attribute__((interrupt(USCIAB0RX_VECTOR))) void USCIAB0_Rx(void)
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
__attribute__((interrupt(PORT1_VECTOR))) void PORT1_Irq(void)
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
__attribute__((interrupt(PORT2_VECTOR))) void PORT2_Irq(void)
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


#ifdef TIMER0_CCR0_ENABLE_INT
__attribute__((interrupt(TIMER0_A0_VECTOR))) void Timer0_A0(void)
{
	TimerControl<TACTL_, TAR_,  TACCTL0_, TACCTL1_, TACCTL2_, TACCR0_, TACCR1_, TACCR2_>::mCcr0Callback(0);
}
#endif

#ifdef TIMER0_ENABLE_INT
__attribute__((interrupt(TIMER0_A1_VECTOR))) void Timer0_A1(void)
{
	switch( TAIV )
	{
		case  2: break;                          // CCR1 not used yet
		case  4: break;                          // CCR2 not used yet
		case 10:           				// overflow
			TimerControl<TACTL_, TAR_,  TACCTL0_, TACCTL1_, TACCTL2_, TACCR0_, TACCR1_, TACCR2_>::mOverFlowCallback(0);
			break;
		default:
			break;
	}

}
#endif

#ifdef TIMER1_CCR0_ENABLE_INT
__attribute__((interrupt(TIMER1_A0_VECTOR))) void Timer1_A0(void)
{

}
#endif

#ifdef TIMER1_ENABLE_INT
__attribute__((interrupt(TIMER1_A1_VECTOR))) void Timer1_A1(void)
{
	switch( TA1IV )
	{
		case  2: break;                          // CCR1 not used yet
		case  4: break;                          // CCR2 not used yet
		case 10:            				// overflow
			TimerControl<TA1CTL_, TA1R_, TA1CCTL0_, TA1CCTL1_, TA1CCTL2_,TA1CCR0_, TA1CCR1_, TA1CCR2_>::mOverFlowCallback(0);
			break;
		default:
			break;
	}

}
#endif
}


