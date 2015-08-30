#include "mcu_config.hpp"
#include <msp430.h>


extern "C" {
#include <stdlib.h>
}


/*static volatile bool timerElapsed = false;
void periodicPrint(void* args)
{
	timerElapsed = true;

}*/

constexpr uint16_t EXPECTED = 1953;


//template<TimerSource _source, class _timer, class _pin, int32_t _clock >
class Calibration
{
public:
	static int start()
	{
		//Output the smclk
		smclk_out::select_on();
		smclk_out::select2_off();
		smclk_out::output();

		/*//check if timer be running
		if(!Timer0_A0::is_running()) {
			_DINT();
			//Set up the output in
			smclk_out::select_on();
			smclk_out::select2_off();
			smclk_out::output();
			Timer0_A0::clear_timer();
			Timer0_A0::start_timer(Timer_Mode::CONT_MODE,Timer_Source::SMCLK);
			_EINT();
			return 0;
		}*/

		startCapture();

		while(1) {
			uint16_t value = getCaptureValue();
			uart::send((int64_t) value );
			uart::sendLine();
		}
		Timer0::stop_timer();
		return -1;
	}
private:
	struct CalConstants {
		uint8_t dcoctl;
		uint8_t bcsctl1;
	};

	static uint16_t getCaptureValue()
	{
		//start timer and get capture
		while(!Timer0::CapCompControl0::read_capture_flag())
		{
			__delay_cycles(10000);
			//uart::send(REG_16(TACCTL0_), McuPeripheral::Base::BASE_HEX);
			//uart::sendLine();
		}
		return Timer0::CapCompControl0::get_cap_comp_value();
	}

	static void startCapture()
	{
		BCSCTL1 |= DIVA_3;                        // Select Aclk with divide by 8
		//Set up the output in
		//Setup to capture aclk using smclk as source
		Timer0::CapCompControl0::start_capture(McuPeripheral::CapCompSelect::CCIB);
		uart::send(REG_16(TACCTL0_), McuPeripheral::Base::BASE_HEX);
		uart::sendLine();
		Timer0::start_timer(Timer_Mode::CONT_MODE,Timer_Source::SMCLK);
		uart::send(REG_16(TACTL_), McuPeripheral::Base::BASE_HEX);
		uart::sendLine();
		Timer0::clear_timer();
	}

	static void stopCapture()
	{
		Timer0::CapCompControl0::stop_capture();
		Timer0::stop_timer();
	}

	CalConstants mCalConstants[4];


};



int main()
{
	sys::init();
	uart::init();
	uart::send("Starting DCO calibration!\n");
	led0::output();
	Calibration::start();




	//Start with SMCLK
	//Timer0_A0::clear_timer();
	//Timer0_A0::start_timer(Timer_Mode::CONT_MODE,Timer_Source::SMCLK);

	//Start with ACLK
	//Timer1_A0::clear_timer();
	//Timer1_A0::start_timer(Timer_Mode::CONT_MODE,Timer_Source::ACLK);

	//int ret = timer0Output::start();
	/*if(ret <0) {
		uart::sendLine("Failed to start");
	}
	uint16_t start = 0;
	uint16_t stop  = 0;*/
	while(1) {
		led0::toggle();
		/*while(Timer1_A0::count_value() != 0);
		start = Timer0_A0::count_value();

		while(Timer1_A0::count_value() < 4);
		stop = Timer0_A0::count_value();

		uart::send((int64_t)start);
		uart::sendLine();
		uart::send((int64_t)stop);
		uart::sendLine();
		uart::send((int64_t)(stop - start));
		uart::sendLine();*/

	}

	return 1;
}



