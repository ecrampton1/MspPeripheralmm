#ifndef _MCU_CONFIG_HPP_
#define _MCU_CONFIG_HPP_


#include "msp430/msp_sys.hpp"
#include "msp430/msp_gpio.hpp"
#include "msp430/msp_uart.hpp"
#include "msp430/msp_timer.hpp"
#include "msp430/msp_periph.hpp"
#include "dht/dht11.hpp"

namespace Periph = McuPeripheral;

#define  uart_rx BIT1
#define  uart_tx BIT2
constexpr Speed clock_speed = Speed::SPEED_16MHZ;

using led0 =  Periph::McuPin<McuPort1,BIT0>;
using led1 =  Periph::McuPin<McuPort1,BIT6>;
using dhtDataPin = Periph::McuPin<McuPort2,BIT3>;

using uart =  Periph::McuUart<UartA0, Periph::BaudRate::BAUD_115200, clock_speed>;
using sys =  Periph::McuSystem<clock_speed>;

using msp430Timer = McuPeripheral::McuTimer< Timer_Source::SMCLK, Timer0, 19000 , (int32_t) clock_speed >;

//using mcuTimer1 = Periph::McuTimer< Timer_Source::ACLK, Timer1, 4000000 , (int32_t) clock_speed >;
//using timer0Output = Periph::McuTimerOutput< Timer_Source::SMCLK, Timer0_A0, smclk_out , (int32_t) clock_speed >;


//If you want to enable smaller size try turning off isr's you are not using




#endif //_MCU_CONFIG_HPP_
