#ifndef _MCU_CONFIG_HPP_
#define _MCU_CONFIG_HPP_

#ifdef MSP430_BUILD
#include "msp430/msp_sys.hpp"
#include "msp430/msp_gpio.hpp"
#include "msp430/msp_uart.hpp"

namespace Periph = McuPeripheral;

#define  uart_rx BIT1
#define  uart_tx BIT2

typedef Periph::McuPin<McuPort1,BIT0> led0;
typedef Periph::McuPin<McuPort1,BIT6> led1;
typedef Periph::McuUart<Periph::BaudRate::BAUD_115200,Speed::SPEED_16MHZ, uart_rx, uart_tx> uart;
typedef Periph::McuSystem<Speed::SPEED_16MHZ> sys;

#endif

#endif //_MCU_CONFIG_HPP_
