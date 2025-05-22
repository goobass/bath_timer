#include "LPC11xx.h"                    // Device header
#include "Board_Uart.h"                  //
#include "Board_Timer.h"                  // ::Board Support:Timer
#define LSR_THRE   0x20
int32_t Uart_Initialize(unsigned int baudrate) {

	
	  // pin
  LPC_IOCON->PIO1_6 = 0x01; // RXD
  LPC_IOCON->PIO1_7 = 0x01; // TXD
	
  LPC_SYSCON->SYSAHBCLKCTRL |= ((1UL <<  6) |     /* enable clock for GPIO        */
	                              (1UL <<  12) | 
                                (1UL << 16) );    /* enable clock for IOCON       */
	
	LPC_SYSCON->UARTCLKDIV = 0x01; // UART clock



  // ???????
  LPC_UART->LCR |= (1 << 7); // DLAB???????
  LPC_UART->DLL = (uint8_t)(SystemCoreClock / (16 * baudrate)); // ???????????????????
  LPC_UART->DLM = (uint8_t)((SystemCoreClock / (16 * baudrate)) >> 8);
  
  // ?????
  LPC_UART->LCR &=(unsigned int) ~(1 << 7); // DLAB???????
  LPC_UART->LCR |= ((3 << 0) | (0 <<2)); // ????8???????????1???????

  return 0;
}

// send a character
void UartPutC(const char cPut) {
    while (!(LPC_UART->LSR & LSR_THRE));   // wait for TX buffer ready
    LPC_UART->THR = cPut;
}



// send a string
int UartPutS(const char *sPut) {
    int n;
    for (n = 0; *sPut; sPut++, n++) {
	//							delay_ms(10);
        UartPutC(*sPut);

    }
    return n;
}
