#ifndef _MCU_SYS_HPP
#define _MCU_SYS_HPP

namespace McuPeripheral {

enum class McuSpeed { SPEED_1MHZ, SPEED_8MHZ, SPEED_12MHZ, SPEED_16MHZ };

#define FOR_ALL_SPEEDS( ACTION )\
	ACTION( 1MHZ )\
	ACTION( 8MHZ )\
	ACTION( 12MHZ )\
	ACTION( 16MHZ )\

class SystemBase
{
public:

	static void init();
	static void enable_irq();
	static void disable_irq();
	static McuSpeed getSpeed();
protected:
	static McuSpeed mSpeed;

};



};


#endif //_MCU_SYS_HPP
