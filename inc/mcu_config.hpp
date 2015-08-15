#ifndef _MCU_CONFIG_HPP_
#define _MCU_CONFIG_HPP_

#ifdef MSP430_BUILD
#include "msp430/msp_sys.hpp"
#include "msp430/msp_gpio.hpp"
#include "msp430/msp_uart.hpp"

namespace Periph = McuPeripheral;

#define  uart_rx BIT1
#define  uart_tx BIT2

using led0 =  Periph::McuPin<McuPort1,BIT0>;
using led1 =  Periph::McuPin<McuPort1,BIT6>;
using uart =  Periph::McuUart<Periph::BaudRate::BAUD_115200,Speed::SPEED_16MHZ, uart_rx, uart_tx>;
using sys =  Periph::McuSystem<Speed::SPEED_16MHZ>;

#endif

#endif //_MCU_CONFIG_HPP_
