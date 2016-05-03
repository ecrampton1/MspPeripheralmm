#include "mcu_config.hpp"

class Calibration
{
public:
	struct CalConstants {
		uint8_t dcoctl;
		uint8_t bcsctl1;
	};

	static int start()
	{
		//Output the smclk
		smclk_out::selectOn();
		smclk_out::select2Off();
		smclk_out::output();
		//led0::selectOn();
		//led0::select2_off();
		//led0::output();

		for(int i = 0; i < 4; i++) {
			startCapture();
			updateClockValues(mExpectedValues[i]);
			mCalConstants[i].bcsctl1 = BCSCTL1;
			mCalConstants[i].dcoctl = DCOCTL;
			stopCapture();
		}

		//writeNewClockValues();

		Timer0::stop_timer();
		return 0;
	}

	static const CalConstants& getConstants(int index)
	{
		if(index < 0 || index > sizeof(mCalConstants))
			index = 0;

		return mCalConstants[index];
	}
private:


	static void writeNewClockValues()
	{
		//TODO create flash writing class
		char* Flash_ptrA = (char *)0x10C0;              // Point to beginning of seg A
		FCTL2 = FWKEY + FSSEL0 + FN1;             // MCLK/3 for Flash Timing Generator
		FCTL1 = FWKEY + ERASE;                    // Set Erase bit
		FCTL3 = FWKEY + LOCKA;                    // Clear LOCK & LOCKA bits
		*Flash_ptrA = 0x00;                       // Dummy write to erase Flash seg A
		FCTL1 = FWKEY + WRT;                      // Set WRT bit for write operation
		Flash_ptrA = (char *)0x10F8;              // Point to beginning of cal consts
		for (int j = 0; j < 4; j++) {
			*Flash_ptrA++ = mCalConstants[j].dcoctl;            // re-flash DCO calibration data
			*Flash_ptrA++ = mCalConstants[j].bcsctl1;            // re-flash DCO calibration data
		}
		FCTL1 = FWKEY;                            // Clear WRT bit
		FCTL3 = FWKEY + LOCKA + LOCK;             // Set LOCK & LOCKA bit
	}

	static void updateClockValues(uint16_t expected)
	{
		uint16_t capture, diff, capture_old = 0;
		while(1) {
			capture = getCaptureValue();
			diff = capture - capture_old;
			capture_old = capture;

			if(expected == diff) {
				return;
			}
			else if(expected < diff) {
				--DCOCTL;
				if(DCOCTL == 0xFF)
					if (BCSCTL1 & 0x0f)
						--BCSCTL1;
			}
			else
			{
				++DCOCTL;
				if(DCOCTL == 0x00)
					if ((BCSCTL1 & 0x0f) != 0x0f)
						++BCSCTL1;
			}
		}
	}

	static uint16_t getCaptureValue()
	{
		//start timer and get capture
		while(!Timer0::CapCompControl0::read_capture_flag()){}
		return Timer0::CapCompControl0::get_cap_comp_value();
	}

	static void startCapture()
	{
		BCSCTL1 |= DIVA_3;                        // Select Aclk with divide by 8
		//Set up the output in
		//Setup to capture aclk using smclk as source
		Timer0::CapCompControl0::start_capture(McuPeripheral::CapCompSelect::CCIB);
		Timer0::clear_timer();
		Timer0::start_timer(Timer_Mode::CONT_MODE,Timer_Source::SMCLK);

	}

	static void stopCapture()
	{
		Timer0::CapCompControl0::stop_capture();
		Timer0::stop_timer();
	}

	static CalConstants mCalConstants[4];
	static constexpr uint16_t EXPECTED_16MHZ = 3906;
	static constexpr uint16_t EXPECTED_12MHZ = 2930;
	static constexpr uint16_t EXPECTED_8MHZ = 1953;
	static constexpr uint16_t EXPECTED_1MHZ = 244;

	static uint16_t mExpectedValues[4];


};

uint16_t Calibration::mExpectedValues[4] = { EXPECTED_1MHZ,EXPECTED_8MHZ,EXPECTED_12MHZ,EXPECTED_16MHZ };
Calibration::CalConstants Calibration::mCalConstants[4];
