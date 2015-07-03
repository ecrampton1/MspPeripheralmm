#include "mcu_sys.hpp"
#include <msp430.h>

namespace McuPeripheral {

uint32_t SystemBase::mSpeed = 1000000;//1MHZ

uint32_t SystemBase::getSpeed()
{
		return mSpeed;
}

}
