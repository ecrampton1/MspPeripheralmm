#include "mcu_config.hpp"
#include <stdlib.h>
#include <cstdint>


static const char* AesKey = "thisIsEncryptKey";

static uint8_t buffer[66];


struct Payload {
  int           nodeId; //store this nodeId
  unsigned long uptime; //uptime in ms
  unsigned long         temp;   //temperature maybe?
};

void setup()
{
	led0::clear();
	led0::output();
	sys::init();
	sys::enableWatchDog(); //starts counting for system time
	uart::init();
	uart::send("Init\n");
	rfm69::init();
	//rfm69::setEncryptionKey(AesKey);
	rfm69::enableRx();
	rfm69::printAllRegisters();
	rfm69::setNodeAddress(99);
	rfm69::setNetworkAddress(100);
}


void receiver()
{
	PacketHeader* header = reinterpret_cast<PacketHeader*>(buffer);
	if(rfm69::isPayloadReady()) {
		led0::toggle();
		rfm69::readPayload(buffer,sizeof(buffer));
		uart::send(header->Control);
		uart::sendLine();
		Payload* payload  = reinterpret_cast<Payload*>(&buffer[sizeof(PacketHeader)]);
		PRINT(payload->nodeId," - ",payload->uptime," - ",payload->temp,ENDL)
		rfm69::enableRx();
	}
}

void sender()
{
	Payload* payload = reinterpret_cast<Payload*>(&buffer[0]);
	payload->nodeId = 99;
	payload->uptime = sys::millis();
	payload->temp = 37;

	bool ret = rfm69::writePayloadWithAck(buffer,sizeof(Payload),99);
	if(ret == true) {
		led0::toggle();
	}
	else {
		led0::set();
	}

}

void loop()
{

	receiver();
	//sender();
	//sys::delayInMs(100);

}


int main(void)
{
	setup();

	while(1)
	{
		loop();
	}
}



