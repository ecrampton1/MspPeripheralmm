#include "msp430/msp_timer.hpp"

namespace McuPeripheral {


template<uint16_t _control, uint16_t _counter, uint16_t _interrupt, uint16_t _ccc0, uint16_t _ccc1, uint16_t _ccc2, uint16_t _ccr0, uint16_t _ccr1, uint16_t _ccr2 >
void* McuPeripheral::TimerControl<_control,_counter,_interrupt,_ccc0,_ccc1, _ccc2,_ccr0,_ccr1,_ccr2>::mTimerArgs = 0;

template<uint16_t _control, uint16_t _counter, uint16_t _interrupt, uint16_t _ccc0,uint16_t _ccc1, uint16_t _ccc2, uint16_t _ccr0, uint16_t _ccr1, uint16_t _ccr2 >
McuPeripheral::callback_t McuPeripheral::TimerControl<_control,_counter,_interrupt,_ccc0,_ccc1, _ccc2,_ccr0,_ccr1,_ccr2>::mTimerHandler = 0;


#ifdef TIMER0_ENABLE_INT
template<>
__attribute__((__interrupt__(TIMER0_A1_VECTOR))) void TimerControl<TACTL_, TAR_, TAIV_, TACCTL0_, TACCTL1_, TACCTL2_, TACCR0_, TACCR1_, TACCR2_>::Timer_A1(void)
{
	switch( TAIV )
	{
		case  2: break;                          // CCR1 not used yet
		case  4: break;                          // CCR2 not used yet
		case 10:           				// overflow
			mTimerHandler(mTimerArgs);
			break;
		default:
			break;
	}

}
#endif

#ifdef TIMER1_ENABLE_INT
template<>
__attribute__((__interrupt__(TIMER1_A1_VECTOR))) void TimerControl<TA1CTL_, TA1R_, TA1IV_, TA1CCTL0_, TA1CCTL1_, TA1CCTL2_,TA1CCR0_, TA1CCR1_, TA1CCR2_>::Timer_A1(void)
{
	switch( TA1IV )
	{
		case  2: break;                          // CCR1 not used yet
		case  4: break;                          // CCR2 not used yet
		case 10:            				// overflow
			mTimerHandler(mTimerArgs);
			break;
		default:
			break;
	}

}
#endif

}

