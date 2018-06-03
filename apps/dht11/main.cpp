#include "mcu_config.hpp"
//#include <limits.h>
//#include "msp430/msp_timer.hpp"
#include <msp430.h>
#include <string.h>
#include "dht/dht11.hpp"

extern "C" {
#include <stdlib.h>
}

/*bool dhtPollingRead();
void dhtInitPollingComm();

struct DHTRawData
{
	uint8_t StartBit;
	uint8_t HumidityReal;
	uint8_t HumidityDecimal;
	uint8_t TemperatureReal;
	uint8_t TemperatureDecimal;
	uint8_t Crc;

	bool checkCRC()
	{
		uint8_t* ptr = &HumidityReal;
		int total = 0;
		//skip start bit Byte and Crc Byte
		for( size_t i =0; i < sizeof(DHTRawData)-2; ++i) {
			total += ptr[i];
		}
		PRINT("Expected: ", (int)Crc, "Calc: ", total,ENDL)
		return (Crc == total);
	}
};

static DHTRawData gDHTRawData;*/
using dht = DHT<dhtDataPin,msp430Timer,sys,clock_speed>;

static volatile bool gReadComplete;

static void dhtReadComplete(void* args)
{
	gReadComplete = true;
}

int main()
{

	sys::init();
	sys::enableWatchDog(); //starts counting for system time
	led0::output();
	led0::clear();
	led1::output();
	led1::clear();
	uart::init();
	uart::send("Start DHT Test\n");
	sys::delayInMs(1000);


	while(1) {
		gReadComplete = false;
		//if(dht::dhtReadBlocking()) {
		if(dht::serviceOnce()) {
			auto data = dht::getData();
			PRINT((int)data.StartBit,ENDL)
			PRINT((int)data.HumidityDecimal,ENDL)
			PRINT((int)data.HumidityReal,ENDL)
			PRINT((int)data.TemperatureDecimal,ENDL)
			PRINT((int)data.TemperatureReal,ENDL)
			PRINT((int)data.Crc,ENDL,ENDL)
		}
		else {
			PRINT("Failed\n")
		}
		sys::delayInMs(3000);
		led1::toggle();

	}

}



/*bool dhtPollingRead()
{

	uint8_t* ptr = reinterpret_cast<uint8_t*>(&gDHTRawData);
	uint8_t mask=1;
	memset(ptr,0,sizeof(DHTRawData));
	dhtInitPollingComm();

	//Allow dht to drive pin low
	dhtDataPin::input();
	uint16_t start = mcuTimer0::getTimerCount();

	while(dhtDataPin::read()) {
		//quick dirty wait
		if((mcuTimer0::getTimerCount() - start) > 1600) {
			return false;
		}
	}


	uint16_t end = mcuTimer0::getTimerCount();       // Get start time for timeout
	do {
			start= end; // Start time of this bit is end time of previous bit
			while ( !(dhtDataPin::read()) ) {        // Wait while low, return if stuck low
					if ((mcuTimer0::getTimerCount() - start) > 1600) {
						return false;
					}
			}
			while (dhtDataPin::read()) {             // Wait while high, return if stuck high
					if ((mcuTimer0::getTimerCount() - start) > 3200) {
						return false;
					}
			}
			end = mcuTimer0::getTimerCount();                     // Get end time
			if ((end - start) > 1760)           // If time > 110uS, then it is a "one" bit.
					*ptr |= mask;
			if (!(mask >>= 1)) {              // Shift mask, move to next byte when mask is zero
					mask = 0x80;
					++ptr;
			}
	} while (ptr <= ((&gDHTRawData.Crc)));                     // Do until array is full


	mcuTimer0::stop();
	return gDHTRawData.checkCRC();
}

void dhtInitPollingComm()
{
	mcuTimer0::startFreeRunningTimer();
	//Pull low for at least 18 ms
	dhtDataPin::clear();
	dhtDataPin::output();
	dhtDataPin::pullOff();
	sys::delayInMs(18);

	//pull high for 20-40us (this may need to change due to timing of dht)
	dhtDataPin::pullUp();
	dhtDataPin::set();
	sys::delayInUs(30);
}*/
