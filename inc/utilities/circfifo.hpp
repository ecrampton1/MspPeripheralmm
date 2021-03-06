#ifndef _CIRC_FIFO_HPP
#define _CIRC_FIFO_HPP
#include "msp430/msp_sys.hpp"


template<class T, int _size, class _lock, class _unlock>
class FifoBuffer  {
public:

	bool push(const T data){
		if(full()) {
			return false;
		}

		//Put at the head of the buffer
		--mHead;
		mBuffer[mHead] = data;

		//When we reach 0 wrap around the buffer
		if(0 == mHead)
			mHead = _size;

		//Lock before checking because this could be accessed in an ISR
		//Note the template passes in the class with overloaded operator for functor
		Lock();
		if(mTail == mHead)
			mHead = 0;
		UnLock();

		return true;
	}

	bool pop (T& return_data){
		if(empty()) {
			return false;
		}

		// If we were full head==0 then we set head to where tail used to be
		Lock();
		if(full())
			mHead = mTail;
		UnLock();

		//Pop from tail
		--mTail;
		return_data = mBuffer[mTail];

		//After setting head if full we can safely set the tail to wrap around
		if(0 == mTail)
			mTail = _size;

		return true;
	}

	void init() {
		mHead = _size;
		mTail = _size;
	}

	inline bool full() {
		return (mHead == 0);
	}

	inline bool empty() {
		return (mTail == mHead);
	}

private:

	static _lock Lock;
	static _unlock UnLock;

	volatile T mBuffer[_size];
	volatile uint8_t mHead;
	volatile uint8_t mTail;
};

class FakeFifoBuffer  {
public:

	template<class T>
	bool push(T data){
		return false;
	}

	template<class T>
	bool pop (T& return_data){
		return false;
	}

	void init() {
		//do nothing
	}

	inline bool full() {
		//do nothing
		return true;
	}

	inline bool empty() {
		//do nothing
		return true;
	}

};

#endif //_CIRC_FIFO_HPP
