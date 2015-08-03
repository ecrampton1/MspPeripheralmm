#ifndef _MSP_SYS_HPP
#define _MSP_SYS_HPP

#include "mcu_sys.hpp"


namespace McuPeripheral {


template<McuSpeed M>
class McuSystem : public SystemBase
{
public:
	static void init();

#define DELTA_16MHZ   3906                  // 2930 x 4096Hz = 16.00MHz
	//static uint32_t getSpeed()
	/*static void Set_DCO(unsigned int Delta) {      // Set DCO to selected frequency
	    unsigned int Compare, Oldcapture = 0;

	    BCSCTL1 |= DIVA_3;                  // ACLK = LFXT1CLK/8
	    TACCTL0 = CM_1 + CCIS_1 + CAP;      // CAP, ACLK
	    TACTL = TASSEL_2 + MC_2 + TACLR;    // SMCLK, cont-mode, clear

	    while (1) {
	        while (!(CCIFG & TACCTL0));     // Wait until capture occured
	        TACCTL0 &= ~CCIFG;              // Capture occured, clear flag
	        Compare = TACCR0;               // Get current captured SMCLK
	        Compare = Compare - Oldcapture; // SMCLK difference
	        Oldcapture = TACCR0;            // Save current captured SMCLK

	        if (Delta == Compare)
	            break;                      // If equal, leave "while(1)"
	        else if (Delta < Compare) {
	            DCOCTL--;                   // DCO is too fast, slow it down
	            if (DCOCTL == 0xFF)         // Did DCO roll under?
	                if (BCSCTL1 & 0x0f)
	                    BCSCTL1--;          // Select lower RSEL
	        }
	        else {
	            DCOCTL++;                   // DCO is too slow, speed it up
	            if (DCOCTL == 0x00)         // Did DCO roll over?
	                if ((BCSCTL1 & 0x0f) != 0x0f)
	                    BCSCTL1++;          // Sel higher RSEL
	        }
	    }
	    TACCTL0 = 0;                        // Stop TACCR0
	    TACTL = 0;                          // Stop Timer_A
	    BCSCTL1 &= ~DIVA_3;                 // ACLK = LFXT1CLK
	} */// Set_DCO

};

}


typedef McuPeripheral::McuSpeed Speed;
#endif //_MSP_SYS_HPP
