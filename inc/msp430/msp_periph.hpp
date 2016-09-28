#ifndef _MSP_PERIPH_HPP
#define _MSP_PERIPH_HPP

#include <msp430.h>
#include "msp430/msp_sys.hpp"
#include "utilities/circfifo.hpp"

#define REG_16(x) *((volatile uint16_t*)x)
#define REG_8(x) *((volatile uint8_t*)x)

namespace McuPeripheral {

/**@brief Wrapper to disable all interrupts
 */
struct DisableAllInterrupts {
	void operator () (){
		__dint();
	}
};

/**@brief Wrapper to enable all interrupts
 */
struct EnableAllInterrupts {
	void operator () (){
		__eint();
	}
};

/**@brief Wrapper to disable specifc interrupt or interrupts
 * tparam _ienable the interrupt register that the _iflag is cleared to disable interrupts
 * tparam _iflag the flag in the _ienable register to be cleared for disabling interrupt
 */
template<uint8_t _ienable, uint8_t  _iflag>
struct DisableInterrupt {
	void operator () (){
		REG_8(_ienable) &= ~_iflag;
	}
};

/**@brief Wrapper to enable specifc interrupt or interrupts
 * tparam _ienable the interrupt register that the _iflag is cleared to enable interrupts
 * tparam _iflag the flag in the _ienable register to be cleared for enabling interrupt
 */
template<uint8_t _ienable, uint8_t  _iflag>
struct EnableInterrupt {
	void operator () (){
		REG_8(_ienable) |= _iflag;
	}
};

/**@brief Empty class that can be used as a dummy when interrupts are not needed for disabling or enabling
 */
struct FakeInterrupt {
	void operator () (){
		//Do nothing
	}
};

/**@brief Templated class for specifing interrupts used in communications
 * tparam _ienable the interrupt register that the _iflag is set/cleared for interrupts
 * tparam _txen tx flag for enabling and disabling interrupts
 * tparam _rxen rx flag for enabling and disabling interrupts
 */
template<uint8_t _ienable,uint8_t _txen, uint8_t _rxen, bool _isinterrupt = true>
class Interrupts {
public:
	static FifoBuffer<uint8_t, 8, McuPeripheral::DisableInterrupt<_ienable,_txen>, EnableInterrupt<_ienable,_txen> > mTxBuffer;
	static FifoBuffer<uint8_t, 8, McuPeripheral::DisableInterrupt<_ienable,_rxen>, EnableInterrupt<_ienable,_rxen> > mRxBuffer;

	static void init() { REG_8(_ienable) &= ~(_rxen | _txen); }
	static bool isInterrupt() { return _isinterrupt; }

	static DisableInterrupt<_ienable,_txen> disableTxInterrupt;
	static DisableInterrupt<_ienable,_rxen> disableRxInterrupt;
	static EnableInterrupt<_ienable,_txen> enableTxInterrupt;
	static EnableInterrupt<_ienable,_rxen> enableRxInterrupt;
};

template<uint8_t _ienable,uint8_t _txen, uint8_t _rxen, bool _isinterrupt>
FifoBuffer<uint8_t, 8, McuPeripheral::DisableInterrupt<_ienable,_txen>, EnableInterrupt<_ienable,_txen> > Interrupts<_ienable,_txen,_rxen, _isinterrupt>::mTxBuffer;

template<uint8_t _ienable,uint8_t _txen, uint8_t _rxen, bool _isinterrupt>
FifoBuffer<uint8_t, 8, McuPeripheral::DisableInterrupt<_ienable,_rxen>, EnableInterrupt<_ienable,_rxen> > Interrupts<_ienable,_txen,_rxen, _isinterrupt>::mRxBuffer;

template<uint8_t _ienable,uint8_t _txen, uint8_t _rxen, bool _isinterrupt>
DisableInterrupt<_ienable,_txen> Interrupts<_ienable,_txen,_rxen, _isinterrupt>::disableTxInterrupt;

template<uint8_t _ienable,uint8_t _txen, uint8_t _rxen, bool _isinterrupt>
DisableInterrupt<_ienable,_rxen> Interrupts<_ienable,_txen,_rxen, _isinterrupt>::disableRxInterrupt;

template<uint8_t _ienable,uint8_t _txen, uint8_t _rxen, bool _isinterrupt>
EnableInterrupt<_ienable,_txen> Interrupts<_ienable,_txen,_rxen, _isinterrupt>::enableTxInterrupt;

/**@brief dummy class for interrupts that are used when interrupts are not to be enabled.
 *        this is used for polling functionality of the communication.
 */
template<bool _isinterrupt = false>
class FakeInterupts {
public:
	static FakeFifoBuffer mTxBuffer;
	static FakeFifoBuffer mRxBuffer;
	static void init() { return; }
	static bool isInterrupt()  { return _isinterrupt; }

	static FakeInterrupt disableTxInterrupt;
	static FakeInterrupt disableRxInterrupt;
	static FakeInterrupt enableTxInterrupt;
	static FakeInterrupt enableRxInterrupt;
};

}

#endif //_MSP_PERIPH_HPP
