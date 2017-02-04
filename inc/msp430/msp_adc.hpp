#ifndef _MSP_SPI_HPP
#define _MSP_SPI_HPP
#include "mcu_spi.hpp"
#include "utilities/utilities.hpp"
#include <stdint.h>
#include <msp430.h>
#include "msp_sys.hpp"
#include "msp_gpio.hpp"
#include "utilities/circfifo.hpp"
#include <math.h>
#include "msp430/msp_periph.hpp"


namespace McuPeripheral
{

enum class AdcConversionMode : uint16_t
{
	ADC_SINGLE = CONSEQ_0,
	ADC_SEQUENCE = CONSEQ_1,
	ADC_REPEAT_SINGLE = CONSEQ_2,
	ADC_REPEAT_SEQUENCE = CONSEQ_3
};

enum class AdcInputChannel : uint16_t
{
	ADC_CHANNEL_A0 = INCH_0,
	ADC_CHANNEL_A1 = INCH_1,
	ADC_CHANNEL_A2 = INCH_2,
	ADC_CHANNEL_A3 = INCH_3,
	ADC_CHANNEL_A4 = INCH_4,
	ADC_CHANNEL_A5 = INCH_5,
	ADC_CHANNEL_A6 = INCH_6,
	ADC_CHANNEL_A7 = INCH_7,
	ADC_CHANNEL_VREF = INCH_8,
	ADC_CHANNEL_mVREF = INCH_9,
	ADC_CHANNEL_TSENSOR = INCH_10,
	/*Other (Vcc-Vss)/2 inputs after this not sure what they accomplish*/
	ADC_CHANNEL_INCH11 = INCH_11,
	ADC_CHANNEL_INCH12 = INCH_12,
	ADC_CHANNEL_INCH13 = INCH_13,
	ADC_CHANNEL_INCH14 = INCH_14,
	ADC_CHANNEL_INCH15 = INCH_15
};


enum class AdcReference : uint16_t
{
	ADC_REF0 = SREF_0, /*Vcc to Vss*/
	ADC_REF1 = SREF_1, /*Vref+ to Vss*/
	ADC_REF2 = SREF_2, /*22.3.1 Ref Man*/
	ADC_REF3 = SREF_3,
	ADC_REF4 = SREF_4,
	ADC_REF5 = SREF_5,
	ADC_REF6 = SREF_6,
	ADC_REF7 = SREF_7
};

enum class AdcSHT : uint16_t
{
	ADC_SHT_4 = ADC10SHT_0,
	ADC_SHT_8 = ADC10SHT_1,
	ADC_SHT_16 = ADC10SHT_2,
	ADC_SHT_64 = ADC10SHT_3
};

enum class AdcSampleSource : uint16_t
{
	ADC_Source_StartBit = SHS_0,
	ADC_Source_TAOut1 = SHS_1,
	ADC_Source_TAOut0 = SHS_2,
	ADC_Source_TAOut2 = SHS_3
};

template< uint16_t _dtc0, uint16_t _ctl0, uint16_t _sa, bool _enableInt=false>
class AdcControl
{
public:



	static uint16_t readMemory()
	{
		uint16_t ret = REG_16(mem) & 0x3FF;
		return ret;
	}

	static bool adcBusy()
	{
		return ((REG_16(ctl1) & ADC10BUSY) > 0);
	}

	static void adcConversion(AdcConversionMode mode)
	{
		REG_16(ctl1) &= ~CONSEQ_3;
		REG_16(ctl1) |= mode;
	}

	static void enableConversion()
	{
		REG_16(ctl0) |= ENC;
	}

	static void disableConversion()
	{
		REG_16(ctl0) &= ~ENC;
	}

	static void startSample(AdcInputChannel channel)
	{
		REG_16(ctl1) &= ~static_cast<uint16_t>(AdcInputChannel::ADC_CHANNEL_INCH15);
		REG_16(ctl1) |= static_cast<uint16_t>(channel);
		REG_16(ctl0) |= ENC | ADC10SC;
	}

	static void enableReferenceOut()
	{
		REG_16(ctl0) |= REFOUT;
	}

	static void disableReferenceOut()
	{
		REG_16(ctl0) &= ~REFOUT;
	}

	static void enableReferenceBurst()
	{
		REG_16(ctl0) |= REFBURST;
	}

	static void disableReferenceBurst()
	{
		REG_16(ctl0) &= ~REFBURST;
	}

	static void referenceSelect(AdcReference ref)
	{
		REG_16(ctl0) &= AdcReference::ADC_REF7;
		REG_16(ctl0) |= ref;
	}

	static void sampleAndHoldClocks( AdcSHT sht )
	{
		REG_16(ctl0) &= AdcSHT::ADC_SHT_64;
		REG_16(ctl0) |= sht;
	}

	static void enableMultipleSampleConversion( )
	{
		REG_16(ctl0) |= MSC;
	}

	static void disableMultipleSampleConversion( )
	{
		REG_16(ctl0) &= ~MSC;
	}

	static bool flagSet()
	{
		return (REG_16(ctl0) & ADC10IFG) > 0;
	}

	static void enableIrq()
	{
		REG_16(ctl0) |= ADC10IE;
	}

	static void disableIrq()
	{
		REG_16(ctl0) &= ~ADC10IE;
	}

	static void init(uint8_t inputs, uint16_t ref)
	{

		REG_16(ae0) = inputs;
		REG_16(ctl1) = 0;
		REG_16(ctl1) = ADC10SSEL_2 | ADC10DIV_7;
		REG_16(ctl0) = static_cast<uint16_t>(AdcReference::ADC_REF1) | static_cast<uint16_t>(AdcSHT::ADC_SHT_64) | ADC10ON | REFON;



		if(_enableInt == true) {
			enableIrq();
		}
	}

private:

	static constexpr uint16_t ctl0 = _ctl0;
	static constexpr uint16_t ctl1 = ctl0 + 2;
	static constexpr uint16_t mem = ctl1 + 2;
	static constexpr uint16_t tc0 = _dtc0;
	static constexpr uint16_t tc1 = tc0 +1;
	static constexpr uint16_t ae0 = tc1 +1;
	static constexpr uint16_t ae1 = ae0 +1;
	static constexpr uint16_t sa = _sa;

};

//How to handle sampling as well.
template<class _adc, bool _ref2_5, bool _tsensor, class _a0pin=FakePin, class _a1pin=FakePin, class _a2pin=FakePin, class _a3pin=FakePin, class _a4pin=FakePin, class _a5pin=FakePin, class _a6pin=FakePin, class _a7pin=FakePin>
class McuAdc
{
public:
	static constexpr uint16_t REFERENCE = (uint16_t)_ref2_5 << 6;
	static constexpr uint8_t INPUTS = _a0pin::getBitValue() | _a1pin::getBitValue() | _a2pin::getBitValue() | _a3pin::getBitValue() | _a4pin::getBitValue() | _a5pin::getBitValue() | _a6pin::getBitValue() | _a7pin::getBitValue();

	//templated
	static void init() {
		_adc::init(INPUTS, REFERENCE);
		//Do a loop through of the
	}

	static void sampleSingleChannel(AdcInputChannel channel)
	{
		//do sampleonce either interrupt or blocking
		_adc::startSample(channel);
		while(_adc::adcBusy()) {}
		mCurrentAdcData = _adc::readMemory();
		_adc::disableConversion();

	}

	//returns in millivolts, realize this is not accurate due to 16 bit nature of the calculation
	//this could be off as much as ~40 mV
	static uint16_t getSingleChannelVoltage()
	{
		//
		return (mCurrentAdcData*15) / 10;
	}

	static uint16_t getSingleChannelRawData()
	{
		return mCurrentAdcData;
	}

	static uint16_t getTemperatureSensorInF()
	{
		if(_tsensor) {
			return convertTemperatureSensorToF();
		}
		else {
			return 0;
		}
	}




private:

	static uint16_t convertTemperatureSensorToF()
	{
		uint16_t ret = (mCurrentAdcData * 48724L - 30634388L) >> 16;
		return ret;
	}

	static uint16_t adcVoltage()
	{
		uint16_t ret = (mCurrentAdcData * 48724 - 30634388) >> 16;
		return ret;
	}

	//How to handle this for multiple adc channels?
	static uint16_t mCurrentAdcData;
};

template <class _adc, bool _ref2_5, bool _tsensor, class _a0pin, class _a1pin, class _a2pin, class _a3pin, class _a4pin, class _a5pin, class _a6pin, class _a7pin >
uint16_t McuAdc<_adc, _ref2_5, _tsensor, _a0pin,_a1pin, _a2pin,_a3pin,_a4pin, _a5pin, _a6pin,_a7pin>::mCurrentAdcData;


}


/*Gpio Inputs for Adc*/
using adca0input =  McuPeripheral::McuPin<McuPort1,BIT0>;
using adca1input =  McuPeripheral::McuPin<McuPort1,BIT1>;
using adca2input =  McuPeripheral::McuPin<McuPort1,BIT2>;
using adca3input =  McuPeripheral::McuPin<McuPort1,BIT3>;
using adca4input =  McuPeripheral::McuPin<McuPort1,BIT4>;
using adca5input =  McuPeripheral::McuPin<McuPort1,BIT5>;
using adca6input =  McuPeripheral::McuPin<McuPort1,BIT6>;
using adca7input =  McuPeripheral::McuPin<McuPort1,BIT7>;




using AdcControlInterrupts = McuPeripheral::AdcControl< ADC10DTC0_, ADC10CTL0_, ADC10SA_, true>;
using AdcControlBlocking = McuPeripheral::AdcControl< ADC10DTC0_, ADC10CTL0_, ADC10SA_>;

#endif //_MSP_SPI_HPP
