#ifndef _MSP_TIMER_HPP
#define _MSP_TIMER_HPP
#include <stdint.h>
#include "mcu_timer.hpp"
#include "msp_periph.hpp"
#include "msp430/msp_gpio.hpp"//TODO debug
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
	static void intEnable() { REG_16(_ccc) |= CCIE; }

	/**@brief Enable the capture/compare irq of the specified compare/capture
	 * register.  @ref CapCompRegister for valid registers to use.
	 */
	static void intDisable() { REG_16(_ccc) &= ~CCIE; }

	/**
	 * @return the value in the compare/capture register
	 */
	static uint16_t getCapCompValue() { return REG_16(_ccr); }

	static bool getCciValue() { return REG_16(_ccc) & CCI; }

	static bool getCovValue() { return REG_16(_ccc) & COV; }

	static bool clearCov() { return REG_16(_ccc) &= ~COV; }


	/**@brief Starts the capture based on the parameters passed in
	 * @param [in] ccis selects what is triggering the input for the capture @ref CapCompSelect
	 * @param [in] cm selects the capture mode uses @ref CapMode
	 * @param [in] scs sets if this is asynchronous trigger synchronous triggered based on clock
	 */
	static void startCapture(CapCompSelect ccis,  CapMode cm = CapMode::RISING_EDGE, SyncCapSource scs = SyncCapSource::ASYNC )
	{
		REG_16(_ccc) |= static_cast<uint16_t>(scs) | static_cast<uint16_t>(cm)
				| CAP | static_cast<uint16_t>(ccis);
	}

	/**@brief Stop capturing */
	static void stopCapture(){ REG_16(_ccr) &= ~(CM_3 | CAP); }

	/**@brief this will return the state of the capture flag and will clear.
	 * @return the state of the capture input flag if set a capture has occurred */
	static bool readCaptureFlag()
	{
		//read the capture input flag and clear it.
		bool ret = CCIFG & REG_16(_ccc);
		if(ret) { REG_16(_ccc) &= ~CCIFG; }
		return  ret;
	}

	static callback_t mCallback;

private:


};


template<uint16_t _control, uint16_t _counter, uint16_t _taiv,
	uint16_t _ccc0, uint16_t _ccc1, uint16_t _ccc2, uint16_t _ccr0, uint16_t _ccr1, uint16_t _ccr2>
class TimerControl
{
public:

	//Capture Compare Register Control
	using CapCompControl0 = CaptureCompareControl<_ccc0,_ccr0>;
	using CapCompControl1 = CaptureCompareControl<_ccc1,_ccr1>;
	using CapCompControl2 = CaptureCompareControl<_ccc2,_ccr2>;

	static callback_t mCallback;

	/**@brief Enable the overflow irq of the timer
	 */
	static void intEnable() { _DINT(); REG_16(_control) |= TAIE;  _EINT();}

	/**@brief Disable the overflow irq of the timer
	 */
	static void intDisable() { REG_16(_control) &= ~TAIE; }

	static bool isIntEnable() { return REG_16(_control) & TAIE; }

	static void clearTimer() { REG_16(_control) |= TACLR; }

	static void stopTimer() { REG_16(_control) &= ~MC_3; }

	static bool clearOverFlow() {return REG_16(_taiv) &= ~TAIV;}

	static uint16_t getCounterValue() { return REG_16(_counter); }

	static bool isCaptureOverFlowSet() {return REG_16(_control) & COV;}

	static bool isOverFlowFlagSet() { return REG_16(_taiv) & TAIFG;}

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

	static constexpr uint32_t computeTickTimingNs()
	{
		return (uint32_t)(computeTickTiming() * 1000000000.0F);
	}

	static constexpr float computeOverFlowTiming()
	{
		return computeTickTiming()*0xFFFF;
	}

	static constexpr TimerSource Source = _source;
	static constexpr uint32_t ClockSpeed = _clockspeed;
	static constexpr ClockDivider Divider = _divider;


};


template< class _timer, class _timerconfig, class _ccc, class _gpio, CapCompSelect _ccis=CapCompSelect::CCIA, bool _pulsehigh=true >
class PulseWidthMeasure
{
public:

	static void init()
	{
		_gpio::clear();
		_gpio::input();
		_gpio::selectOn();//Only works for Port2?
		intEnable();
		_ccc::mCallback = &timerCallback;
	}

	//Same timer interrupt
	static void intEnable()
	{
		_ccc::intEnable();
	}

	//Same timer interrupt
	static void intDisable()
	{
		_ccc::intDisable();
	}

	static void start()
	{
		_timer::clearTimer();  //copy paste from continuous mode timer... TODO clean up
		_timer::startTimer(TimerMode::CONT_MODE,_timerconfig::Source,_timerconfig::Divider);
		_ccc::startCapture(_ccis,CapMode::DUAL_EDGE);
	}

	static void stop()
	{
		_timer::stopTimer();
		_ccc::stopCapture();
		(void)_ccc::readCaptureFlag();

	}

	static uint16_t getPulseWidthTicks()
	{
		return mEndCount - mStartCount;
	}

	static uint32_t getPulseWidthNs()
	{
		//TODO determine the formula for ticks to us
		return getPulseWidthTicks()*_timerconfig::computeTickTimingNs();
	}

	static void timerCallback(callback_args_t callback)
	{
		const uint16_t count = _ccc::getCapCompValue();
		if(_ccc::getCovValue()) {
			_ccc::clearCov();
		}
		if(_pulsehigh) {
			setCounts(_ccc::getCciValue(), count);
		}
		else {

			setCounts(!_ccc::getCciValue(), count);
		}
	}

	static inline void setCounts(const bool start, const uint16_t count)
	{
		if(start) {

			mStartCount = count;
		}
		else {
			McuPin<McuPort1,BIT0>::toggle();
			mEndCount = count;
			mCallback(0); //send the pulse width to user
		}
	}

	static void setCallback(callback_t callback)
	{
		mCallback = callback; //User callback
	}

	static volatile uint16_t mStartCount;
	static volatile uint16_t mEndCount;
	static callback_t mCallback;

private:
	PulseWidthMeasure() =delete;
	PulseWidthMeasure(const PulseWidthMeasure&) =delete;
	PulseWidthMeasure& operator=(const PulseWidthMeasure&) =delete;

};


template< class _timer, class _timerconfig >
class ContinuousTimer
{
public:

	static void intEnable()
	{
		_timer::intEnable();
	}

	static void start()
	{
		_timer::clearTimer();
		_timer::startTimer(TimerMode::CONT_MODE,_timerconfig::Source,_timerconfig::Divider);
	}

	static void setCallback(callback_t callback)
	{
		_timer::mCallback = callback;
	}

	static uint16_t getTimerCount(){ return _timer::getCounterValue(); }

	static void stop() { _timer::stopTimer(); }

private:

	ContinuousTimer() =delete;
	ContinuousTimer(const ContinuousTimer&) =delete;
	ContinuousTimer& operator=(const ContinuousTimer&) =delete;
};

template<uint16_t _control, uint16_t _counter, uint16_t _taiv,uint16_t _ccc0,uint16_t _ccc1, uint16_t _ccc2, uint16_t _ccr0, uint16_t _ccr1, uint16_t _ccr2 >
McuPeripheral::callback_t McuPeripheral::TimerControl<_control,_counter, _taiv, _ccc0,_ccc1, _ccc2,_ccr0,_ccr1,_ccr2>::mCallback;

template< class _timer, class _timerconfig, class _ccc, class _gpio, CapCompSelect _ccis, bool _pulsehigh >
volatile uint16_t PulseWidthMeasure<_timer, _timerconfig, _ccc, _gpio, _ccis, _pulsehigh>::mStartCount;

template< class _timer, class _timerconfig, class _ccc, class _gpio, CapCompSelect _ccis, bool _pulsehigh >
volatile uint16_t PulseWidthMeasure<_timer, _timerconfig, _ccc, _gpio, _ccis, _pulsehigh>::mEndCount;

template< class _timer, class _timerconfig, class _ccc, class _gpio, CapCompSelect _ccis, bool _pulsehigh >
callback_t PulseWidthMeasure<_timer, _timerconfig, _ccc, _gpio, _ccis, _pulsehigh>::mCallback;

template< uint16_t _ccc, uint16_t _ccr >
callback_t CaptureCompareControl<_ccc,_ccr>::mCallback;

}

using Timer_Source = McuPeripheral::TimerSource;
using Timer_Mode = McuPeripheral::TimerMode;
using Timer0 = McuPeripheral::TimerControl<TACTL_, TAR_, TAIV_, TACCTL0_, TACCTL1_, TACCTL2_, TACCR0_, TACCR1_, TACCR2_>;
using Timer1 = McuPeripheral::TimerControl<TA1CTL_, TA1R_, TAIV_, TA1CCTL0_, TA1CCTL1_, TA1CCTL2_, TA1CCR0_, TA1CCR1_, TA1CCR2_>;

#endif //_MSP_TIMER_HPP
