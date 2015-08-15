#ifndef _MSP_TIMER_HPP
#define _MSP_TIMER_HPP
#include <stdint.h>
#include "mcu_timer.hpp"
#include <msp430.h>

namespace McuPeripheral {

enum class TimerSource : uint16_t
{
	SMCLK = TASSEL_2,
	ACLK = TASSEL_1
};


enum class TimerMode : uint16_t
{
	UP_MODE = MC_1,
	CONT_MODE = MC_2,
	UP_DOWN_MODE = MC_3
};

enum class ClockDivider : uint16_t
{
	DIVIDE_1 = ID_0,
	DIVIDE_2 = ID_1,
	DIVIDE_4 = ID_2,
	DIVIDE_8 = ID_3

};

static callback_t callbackTimerA0;

template<uint16_t _control, uint16_t _conunter, uint16_t _interrupt, TimerSource _source >
class TimerControl
{
public:
	static callback_t mTimerHandler;
	static void* mTimerArgs;

	static void enable_irq() { *((volatile uint16_t*)_control) |= TAIE; }
	static void disable_irq() { *((volatile uint16_t*)_control) &= ~TAIE; }
	static void clear_timer() { *((volatile uint16_t*)_control) |= TACLR; }
	static void stop_timer() { *((volatile uint16_t*)_control) &= ~MC_3; }
	static void start_timer( TimerMode mode ) { *((volatile uint16_t*)_control) |= ((uint16_t)_source | (uint16_t)mode | (uint16_t)ClockDivider::DIVIDE_1); }
	static bool is_running() { return ( *((volatile uint16_t*)_control) & MC_3 ) > 0; }



};


template < TimerSource r, uint64_t s, int32_t c >
constexpr uint32_t compute_rollover()
{
	return (r == TimerSource::SMCLK) ? (s / 1000000.0) / (1 / ( c / 1.0 ) * 65535.0) : 0; //TODO ACLK
}


template<TimerSource _source, class _timer, uint64_t _microSeconds, int32_t _clock >
class McuTimer : public TimerBase< McuTimer< _source, _timer, _microSeconds, _clock> >
{
public:

	static int start()
	{
		//check if timer be running
		if(!_timer::is_running()) {
			//_timer::mTimerHandler = &McuTimer< _source, _timer, _microSeconds, _clock>::TimerHandle;
			callbackTimerA0 = &McuTimer< _source, _timer, _microSeconds, _clock>::TimerHandle;
			_DINT();
			_timer::enable_irq();
			_timer::start_timer(TimerMode::CONT_MODE);
			_EINT();
			return 0;
		}

		return -1;
	}
	static void stop() { _timer::stop_timer(); }

	static void TimerHandle(void*)
	{
		++mRollOverCount;
		if(mRollOverCount >= ROLLOVER)
		{
			mRollOverCount = 0;
			TimerBase< McuTimer< _source, _timer, _microSeconds, _clock> >::mTimerCallback( TimerBase< McuTimer< _source, _timer, _microSeconds, _clock> >::mTimerCallbackArgs);
		}
	}

	static uint32_t mRollOverCount;
	static uint16_t mTimerCCR0Count;

private:
	static constexpr uint32_t ROLLOVER = compute_rollover<_source, _microSeconds, _clock>();
	static constexpr uint16_t TIMERCCR0 = 0;

	//static const uint16_t TIMERCCR0 = 0;
	//static const uint16_t TIMERCCR0 = compute_ccr0(_source, _microSeconds, _clock);
};

template<TimerSource _source, class _timer, uint64_t _microSeconds, int32_t _clock >
uint32_t McuTimer< _source, _timer, _microSeconds, _clock>::mRollOverCount;
template<TimerSource _source, class _timer, uint64_t _microSeconds, int32_t _clock >
uint16_t McuTimer< _source, _timer, _microSeconds, _clock>::mTimerCCR0Count;

}

using TimerSrc = McuPeripheral::TimerSource;
using TimerA0 = McuPeripheral::TimerControl<TACTL_, TAR_, TAIV_, McuPeripheral::TimerSource::SMCLK>;

//#include <signal.h>
//__attribute__((__interrupt__(TIMER0_A1_VECTOR)))
#pragma vector=TIMER0_A1_VECTOR
__attribute__((__interrupt__)) void Timer_A1(void)
//static void Timer_A (void)
//interrupt (TIMER0_A1_VECTOR) TA0_isr(void)
{
	//static uint32_t mRollOverCount = 0;
	switch( TAIV )
	{
		case  2: break;                          // CCR1 not used
		case  4: break;                          // CCR2 not used
		case 10: break;           				// overflow
	}
	McuPeripheral::callbackTimerA0(0);

}


#endif //_MSP_TIMER_HPP
