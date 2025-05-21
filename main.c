
#include <stdint.h>
#include "LPC11xx.h"                    // Device header
//#include "cmsis_os.h"                   // ARM::CMSIS:RTOS:Keil RTX
#include "Board_LED.h"                  // ::Board Support:LED
#include "Board_CLKOUT.h"                  // ::Board Support:LED
#include "Board_Timer.h"                  // ::Board Support:Timer
#include "Board_Port.h"     
#include "Board_Uart.h"     
#include <stdio.h>
#include "Board_I2c.h"
#include <string.h>
#include "Board_Buttons.h"
/*----------------------------------------------------------------------------
  Main function
 *----------------------------------------------------------------------------*/
 
 #include "LPC11xx.h"

void temp(void) {
    // ?????(?????????)
    // ?: LED???????
    // ??????????????????
    for (volatile int i = 0; i < 100000; i++);
}




void GPIOInit(void) {
    // P0_8 (SWDIO??) ? wake-up pin ?????
    LPC_SYSCON->STARTERP0 |= (1 << 0);  // ? P0_8 (start logic wakeup 0) ????
    LPC_SYSCON->PDSLEEPCFG &= ~(1 << 1); // BOD power down disable
    LPC_SYSCON->PDAWAKECFG = LPC_SYSCON->PDSLEEPCFG; // wakeup???????
    LPC_SYSCON->STARTAPRP0 &= ~(1 << 0); // High?????????
    LPC_IOCON->PIO0_8 &= ~0x3F;          // IOCON???
    LPC_IOCON->PIO0_8 |= 0x01;           // GPIO??????
    LPC_GPIO0->DIR &= ~(1 << 8);         // P0_8?????
}



int was_wakeup_from_deep_powerdown(void) {
    // ???????
    return (LPC_PMU->GPREG4 == 0xA5A5);
}

void enter_deep_powerdown(void) {
    LPC_PMU->GPREG4 = 0xA5A5; // ?????????
    LPC_PMU->PCON = 0x3;      // Deep power-down mode
    __WFI();                  // Wait for interrupt
}

int main(void) {
    SystemInit();   // ?????????
    GPIOInit();     // GPIO???wake-up??

    if (was_wakeup_from_deep_powerdown()) {
        LPC_PMU->GPREG4 = 0; // ?????
        temp();              // ??????
    }

    enter_deep_powerdown();  // ??Deep Power-down?
    while (1);               // ??????
}
