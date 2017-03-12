#ifndef _MCU_SYS_HPP
#define _MCU_SYS_HPP
#include <stdint.h>

namespace McuPeripheral {



enum class McuSpeed : uint32_t { SPEED_1MHZ = 1000000, SPEED_8MHZ= 8000000, SPEED_12MHZ= 12000000, SPEED_16MHZ=16000000 };

#define FOR_ALL_SPEEDS( ACTION )\
	ACTION( 1MHZ )\
	ACTION( 8MHZ )\
	ACTION( 12MHZ )\
	ACTION( 16MHZ )\

class SystemBase
{
public:

protected:
	//static uint32_t mSpeed;

};



}


#endif //_MCU_SYS_HPP
