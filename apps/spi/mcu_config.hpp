#ifndef _MCU_CONFIG_HPP_
#define _MCU_CONFIG_HPP_


#include "msp430/msp_sys.hpp"
#include "msp430/msp_gpio.hpp"
#include "msp430/msp_uart.hpp"
#include "msp430/msp_timer.hpp"
#include "msp430/msp_spi.hpp"

namespace Periph = McuPeripheral;

constexpr Speed clock_speed = Speed::SPEED_16MHZ;

using led0 =  Periph::McuPin<McuPort1,BIT0>;

//nrf24l01 pins
using nrf_ce = Periph::McuPin<McuPort2,BIT0>;
using nrf_csn = Periph::McuPin<McuPort2,BIT1>;
using nrf_irq = Periph::McuPin<McuPort2,BIT2>;


using uart =  Periph::McuUart<UartA0, Periph::BaudRate::BAUD_115200, clock_speed>;
using sys =  Periph::McuSystem<clock_speed>;
using spi = Periph::McuSpi<SpiB0,Periph::SpiClock::SPI_EDGE2_LOW,Periph::SpiMaster::SPI_MASTER,Periph::SpiBitOrder::SPI_MSB,2,false>;
//using mcuTimer0 = Periph::McuTimer< Timer_Source::SMCLK, Timer0, 1000000 , (int32_t) clock_speed >;
//using mcuTimer1 = Periph::McuTimer< Timer_Source::ACLK, Timer1, 4000000 , (int32_t) clock_speed >;
//using timer0Output = Periph::McuTimerOutput< Timer_Source::SMCLK, Timer0_A0, smclk_out , (int32_t) clock_speed >;

//If you want to enable smaller size try turning off isr's you are not using


#endif //_MCU_CONFIG_HPP_
