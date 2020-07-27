#ifndef _MSP_NVM_HPP
#define _MSP_NVM_HPP
#include <stdint.h>
#include <msp430.h>
#include "utilities/utilities.hpp"
#include "mcu_sys.hpp"

namespace McuPeripheral {

constexpr uint8_t calculate_divider(uint32_t speed)
{
	return ((speed / static_cast<uint32_t>(McuSpeed::SPEED_1MHZ)) *2) + 2;
}

static constexpr uint16_t SEGASTART = 0x10C0;
static constexpr uint16_t SEGBSTART = 0x1080;
static constexpr uint16_t SEGCSTART = 0x1040;
static constexpr uint16_t SEGDSTART = 0x1000;
static constexpr uint16_t INFOSEGSIZE = 64;

template < uint16_t _flash_segment, uint16_t _flash_size, bool _lockA=false>
class McuFlashSegment
{
public:

	static const uint16_t segmentAddress = _flash_segment;
	static const uint16_t flashSize = _flash_size;
	static const bool lockA = _lockA;

};

//Currently only used with information flash (how to expand to other addresses
template < class _flash, McuSpeed _clock>
class McuNvm
{
public:

	static constexpr uint8_t divider = (_clock == McuSpeed::SPEED_1MHZ) ? 3 : calculate_divider(static_cast<uint32_t>(_clock));


	static void init()
	{
		FCTL2 = FWKEY + FSSEL_3 + divider; //set flash speed
	}

	/*
	 * Index from the _flashsegment starting address.
	 */
	static bool read_block(uint16_t startIndex, uint8_t* retPtr, uint16_t sizeInBytes=1)
	{
		bool ret = false;

		if(startIndex < _flash::flashSize)
		{
			uint8_t* ptr = (uint8_t*)(0x1000+startIndex);

			unlock();                       // Clear Lock bit
			for(int i = 0; i < sizeInBytes; ++i)
			{
				*retPtr++ = *ptr++;
			}
			lock();
			ret = true;
		}

		return ret;
	}

	/*
	 * Note currently this will erase the entire segment and only writ this index back
	 * If using more memory in the segment read out the contents and rewrite them back to flash.
	 */
	static bool write_byte(uint16_t startIndex, uint8_t* writePtr, uint16_t size)
	{
		uint8_t* ptr = (uint8_t*)(0x1000+startIndex);
		bool ret = false;
		if(startIndex < _flash::flashSize)
		{
			unlock();
			erase();

			while(FCTL3 & BUSY);//wait for erasure to finish
			FCTL1 = FWKEY + WRT;                      // Set WRT bit for write operation

			for(int i = 0; i < size; ++i)
			{
				while(FCTL3 & BUSY);//wait for erasure to finish
				*ptr++ = *writePtr++;
			}

			FCTL1 = FWKEY; //clear write
			lock();
			ret = true;
		}
		return ret;
	}


private:

	static void unlock()
	{
		if(_flash::lockA)
		{
			FCTL3 = FWKEY + LOCKA;                    // Clear LOCK & LOCKA bits
		}
		else
		{
			FCTL3 = FWKEY;                    // Clear LOCK & LOCKA bits
		}
	}

	static void lock()
	{
		uint16_t setLockA = ((FCTL3 & LOCKA) > 0) ? 0 : LOCKA;
		FCTL3 = FWKEY + setLockA + LOCK;             // Set LOCK & LOCKA bit
	}

	//erases the entire segment.
	static void erase()
	{
		uint8_t* ptr = (uint8_t*)_flash::segmentAddress;

		FCTL1 = FWKEY + ERASE;                    // Set Erase bit

		*ptr = 0x00;                       // Dummy write to erase Flash seg
		FCTL1 = FWKEY;                       // Clear WRT bitFCTL1 = FWKEY;                       // Clear WRT bit

	}
};



using InformationAMemory = McuFlashSegment<SEGASTART,INFOSEGSIZE,true>;
using InformationBMemory = McuFlashSegment<SEGBSTART,INFOSEGSIZE>;
using InformationCMemory = McuFlashSegment<SEGCSTART,INFOSEGSIZE>;
using InformationDMemory = McuFlashSegment<SEGDSTART,INFOSEGSIZE>;

}


#endif //_MSP_GPIO_HPP
