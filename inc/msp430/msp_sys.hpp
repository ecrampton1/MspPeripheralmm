#ifndef _MSP_SYS_HPP
#define _MSP_SYS_HPP

#include "mcu_sys.hpp"


namespace McuPeripheral {


template<McuSpeed M>
class McuSystem : public SystemBase
{
public:
	static void init();
	//static uint32_t getSpeed()


};

};


typedef McuPeripheral::McuSpeed Speed;
#endif //_MSP_SYS_HPP
