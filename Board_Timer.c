/*-----------------------------------------------------------------------------
 * Name:    LED_MCB11C14.c
 * Purpose: LED interface for MCB11C14 evaluation board
 * Rev.:    1.00
 *----------------------------------------------------------------------------*/

/* Copyright (c) 2015 ARM LIMITED

   All rights reserved.
   Redistribution and use in source and binary forms, with or without
   modification, are permitted provided that the following conditions are met:
   - Redistributions of source code must retain the above copyright
     notice, this list of conditions and the following disclaimer.
   - Redistributions in binary form must reproduce the above copyright
     notice, this list of conditions and the following disclaimer in the
     documentation and/or other materials provided with the distribution.
   - Neither the name of ARM nor the names of its contributors may be used
     to endorse or promote products derived from this software without
     specific prior written permission.
   *
   THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
   AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
   IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
   ARE DISCLAIMED. IN NO EVENT SHALL COPYRIGHT HOLDERS AND CONTRIBUTORS BE
   LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
   CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
   SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
   INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
   CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
   ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
   POSSIBILITY OF SUCH DAMAGE.
   ---------------------------------------------------------------------------*/

#include "LPC11xx.h"                    // Device header
#include "Board_Timer.h"                  // ::Board Support:Timer



/* CLKOUT  PIO0_1 */

/**
  \fn          int32_t LED_Initialize (void)
  \brief       Initialize LEDs
  \returns

*/
void delay_sec   (uint32_t sec) {
  LPC_SYSCON->SYSAHBCLKCTRL |= (1UL <<  9);     /* enable clock for Timer        */
	

	LPC_TMR32B0->TCR = 0x02UL;													/*Reset Timer*/
	LPC_TMR32B0->PR  = 0;											        /*Clear Prescaler*/
	LPC_TMR32B0->MR0 = sec*1000;
	LPC_TMR32B0->IR  = 0xFFUL;												/*Reset all interrupts*/
	LPC_TMR32B0->MCR = 0x04UL;                        /*stop on MR0*/
	LPC_TMR32B0->PR   =12000-1;
	LPC_TMR32B0->TCR = 0x01;													/*Start Timer*/
	
	 while (LPC_TMR32B0->TCR & 0x01); // Wait until delay time has elapsed

}


void delay_ms   (uint32_t ms) {
  LPC_SYSCON->SYSAHBCLKCTRL |= (1UL <<  9);     /* enable clock for Timer        */
	

	LPC_TMR32B0->TCR = 0x02UL;													/*Reset Timer*/
	LPC_TMR32B0->PR  = 0;											        /*Clear Prescaler*/
	LPC_TMR32B0->MR0 = ms;
	LPC_TMR32B0->IR  = 0xFFUL;												/*Reset all interrupts*/
	LPC_TMR32B0->MCR = 0x04UL;                        /*stop on MR0*/
	LPC_TMR32B0->PR   =12000-1;
	LPC_TMR32B0->TCR = 0x01;													/*Start Timer*/
	
	 while (LPC_TMR32B0->TCR & 0x01); // Wait until delay time has elapsed

}



void delay_micro   (uint32_t micro) {
  LPC_SYSCON->SYSAHBCLKCTRL |= (1UL <<  7);     /* enable clock for Timer        */
	

	LPC_TMR16B0->TCR = 0x02UL;													/*Reset Timer*/
	LPC_TMR16B0->PR  = 0;											        /*Clear Prescaler*/
	LPC_TMR16B0->MR0 = micro;
	LPC_TMR16B0->IR  = 0xFFUL;												/*Reset all interrupts*/
	LPC_TMR16B0->MCR = 0x04UL;                        /*stop on MR0*/
	LPC_TMR16B0->PR   =12-1;
	LPC_TMR16B0->TCR = 0x01;													/*Start Timer*/
	
	 while (LPC_TMR16B0->TCR & 0x01); // Wait until delay time has elapsed

}
