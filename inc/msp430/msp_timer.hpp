#ifndef _MSP_TIMER_HPP
#define _MSP_TIMER_HPP
#include <stdint.h>
#include "mcu_timer.hpp"
#include "msp_periph.hpp"
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

enum class CapCompSelect : uint16_t
{
	CCIA = CCIS_0,
	CCIB = CCIS_1,
	GND  = CCIS_2,
	VCC  = CCIS_3
};

enum class CapMode : uint16_t
{
	RISING_EDGE = CM_1,
	FALLING_EDGE = CM_2,
	DUAL_EDGE = CM_3
};

enum class SyncCapSource : uint16_t
{
	ASYNC = 0,
	SYNC = SCS
};


template< uint16_t _ccc, uint16_t _ccr >
class CaptureCompareControl
{
public:

	/**@brief Enable the capture/compare irq of the specified compare/capture
	 * register.
	*/
	static void enable_cap_comp_irq() { REG_16(_ccc) |= CCIE; }

	/**@brief Enable the capture/compare irq of the specified compare/capture
	 * register.  @ref CapCompRegister for valid registers to use.
	 */
	static void disable_cap_comp_irq() { REG_16(_ccc) &= ~CCIE; }

	/**
	 * @return the value in the compare/capture register
	 */
	static uint16_t get_cap_comp_value() { return REG_16(_ccr); }


	/**@brief Starts the capture based on the parameters passed in
	 * @param [in] ccis selects what is triggering the input for the capture @ref CapCompSelect
	 * @param [in] cm selects the capture mode uses @ref CapMode
	 * @param [in] scs sets if this is asynchronous trigger synchronous triggered based on clock
	 */
	static void start_capture(CapCompSelect ccis,  CapMode cm = CapMode::RISING_EDGE, SyncCapSource scs = SyncCapSource::ASYNC )
	{
		REG_16(_ccc) = static_cast<uint16_t>(scs) | static_cast<uint16_t>(cm)
				| CAP | static_cast<uint16_t>(ccis);
	}

	/**@brief Stop capturing */
	static void stop_capture(){ REG_16(_ccr) &= ~(CM_3 | CAP); }

	/**@brief this will return the state of the capture flag and will clear.
	 * @return the state of the capture input flag if set a capture has occurred */
	static bool read_capture_flag()
	{
		//read the capture input flag and clear it.
		bool ret = CCIFG & REG_16(_ccc);
		if(ret) { REG_16(_ccc) &= ~CCIFG; }
		return  ret;
	}

private:


};


template<uint16_t _control, uint16_t _counter,
	uint16_t _ccc0, uint16_t _ccc1, uint16_t _ccc2, uint16_t _ccr0, uint16_t _ccr1, uint16_t _ccr2>
class TimerControl
{
public:

	//Capture Compare Register Control
	using CapCompControl0 = CaptureCompareControl<_ccc0,_ccr0>;
	using CapCompControl1 = CaptureCompareControl<_ccc1,_ccr1>;
	using CapCompControl2 = CaptureCompareControl<_ccc2,_ccr2>;

	static callback_t mOverFlowCallback;
	static void* mOverFlowArgs;

	/**@brief Enable the overflow irq of the timer
	 */
	static void intEnable() { REG_16(_control) |= TAIE; }

	/**@brief Disable the overflow irq of the timer
	 */
	static void intDisable() { REG_16(_control) &= ~TAIE; }

	static bool isIntEnable() { return REG_16(_control) & TAIE; }

	static void clearTimer() { REG_16(_control) |= TACLR; }

	static void stopTimer() { REG_16(_control) &= ~MC_3; }

	static uint16_t getCounterValue() { return REG_16(_counter); }

	static void startTimer( TimerMode mode, TimerSource source, ClockDivider divide=ClockDivider::DIVIDE_1 ) { *((volatile uint16_t*)_control) |= ((uint16_t)source | (uint16_t)mode | (uint16_t)divide); }

	static bool isRunning() { return ( *((volatile uint16_t*)_control) & MC_3 ) > 0; }

};


template < TimerSource r, uint64_t s, int32_t c >
constexpr uint32_t compute_rollover()
{
	return (r == TimerSource::SMCLK) ? (s / 1000000.0F) / (1 / ( c / 1.0F ) * 65535.0F) : (s / 1000000.0F) / (1 / ( 32768.0F / 1.0F ) * 65535.0F); //TODO ACLK
}


//MCLK source
template<TimerSource _source, uint32_t _clockspeed, ClockDivider _divider=ClockDivider::DIVIDE_1>
class TimerConfig
{
public:

	static constexpr float computeTickTiming()
	{
		return 1.0F / _clockspeed;
	}

	static constexpr float computeOverFlowTiming()
	{
		return computeTickTiming()*0xFFFF;
	}

	static constexpr TimerSource Source = _source;
	static constexpr uint32_t ClockSpeed = _clockspeed;
	static constexpr ClockDivider Divider = _divider;


};

/*class InputCapture
{
public:

};*/


template< class _timer, class _timerconfig >
class ContinuousTimer
{
public:

	static void intEnable()
	{
		_DINT();
		_timer::intEnable();
		_EINT();
	}

	static void start()
	{
		_timer::clearTimer();
		_timer::startTimer(TimerMode::CONT_MODE,_timerconfig::Source,_timerconfig::Divider);
	}

	static void setCallback(callback_t callback, void* args)
	{
		_timer::mOverFlowCallback = callback;
		_timer::mOverFlowArgs = args;
	}

	static uint16_t getTimerCount(){ return _timer::getCounterValue(); }

	static void stop() { _timer::stopTimer(); }

private:

	ContinuousTimer() =delete;
	ContinuousTimer(const ContinuousTimer&) =delete;
	ContinuousTimer& operator=(const ContinuousTimer&) =delete;
};


template<uint16_t _control, uint16_t _counter, uint16_t _ccc0, uint16_t _ccc1, uint16_t _ccc2, uint16_t _ccr0, uint16_t _ccr1, uint16_t _ccr2 >
void* McuPeripheral::TimerControl<_control,_counter,_ccc0,_ccc1, _ccc2,_ccr0,_ccr1,_ccr2>::mOverFlowArgs;

template<uint16_t _control, uint16_t _counter, uint16_t _ccc0,uint16_t _ccc1, uint16_t _ccc2, uint16_t _ccr0, uint16_t _ccr1, uint16_t _ccr2 >
McuPeripheral::callback_t McuPeripheral::TimerControl<_control,_counter,_ccc0,_ccc1, _ccc2,_ccr0,_ccr1,_ccr2>::mOverFlowCallback;


}

using Timer_Source = McuPeripheral::TimerSource;
using Timer_Mode = McuPeripheral::TimerMode;
using Timer0 = McuPeripheral::TimerControl<TACTL_, TAR_, TACCTL0_, TACCTL1_, TACCTL2_, TACCR0_, TACCR1_, TACCR2_>;
using Timer1 = McuPeripheral::TimerControl<TA1CTL_, TA1R_, TA1CCTL0_, TA1CCTL1_, TA1CCTL2_, TA1CCR0_, TA1CCR1_, TA1CCR2_>;

#endif //_MSP_TIMER_HPP
