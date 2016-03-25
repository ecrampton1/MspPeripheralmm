#ifndef _MSP_PERIPH_HPP
#define _MSP_PERIPH_HPP

#include <msp430.h>
#include "msp430/msp_sys.hpp"
#include "utilities/circfifo.hpp"

#define REG_16(x) *((volatile uint16_t*)x)
#define REG_8(x) *((volatile uint8_t*)x)

namespace McuPeripheral {


struct DisableAllInterrupts {
	void operator () (){
		__dint();
	}
};

struct EnableAllInterrupts {
	void operator () (){
		__eint();
	}
};

template<uint8_t _ienable, uint8_t  _iflag>
struct DisableInterrupt {
	void operator () (){
		REG_8(_ienable) &= ~_iflag;
	}
};

template<uint8_t _ienable, uint8_t  _iflag>
struct EnableInterrupt {
	void operator () (){
		REG_8(_ienable) |= _iflag;
	}
};

//Should be a base class to inherit from
struct FakeDisableInterrupt {
	void operator () (){
		//Do nothing
	}
};

struct FakeEnableInterrupt {
	void operator () (){
		//Do nothing
	}
};


template<uint8_t _ienable,uint8_t _txen, uint8_t _rxen>
class Interrupts {
public:
	static FifoBuffer<uint8_t, 8, McuPeripheral::DisableInterrupt<_ienable,_txen>, EnableInterrupt<_ienable,_txen> > mTxBuffer;
	static FifoBuffer<uint8_t, 8, McuPeripheral::DisableInterrupt<_ienable,_rxen>, EnableInterrupt<_ienable,_rxen> > mRxBuffer;

	static void init() { REG_8(_ienable) &= ~(_rxen | _txen); }
	static DisableInterrupt<_ienable,_txen> disableTxInterrupt;
	static DisableInterrupt<_ienable,_rxen> disableRxInterrupt;
	static EnableInterrupt<_ienable,_txen> enableTxInterrupt;
	static EnableInterrupt<_ienable,_rxen> enableRxInterrupt;
};

template<uint8_t _ienable,uint8_t _txen, uint8_t _rxen>
FifoBuffer<uint8_t, 8, McuPeripheral::DisableInterrupt<_ienable,_txen>, EnableInterrupt<_ienable,_txen> > Interrupts<_ienable,_txen,_rxen>::mTxBuffer;

template<uint8_t _ienable,uint8_t _txen, uint8_t _rxen>
FifoBuffer<uint8_t, 8, McuPeripheral::DisableInterrupt<_ienable,_rxen>, EnableInterrupt<_ienable,_rxen> > Interrupts<_ienable,_txen,_rxen>::mRxBuffer;

template<uint8_t _ienable,uint8_t _txen, uint8_t _rxen>
DisableInterrupt<_ienable,_txen> Interrupts<_ienable,_txen,_rxen>::disableTxInterrupt;

template<uint8_t _ienable,uint8_t _txen, uint8_t _rxen>
DisableInterrupt<_ienable,_rxen> Interrupts<_ienable,_txen,_rxen>::disableRxInterrupt;

template<uint8_t _ienable,uint8_t _txen, uint8_t _rxen>
EnableInterrupt<_ienable,_txen> Interrupts<_ienable,_txen,_rxen>::enableTxInterrupt;

template<uint8_t _ienable,uint8_t _txen, uint8_t _rxen>
EnableInterrupt<_ienable,_rxen> Interrupts<_ienable,_txen,_rxen>::enableRxInterrupt;


class FakeInterupts {
public:
	static FakeFifoBuffer mTxBuffer;
	static FakeFifoBuffer mRxBuffer;
	static void init() { return; }

	static FakeDisableInterrupt disableTxInterrupt;
	static FakeDisableInterrupt disableRxInterrupt;
	static FakeEnableInterrupt enableTxInterrupt;
	static FakeEnableInterrupt enableRxInterrupt;
};

}

#endif //_MSP_PERIPH_HPP
