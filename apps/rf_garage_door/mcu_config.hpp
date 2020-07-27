#ifndef _MCU_CONFIG_HPP_
#define _MCU_CONFIG_HPP_


//TODO make one simple include file with all the msp430 headers in it.
#include "msp430/msp_sys.hpp"
#include "msp430/msp_gpio.hpp"
#include "msp430/msp_uart.hpp"
#include "msp430/msp_timer.hpp"
#include "msp430/msp_spi.hpp"
//Libraries
#include "rfm69/rfm69.hpp"
#include "rfm69_message_handler.hpp"
#include "rf_hcsr04.hpp"
#include "rf_relay_control.hpp"
#include "relay_control.hpp"

namespace Periph = McuPeripheral;

constexpr Speed clock_speed = Speed::SPEED_16MHZ;

using echo_pin =  Periph::McuPin<McuPort2,BIT1>;
using trig_pin =  Periph::McuPin<McuPort2,BIT2>;
using relay_pin =  Periph::McuPin<McuPort1,BIT0>;

//nrf24l01 pins
using rfm_nss = Periph::McuPin<McuPort2,BIT0>;
using rfm_irq = Periph::McuPin<McuPort1,BIT4>;
using fake_pin = Periph::FakePin;


using uart =  Periph::McuUart<UartA0, Periph::BaudRate::BAUD_115200, clock_speed>;
using sys =  Periph::McuSystem<clock_speed>;
using spi = Periph::McuSpi<SpiB0,Periph::SpiClock::SPI_EDGE2_LOW,Periph::SpiBitOrder::SPI_MSB,4 >;
using RF = Rfm69<spi, sys, rfm_nss,rfm_irq, CarrierFrequency::FREQUENCY_915, uart>;
using Handler = PeripheralMessages::RFM69Handler;

//HCSR04 configuration move into rf hcsr04?
using timerConfig1 = Periph::TimerConfig< Timer_Source::SMCLK, (uint32_t) clock_speed, Periph::ClockDivider::DIVIDE_8 >;
using continuousTimer = Periph::ContinuousTimer<Timer1,timerConfig1>;
using pulseWidthMeasure = Periph::PulseWidthMeasure<Timer1,timerConfig1, Timer1::CapCompControl1, echo_pin,TA1CCR1_>;
using rfHcsr04 = RfHcsr04<pulseWidthMeasure,sys,trig_pin,Handler>;

//Rf Relay

using relay = RelayControl<true, relay_pin, sys >;
using rfRelay = RfRelayControl<relay,sys,Handler,true>;


//If you want to enable smaller size try turning off isr's you are not using
//#define PRINT(...)

#endif //_MCU_CONFIG_HPP_
