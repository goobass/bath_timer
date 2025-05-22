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
#include "Board_CLKOUT.h"                  // ::Board Support:CLKOUT



	/* CLKOUT  PIO0_1 */

	/**
	  \fn          int32_t LED_Initialize (void)
	  \brief       Initialize LEDs
	  \returns

	*/
void CLK_Initialize(uint32_t mode) {

	LPC_SYSCON->SYSAHBCLKCTRL |= ((1UL << 6) |     /* enable clock for GPIO        */
		(1UL << 7) | 	  /* enable clock for CT16B0        */
		(1UL << 16));    /* enable clock for IOCON       */

	LPC_IOCON->PIO0_1 = 1UL;                    /* select GPIO function  CLKOUT       */
	LPC_SYSCON->CLKOUTDIV = 1;
	LPC_SYSCON->CLKOUTCLKSEL = mode;
	/*
	  LPC_GPIO0->DIR  |=  (1UL <<  7) ;
	  LPC_GPIO0->DATA &= ~0x000000FFUL;
	*/

}
