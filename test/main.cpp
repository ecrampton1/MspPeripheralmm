#include "msp430/msp_sys.hpp"
#include "msp430/msp_gpio.hpp"
#include <limits.h>

extern "C" {
#include <stdlib.h>
}


namespace Periph = McuPeripheral;

typedef Periph::McuPin<McuPort1,BIT0> Led;
typedef Periph::McuPin<McuPort1,BIT6> Led2;

int main()
{
	Periph::McuSystem<Speed::SPEED_1MHZ>::init();
	Led::output();
	Led2::output();
	Led::clear();

	while(1) {
		Led::toggle();
		Led2::toggle();
		__delay_cycles(USHRT_MAX);
	}
	return 0;
}



