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
#include "Board_LED.h"                  // ::Board Support:LED

#define LED_NUM             (8)

/* LEDs: P2.0..7 */

/**
  \fn          int32_t LED_Initialize (void)
  \brief       Initialize LEDs
  \returns
   - \b  0: function succeeded
   - \b -1: function failed
*/
int32_t LED_Initialize (void) {

  LPC_SYSCON->SYSAHBCLKCTRL |= ((1UL <<  6) |     /* enable clock for GPIO        */
                                (1UL << 16) );    /* enable clock for IOCON       */

  LPC_IOCON->PIO0_7   =  ((0UL << 0) |            /* select GPIO function         */
                          (3UL << 6)  );          /* keep reserved values         */

  LPC_GPIO0->DIR  |=  (1UL <<  7) ;               /* configure PINs as Output     */
  LPC_GPIO0->DATA &= ~0x000000FFUL;               /* switch LEDs off              */

////////////////////////////////////////////////////////////////////////////////////////
	LPC_IOCON->PIO0_8   =  ((0UL << 0) |            /* select GPIO function         */
                          (3UL << 6)  );          /* keep reserved values         */

  LPC_GPIO0->DIR  |=  (1UL <<  8) ;               /* configure PINs as Output     */
  LPC_GPIO0->DATA &= ~0x000001FFUL;               /* switch LEDs off              */
////////////////////////////////////////////////////////////////////////////////////////	
	
  return 0;
}

/**
  \fn          int32_t LED_On (uint32_t num)
  \brief       Turn on requested LED
  \param[in]   num  LED number
  \returns
   - \b  0: function succeeded
   - \b -1: function failed
*/
int32_t LED_On (uint32_t num) {

  if (num <= LED_NUM) {
    LPC_GPIO0->DATA |=  (1UL << num);             /* LED On                   */
  }
  return 0;
}

/**
  \fn          int32_t LED_Off (uint32_t num)
  \brief       Turn off requested LED
  \param[in]   num  LED number
  \returns
   - \b  0: function succeeded
   - \b -1: function failed
*/
int32_t LED_Off (uint32_t num) {

  if (num <= LED_NUM) {
    LPC_GPIO0->DATA &= ~(1UL << num);             /* LED Off                  */
  }
  return 0;
}

/**
  \fn          int32_t LED_SetOut (uint32_t val)
  \brief       Write value to LEDs
  \param[in]   val  value to be displayed on LEDs
  \returns
   - \b  0: function succeeded
   - \b -1: function failed
*/
int32_t LED_SetOut(uint32_t value) {
  uint32_t i;

  for (i = 0; i < LED_NUM; i++) {
    if (value & (1<<i)) {
      LED_On (i);
    } else {
      LED_Off(i);
    }
  }
  return 0;
}

/**
  \fn          uint32_t LED_GetCount (void)
  \brief       Get number of LEDs
  \return      Number of available LEDs
*/
uint32_t LED_GetCount (void) {
  return LED_NUM;
}

