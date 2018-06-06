/* nRF24L01.h
 * Register definitions for manipulating the Nordic Semiconductor
 * nRF24L01+ RF transceiver chipsets.
 *
    Copyright (c) 2007 Stefan Engelke <mbox@stefanengelke.de>
    Some parts copyright (c) 2012 Eric Brundick <spirilis [at] linux dot com>
    Modified: Ed Crampton for use with c++11.
    Permission is hereby granted, free of charge, to any person
    obtaining a copy of this software and associated documentation
    files (the "Software"), to deal in the Software without
    restriction, including without limitation the rights to use, copy,
    modify, merge, publish, distribute, sublicense, and/or sell copies
    of the Software, and to permit persons to whom the Software is
    furnished to do so, subject to the following conditions:
    The above copyright notice and this permission notice shall be
    included in all copies or substantial portions of the Software.
    THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
    EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
    MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
    NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
    HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
    WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
    OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
    DEALINGS IN THE SOFTWARE.
 */
#ifndef _NRF24L01_H
#define _NRF24L01_H
#include <stdint.h>
#include <msp430.h>

//* Registers */
constexpr uint8_t RF24_CONFIG   =   0x00;
constexpr uint8_t  RF24_EN_AA      = 0x01;
constexpr uint8_t  RF24_EN_RXADDR  = 0x02;
constexpr uint8_t  RF24_SETUP_AW   = 0x03;
constexpr uint8_t  RF24_SETUP_RETR = 0x04;
constexpr uint8_t  RF24_RF_CH      = 0x05;
constexpr uint8_t  RF24_RF_SETUP   = 0x06;
constexpr uint8_t  RF24_STATUS     = 0x07;
constexpr uint8_t  RF24_OBSERVE_TX = 0x08;
constexpr uint8_t  RF24_CD         = 0x09;
constexpr uint8_t  RF24_RPD        = 0x09;
constexpr uint8_t  RF24_RX_ADDR_P0 = 0x0A;
constexpr uint8_t  RF24_RX_ADDR_P1 = 0x0B;
constexpr uint8_t  RF24_RX_ADDR_P2 = 0x0C;
constexpr uint8_t  RF24_RX_ADDR_P3 = 0x0D;
constexpr uint8_t  RF24_RX_ADDR_P4 = 0x0E;
constexpr uint8_t  RF24_RX_ADDR_P5 = 0x0F;
constexpr uint8_t  RF24_TX_ADDR    = 0x10;
constexpr uint8_t  RF24_RX_PW_P0   = 0x11;
constexpr uint8_t  RF24_RX_PW_P1   = 0x12;
constexpr uint8_t  RF24_RX_PW_P2   = 0x13;
constexpr uint8_t  RF24_RX_PW_P3   = 0x14;
constexpr uint8_t  RF24_RX_PW_P4   = 0x15;
constexpr uint8_t  RF24_RX_PW_P5   = 0x16;
constexpr uint8_t  RF24_FIFO_STATUS = 0x17;
constexpr uint8_t  RF24_DYNPD       = 0x1C;
constexpr uint8_t  RF24_FEATURE    = 0x1D;

/* Register Bits */
constexpr uint8_t  RF24_MASK_RX_DR  = BIT6;
constexpr uint8_t  RF24_MASK_TX_DS = BIT5;
constexpr uint8_t  RF24_MASK_MAX_RT = BIT4;
constexpr uint8_t  RF24_IRQ_MASK   = (RF24_MASK_RX_DR | RF24_MASK_TX_DS | RF24_MASK_MAX_RT);
constexpr uint8_t  RF24_EN_CRC     = BIT3;
constexpr uint8_t  RF24_CRCO       = BIT2;
constexpr uint8_t  RF24_PWR_UP     = BIT1;
constexpr uint8_t  RF24_PRIM_RX    = BIT0;
constexpr uint8_t  RF24_ENAA_P5    = BIT5;
constexpr uint8_t  RF24_ENAA_P4    = BIT4;
constexpr uint8_t  RF24_ENAA_P3    = BIT3;
constexpr uint8_t  RF24_ENAA_P2    = BIT2;
constexpr uint8_t  RF24_ENAA_P1    = BIT1;
constexpr uint8_t  RF24_ENAA_P0    = BIT0;
constexpr uint8_t  RF24_ERX_P5     = BIT5;
constexpr uint8_t  RF24_ERX_P4     = BIT4;
constexpr uint8_t  RF24_ERX_P3     = BIT3;
constexpr uint8_t  RF24_ERX_P2     = BIT2;
constexpr uint8_t  RF24_ERX_P1     = BIT1;
constexpr uint8_t  RF24_ERX_P0     = BIT0;
constexpr uint8_t  RF24_AW         = BIT0;
constexpr uint8_t  RF24_ARD        = BIT4;
constexpr uint8_t  RF24_ARC        = BIT0;
constexpr uint8_t  RF24_MASK_RX_P  = (BIT1 | BIT2 | BIT3);
constexpr uint8_t  RF24_PLL_LOCK   = BIT4;
constexpr uint8_t  RF24_CONT_WAVE  = BIT7;
constexpr uint8_t  RF24_RF_DR      = BIT3;
constexpr uint8_t  RF24_RF_DR_LOW  = BIT5;
constexpr uint8_t  RF24_RF_DR_HIGH = BIT3;
constexpr uint8_t  RF24_RF_PWR     = BIT1;
constexpr uint8_t  RF24_LNA_HCURR  = BIT0;
constexpr uint8_t  RF24_RX_DR      = BIT6;
constexpr uint8_t  RF24_TX_DS      = BIT5;
constexpr uint8_t  RF24_MAX_RT     = BIT4;
constexpr uint8_t  RF24_RX_P_NO    = BIT1;
constexpr uint8_t  RF24_TX_FULL    = BIT0;
constexpr uint8_t  RF24_PLOS_CNT   = BIT4;
constexpr uint8_t  RF24_ARC_CNT    = BIT0;
constexpr uint8_t  RF24_TX_REUSE   = BIT6;
constexpr uint8_t  RF24_FIFO_FULL  = BIT5;
constexpr uint8_t  RF24_TX_EMPTY   = BIT4;
constexpr uint8_t  RF24_RX_FULL    = BIT1;
constexpr uint8_t  RF24_RX_EMPTY   = BIT0;
constexpr uint8_t  RF24_EN_DPL     = BIT2;
constexpr uint8_t  RF24_EN_ACK_PAY = BIT1;
constexpr uint8_t  RF24_EN_DYN_ACK = BIT0;

/* Instructions */
constexpr uint8_t  RF24_R_REGISTER   = 0x00;
constexpr uint8_t  RF24_W_REGISTER   = 0x20;
constexpr uint8_t  RF24_REGISTER_MASK = 0x1F;
constexpr uint8_t  RF24_R_RX_PAYLOAD = 0x61;
constexpr uint8_t  RF24_W_TX_PAYLOAD = 0xA0;
constexpr uint8_t  RF24_FLUSH_TX     = 0xE1;
constexpr uint8_t  RF24_FLUSH_RX     = 0xE2;
constexpr uint8_t  RF24_REUSE_TX_PL  = 0xE3;
constexpr uint8_t  RF24_R_RX_PL_WID  = 0x60;
constexpr uint8_t  RF24_W_ACK_PAYLOAD = 0xA8;
constexpr uint8_t  RF24_W_TX_PAYLOAD_NOACK = 0xB0;
constexpr uint8_t  RF24_NOP          = 0xFF;
constexpr uint8_t  RF24_ACTIVATE     = 0x50;

#endif
