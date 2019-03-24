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

namespace Periph = McuPeripheral;

constexpr Speed clock_speed = Speed::SPEED_16MHZ;

using led0 =  Periph::McuPin<McuPort1,BIT0>;

//nrf24l01 pins
using rfm_nss = Periph::McuPin<McuPort2,BIT0>;
using rfm_irq = Periph::McuPin<McuPort1,BIT4>;
using fake_pin = Periph::FakePin;


using uart =  Periph::McuUart<UartA0, Periph::BaudRate::BAUD_115200, clock_speed>;
using sys =  Periph::McuSystem<clock_speed>;
using spi = Periph::McuSpi<SpiB0,Periph::SpiClock::SPI_EDGE2_LOW,Periph::SpiBitOrder::SPI_MSB,4 >;
using RF = Rfm69<spi, sys, rfm_nss,rfm_irq, CarrierFrequency::FREQUENCY_915, uart>;




//If you want to enable smaller size try turning off isr's you are not using
//#define PRINT(...)

#endif //_MCU_CONFIG_HPP_
