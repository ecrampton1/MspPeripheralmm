#ifndef _DHT_HPP_
#define _DHT_HPP_

#include <msp430.h>
#include <string.h>
#include "mcu_config.hpp"
#include "utilities/utilities.hpp"

enum class DHTStates
{
	SEND_START,
	RECEIVE_BIT_START,
	RECEIVE_BIT_END,
	END,
};


struct DHTRawData
{
	uint8_t StartBit;
	uint8_t HumidityReal;
	uint8_t HumidityDecimal;
	uint8_t TemperatureReal;
	uint8_t TemperatureDecimal;
	uint8_t Crc;

	bool checkCRC() const
	{
		const uint8_t* ptr = &HumidityReal;
		int total = 0;
		//skip start bit Byte and Crc Byte
		for( size_t i =0; i < sizeof(DHTRawData)-2; ++i) {
			total += ptr[i];
		}
		return (Crc == total);
	}
};

constexpr int calc_high_time(uint32_t clock)
{ return clock / 1000000 * 110;} //110us * clock period in us

constexpr int wait_low_time(uint32_t clock)
{ return clock / 1000000 * 100;} //110us * clock period in us

constexpr int wait_high_time(uint32_t clock)
{ return clock / 1000000 * 200;} //110us * clock period in us


template<class _datapin,class _timer, class _sys,Speed _clock>
class DHT
{
public:

	static bool dhtReadBlocking()
	{
		bool ret = false;
		init();
		startPollingComm();
		if(startResponse()){
			ret = readBits();
		}
		return (ret) ? mData.checkCRC() : ret;
	}

	static const DHTRawData& getData() { return mData; }

	static bool dhtReadNonBlocking()
	{
		if(mState != DHTStates::END || mCallback == nullptr) {
			return false;
		}
		init();
		startInterruptComm();
		return true;
	}

	static void setCallback(McuPeripheral::callback_t callback, void* args)
	{
		mCallback = callback;
		mCallbackArgs = args;
	}

private:

	static constexpr int HIGHBITTIME = calc_high_time(static_cast<uint32_t>(_clock));
	static constexpr int WAITLOWTIME = wait_low_time(static_cast<uint32_t>(_clock));
	static constexpr int WAITHIGHTIME = wait_high_time(static_cast<uint32_t>(_clock));

	static inline void init()
	{
		mState = DHTStates::SEND_START;
		mMask = 1; //start bit is a single bit that is captured
		memset(&mData,0,sizeof(DHTRawData));
	}


	static void startBitTimerComplete(void* args)
	{
		_timer::stop();
		_timer::startFreeRunningTimer();

		if(mState == DHTStates::SEND_START) {
			_datapin::pullUp();
			_datapin::edgeLowToHigh();//wait for the rising edge
			mState = DHTStates::RECEIVE_BIT_START;
			//mSysTime = _sys::micros();
			mSysTime = _timer::getTimerCount();
			_datapin::input(); //THis should pull the line high until the sensor pulls it low
			//while(_datapin::read()); //wait for it to go high
			_sys::delayInUs(20);
			_datapin::intEnable();
		}
		else {
			mState = DHTStates::END;
		}
	}

	static void rxBitGpioTriggered(void* args)
	{
		P1OUT = 0x01;
		if(mState == DHTStates::RECEIVE_BIT_START) {
			mState = DHTStates::RECEIVE_BIT_END;
			_datapin::edgeHighToLow();//wait for the rising edge
		}else if(mState == DHTStates::RECEIVE_BIT_END) {
			//mState = DHTStates::RECEIVE_BIT_START;
			uint16_t end = _timer::getTimerCount();
			//uint16_t end = _sys::micros();
			//if ((end - mSysTime) > 110) {           // If time < 28us is 0 70us is 1.
			if ((end - mSysTime) > HIGHBITTIME) {           // If time < 28us is 0 70us is 1.
				*mIrqIndexPtr |= mMask;
			}
			mSysTime = end;
			if (!(mMask >>= 1)) {              // Shift mask, move to next byte when mask is zero
				mMask = 0x80;
				++mIrqIndexPtr;
			}
			//_datapin::edgeLowToHigh();//wait for the rising
			if(mIrqIndexPtr > (&mData.Crc)){
				mState = DHTStates::END;
				_datapin::intDisable();
				_timer::stop();
				mCallback(mCallbackArgs);
			}
		}
	}

	static void dataPinStart()
	{
		_datapin::intDisable();
		_datapin::clear();
		_datapin::output();
		_datapin::pullOff();
	}

	//Note this starts a timer and returns to main thread
	static void startInterruptComm()
	{
		_datapin::setPinIrqHandler(&rxBitGpioTriggered,0);
		_timer::set_callback(&startBitTimerComplete,0);
		_timer::start();
		dataPinStart();
		mIrqIndexPtr = reinterpret_cast< uint8_t*>(&mData);
	}

	static inline void startPollingComm()
	{
		_timer::startFreeRunningTimer();
		//Pull low for at least 18 ms
		dataPinStart();
		_sys::delayInMs(20);

		//pull high for 20-40us (this may need to change due to timing of dht)
		_datapin::pullUp();
		_datapin::set();
		//_sys::delayInUs(30);
	}

	static inline bool startResponse()
	{
		//Allow dht to drive pin low
		_datapin::input();
		uint16_t start = _timer::getTimerCount();

		while(_datapin::read()) {
			//quick dirty wait
			if((_timer::getTimerCount() - start) > WAITLOWTIME) {
				return false;
			}
		}
		return true;
	}

	//TODO fix this to not assume 16MHZ
	static inline bool readBits()
	{
		uint8_t* ptr = reinterpret_cast<uint8_t*>(&mData);
		uint16_t start;
		uint16_t end = _timer::getTimerCount();       // Get start time for timeout
		do {
				start= end; // Start time of this bit is end time of previous bit
				while ( !(_datapin::read()) ) {        // Wait while low, return if stuck low
						if ((_timer::getTimerCount() - start) > WAITLOWTIME) {
							return false;
						}
				}

				while (_datapin::read()) {             // Wait while high, return if stuck high
						if ((_timer::getTimerCount() - start) > WAITHIGHTIME) {
							return false;
						}
				}

				end = _timer::getTimerCount();                     // Get end time
				if ((end - start) > HIGHBITTIME) {           // If time > 110uS, then it is a "one" bit.
					*ptr |= mMask;
				}

				if (!(mMask >>= 1)) {              // Shift mask, move to next byte when mask is zero
					mMask = 0x80;
					++ptr;
				}
		} while (ptr <= ((&mData.Crc)));                     // Do until array is full
		_timer::stop();
		return true;
	}

	static McuPeripheral::SystemTime mSysTime;
	static McuPeripheral::callback_t mCallback;
	static void* mCallbackArgs;
	static DHTStates mState;
	static DHTRawData mData;
	static uint8_t mMask;
	volatile static uint8_t* mIrqIndexPtr;
};
template<class _datapin,class _timer, class _sys, Speed _clock>
DHTStates DHT<_datapin,_timer,_sys,_clock>::mState = DHTStates::END;
template<class _datapin,class _timer, class _sys, Speed _clock>
DHTRawData DHT<_datapin,_timer,_sys,_clock>::mData;
template<class _datapin,class _timer, class _sys, Speed _clock>
uint8_t DHT<_datapin,_timer,_sys,_clock>::mMask;
template<class _datapin,class _timer, class _sys, Speed _clock>
McuPeripheral::callback_t DHT<_datapin,_timer,_sys,_clock>::mCallback = nullptr;
template<class _datapin,class _timer, class _sys, Speed _clock>
void* DHT<_datapin,_timer,_sys,_clock>::mCallbackArgs = nullptr;
template<class _datapin,class _timer, class _sys, Speed _clock>
volatile uint8_t* DHT<_datapin,_timer,_sys,_clock>::mIrqIndexPtr = nullptr;
template<class _datapin,class _timer, class _sys, Speed _clock>
McuPeripheral::SystemTime DHT<_datapin,_timer,_sys,_clock>::mSysTime = 0;



#endif //_DHT_HPP_
