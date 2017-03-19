#ifndef _MCU_CONFIG_HPP_
#define _MCU_CONFIG_HPP_


#include "msp430/msp_sys.hpp"
#include "msp430/msp_gpio.hpp"
#include "msp430/msp_uart.hpp"
#include "msp430/msp_timer.hpp"
#include "msp430/msp_spi.hpp"
#include "nrf24/nrf24.hpp"
#include "relay_control.hpp"

namespace Periph = McuPeripheral;

constexpr Speed clock_speed = Speed::SPEED_16MHZ;

using led0 =  Periph::McuPin<McuPort1,BIT0>;

//define sensors to use NOTE do no use the same number twice
#define SENSOR_SWITCH0 0
#define SENSOR_TEMP0   1
#define SENSOR_HUM0    2
#define SENSOR_SWITCH1 3

//nrf24l01 pins
using nrf_ce = Periph::McuPin<McuPort2,BIT0>;
using nrf_csn = Periph::McuPin<McuPort2,BIT1>;
using nrf_irq = Periph::McuPin<McuPort2,BIT2>;
using fake_pin = Periph::FakePin;

//dht data pin
using dht_data_pin = Periph::McuPin<McuPort2,BIT3>;

//relay pins
using relay0_pin = Periph::McuPin<McuPort2, BIT4>;
using relay1_pin = Periph::McuPin<McuPort2, BIT5>;

//relay class
using relay0 = RelayControl<true,relay0_pin>;
using relay1 = RelayControl<true,relay1_pin>;

//push button switches
using sw0_pin = Periph::McuPin<McuPort1, BIT3>;
using sw1_pin = Periph::McuPin<McuPort1, BIT4>;

using uart =  Periph::McuUart<UartA0, Periph::BaudRate::BAUD_115200, clock_speed>;
using sys =  Periph::McuSystem<clock_speed>;
using spi = Periph::McuSpi<SpiB0,Periph::SpiClock::SPI_EDGE2_LOW,Periph::SpiBitOrder::SPI_MSB,2 >;
using msp430_timer = McuPeripheral::McuTimer< Timer_Source::SMCLK, Timer0, 19000 , (int32_t) clock_speed >;

//If you want to enable smaller size try turning off isr's you are not using
#define PRINT(...)

#endif //_MCU_CONFIG_HPP_
