#ifndef _CIRC_FIFO_HPP
#define _CIRC_FIFO_HPP
#include "msp430/msp_sys.hpp"

template<class T, int _size, class _lock, class _unlock>
class FifoBuffer  {
public:

	bool push(T data){
		if(full()) {
			return false;
		}

		/* Place byte at position preceding head. */
		--mHead;
		mBuffer[mHead] = data;

		/* If byte just went to position 0, wrap next head index around. */
		if(0 == mHead)
			mHead = _size;

		/* If next head index caught up to tail, then buffer is full. */
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

		/* Read byte preceding tail. */
		--mTail;
		return_data = mBuffer[mTail];

		/* Wrap next tail index around if byte was at 0. */
		if(0 == mTail)
			mTail = _size;

		/* If buffer was full before...it is no longer.
		Point head to where tail is currently.
		Disable/enable interrupts for safety. */
		Lock();  //include a marco specific for msp
		if(full())
			mHead = mTail;
		UnLock();

		return true;
	}

private:

	_lock Lock;
	_unlock UnLock;

	inline bool full() {
		return (mHead == 0);
	}

	inline bool empty() {
		return (mTail == mHead);
	}

	T mBuffer[_size];
	uint8_t mHead = _size;
	uint8_t mTail = _size;
};




#endif //_CIRC_FIFO_HPP
