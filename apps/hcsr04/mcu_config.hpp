#ifndef _MCU_CONFIG_HPP_
#define _MCU_CONFIG_HPP_


#include "msp430/msp_sys.hpp"
#include "msp430/msp_gpio.hpp"
#include "msp430/msp_uart.hpp"
#include "msp430/msp_timer.hpp"

namespace Periph = McuPeripheral;

#define  uart_rx BIT1
#define  uart_tx BIT2

constexpr Speed clock_speed = Speed::SPEED_16MHZ;

using echo_pin =  Periph::McuPin<McuPort2,BIT1>;
using trig_pin =  Periph::McuPin<McuPort2,BIT2>;


using uart =  Periph::McuUart<UartA0, Periph::BaudRate::BAUD_115200, clock_speed>;
using sys =  Periph::McuSystem<clock_speed>;
using timerConfig0 = Periph::TimerConfig< Timer_Source::SMCLK, (uint32_t) clock_speed, Periph::ClockDivider::DIVIDE_8 >;
using continuousTimer = Periph::ContinuousTimer<Timer0,timerConfig0>;
//using mcuTimer1 = Periph::McuTimer< Timer_Source::ACLK, Timer1, 4000000 , (int32_t) clock_speed >;
//using timer0Output = Periph::McuTimerOutput< Timer_Source::SMCLK, Timer0_A0, smclk_out , (int32_t) clock_speed >;

//If you want to enable smaller size try turning off isr's you are not using




#endif //_MCU_CONFIG_HPP_