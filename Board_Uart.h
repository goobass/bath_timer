

#ifndef ____BOARD_UART
#define ____BOARD_UART

#include <stdint.h>




int32_t Uart_Initialize(unsigned int baudrate);
void UartPutC(const char cPut);
int UartPutS(const char *sPut) ;
#endif /* ____BOARD_UART */
