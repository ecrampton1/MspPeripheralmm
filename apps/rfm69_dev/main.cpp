#include "mcu_config.hpp"
#include <stdlib.h>
#include <cstdint>


static const char* AesKey = "thisIsEncryptKey";



void setup() {
	sys::init();

	uart::init();
	sys::enableWatchDog(); //starts counting for system time
	Handler::begin(9);

	nvmSegD::init();
	//initializeIncomingMessages();
	PRINT("SETUP!",ENDL)
	uint32_t temp = 0;
	const uint32_t expected = 0x12345678;
	bool ret = false;
	//Temporary flash setup.

	ret = nvmSegD::read_block(0,(uint8_t*)&temp,sizeof(temp));

	if((temp == expected))
	{
		PRINT("Match Expected: ",expected,"Actual: ", temp,ENDL)
	}
	else
	{

		ret = nvmSegD::write_byte(0,(uint8_t*)&expected,sizeof(expected));
		PRINT("Expected: ",expected,"Actual: ", temp,ENDL)
	}

}

void loop() {

	Handler::serviceOnce();
}


int main(void)
{
	setup();

	while(1)
	{
		loop();
	}
}



