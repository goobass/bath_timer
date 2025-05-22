/*-----------------------------------------------------------------------------
 * Name:    Buttons_MCB11C14.c
 * Purpose: Buttons interface for MCB11C14 evaluation board
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
#include "Board_Remote.h"
#include "Board_Timer.h"                  // ::Board Support:Timer



int32_t remotesend(char* str) {
	uint16_t count = 0;
	uint16_t bit = 0;
	char data;

	LPC_SYSCON->SYSAHBCLKCTRL |= ((1UL << 6) |     /* enable clock for GPIO        */
		(1UL << 16));    /* enable clock for IOCON       */
	LPC_IOCON->PIO0_9 = ((0UL << 0) |            /* select GPIO function         */
		(3UL << 6));          /* keep reserved values         */
	LPC_GPIO0->DIR |= (1UL << 9);               /* configure PINs as Output     */
	LPC_GPIO0->DATA &= ~(1UL << 9);               /* switch LEDs off              */

while(1){

	delay_sec(2);
		
	 count = 0;

		start();
		while (1) {
			if (str[count] == 0x11) {
				break;
			}
			for (bit = 0; bit < 8; bit++) {
				data = str[count] & (1 << bit);


				if (data == 0) {
					data0();
				}
				else {
					data1();
				}
			}
			count++;
		}
			data0();	
			 count = 0;
			start2();	
		start();
		while (1) {
			if (str[count] == 0x11) {
				break;
			}
			for (bit = 0; bit < 8; bit++) {
				data = str[count] & (1 << bit);


				if (data == 0) {
					data0();
				}
				else {
					data1();
				}
			}
			count++;
		}
				
		
		
		
		data0();



	}

	return 0;
}

void on(void) {
	int n;
	int m;
	for (m = 0; m < 22; m++) {
		for (n = 0; n < 2; n++) {
			LPC_GPIO0->DATA |= (1UL << 9);               /* switch LEDs on             */
		}
		for (n = 0; n < 5; n++) {
			LPC_GPIO0->DATA &= ~(1UL << 9);               /* switch LEDs off              */
		}
	}
}

void off(void) {
	int n;
	int m;
	for (m = 0; m < 22; m++) {
		for (n = 0; n < 2; n++) {
			LPC_GPIO0->DATA |= (0UL << 9);               /* switch LEDs off              */
		}
		for (n = 0; n < 5; n++) {
			LPC_GPIO0->DATA &= ~(1UL << 9);               /* switch LEDs off              */
		}
	}
}

void data0(void) {
	on();
	off();
}

void data1(void) {
	on();
	off();
	off();
	off();
}

void start(void) {
//	on();
//	on();
//	on();
//	on();
//	on();
//	on();
//	on();
//	on();
	on();
	on();
	on();
	on();
	on();
	on();
	on();
	on();
	off();
	off();
	off();
	off();
	off();
	off();
	off();
	off();
}


void start2(void) {

	off();
	off();
	off();
	off();
	off();
	off();
	off();
	off();
}


void end(void) {
	on();
	delay_micro(8000);
}






