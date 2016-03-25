#ifndef _MCU_CONFIG_HPP_
#define _MCU_CONFIG_HPP_


#include "msp430/msp_sys.hpp"
#include "msp430/msp_gpio.hpp"
#include "msp430/msp_uart.hpp"
#include "msp430/msp_timer.hpp"

namespace Periph = McuPeripheral;

#define  uart_rx BIT1
#define  uart_tx BIT2

constexpr Speed clock_speed = Speed::SPEED_1MHZ;

using led0 =  Periph::McuPin<McuPort1,BIT0>;
using led1 =  Periph::McuPin<McuPort1,BIT6>;
using button = Periph::McuPin<McuPort1,BIT3>;

using smclk_out =  Periph::McuPin<McuPort1,BIT4>;

using uart =  Periph::McuUart<UartA0, Periph::BaudRate::BAUD_115200, clock_speed>;
using sys =  Periph::McuSystem<clock_speed>;
//using mcuTimer0 = Periph::McuTimer< Timer_Source::SMCLK, Timer0, 1000000 , (int32_t) clock_speed >;
//using mcuTimer1 = Periph::McuTimer< Timer_Source::ACLK, Timer1, 4000000 , (int32_t) clock_speed >;
//using timer0Output = Periph::McuTimerOutput< Timer_Source::SMCLK, Timer0_A0, smclk_out , (int32_t) clock_speed >;

//If you want to enable smaller size try turning off isr's you are not using




#endif //_MCU_CONFIG_HPP_
