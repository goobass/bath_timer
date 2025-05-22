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
#include "Board_I2c.h"
#include <stdint.h>
#include "Board_Timer.h"                  // ::Board Support:Timer
#include "Board_LED.h"                  // ::Board Support:LED
#include <string.h>
#include <stdio.h>
void  I2c_Initialize   (void){
  uint32_t num = 7;	
///////////////////////////////////////////////////////////////////////
   num = 8;		
///////////////////////////////////////////////////////////////////////	
	LED_Off(num);  
	delay_ms(10);  
	LED_On(num);  	
	//delay_sec(1);  	
//
	LPC_SYSCON->SYSAHBCLKCTRL |= (1 << 5); 
	LPC_SYSCON->SYSAHBCLKCTRL |= (1 << 16); 	
	LPC_IOCON->PIO0_4 = 0x01; // SDA
	LPC_IOCON->PIO0_5 = 0x01; // SCL
	LPC_SYSCON->PRESETCTRL  |= (1 << 1); // SCL High time
	LPC_I2C->SCLH = 200; // SCL High time
	LPC_I2C->SCLL = 200; // SCL Low time
	LPC_I2C->CONSET = (1 << 6); // I2C Enable bit をセット
	
I2C_MasterTransmit(0x3E,0x38);	
I2C_MasterTransmit(0x3E,0x39);
I2C_MasterTransmit(0x3E,0x14);	
I2C_MasterTransmit(0x3E,0x78);	
I2C_MasterTransmit(0x3E,0x5F);
delay_ms(200);  
I2C_MasterTransmit(0x3E,0x6A);

I2C_MasterTransmit(0x3E,0x0C);	




	
}
void I2C_MasterTransmit(uint8_t slave_addr, uint8_t data){
	// C0=1 Rs=0
	LPC_I2C->CONSET = (1 << 5); // Startビットセット
	while (!(LPC_I2C->STAT == 0x08)); // Start Conditionの確認
	LPC_I2C->CONCLR = (1 << 5); // Startビットクリア
	LPC_I2C->CONCLR = (1 << 3); // SIクリア

	
  LPC_I2C->DAT = (uint32_t)(slave_addr << 1); // スレーブアドレス送信		
	LPC_I2C->CONCLR = (1 << 3); // SIクリア
  while (!(LPC_I2C->STAT == 0x18));// ACK確認	

  LPC_I2C->DAT = 0x80; // C0=1 Rs=0
	LPC_I2C->CONCLR = (1 << 3); // SIクリア
  while (!(LPC_I2C->STAT == 0x28));// ACK確認		
	
	
  LPC_I2C->DAT = data; // データ送信
	LPC_I2C->CONCLR = (1 << 3); // SIクリア
  while (!(LPC_I2C->STAT == 0x28));// ACK確認	


  LPC_I2C->DAT = 0x00; // C0=0 Rs=0
	LPC_I2C->CONCLR = (1 << 3); // SIクリア
  while (!(LPC_I2C->STAT == 0x28));// ACK確認		


	LPC_I2C->CONSET = (1 << 4); // Stopビットセット
	LPC_I2C->CONCLR = (1 << 3); // SIクリア		
		
	delay_micro(100);
}





void I2C_write( uint8_t data){
	//C0=1 Rs=1
	uint8_t slave_addr = 0x3E;
	
	LPC_I2C->CONSET = (1 << 5); // Startビットセット
	while (!(LPC_I2C->STAT == 0x08)); // Start Conditionの確認
	LPC_I2C->CONCLR = (1 << 5); // Startビットクリア
	LPC_I2C->CONCLR = (1 << 3); // SIクリア

	
  LPC_I2C->DAT = (uint32_t)(slave_addr << 1); // スレーブアドレス送信		
	LPC_I2C->CONCLR = (1 << 3); // SIクリア
  while (!(LPC_I2C->STAT == 0x18));// ACK確認	

  LPC_I2C->DAT = 0xC0; // C0=1 Rs=1
	LPC_I2C->CONCLR = (1 << 3); // SIクリア
  while (!(LPC_I2C->STAT == 0x28));// ACK確認		
	
	
  LPC_I2C->DAT = data; // データ送信
	LPC_I2C->CONCLR = (1 << 3); // SIクリア
  while (!(LPC_I2C->STAT == 0x28));// ACK確認	


  LPC_I2C->DAT = 0x40; //  C0=0 Rs=1
	LPC_I2C->CONCLR = (1 << 3); // SIクリア
  while (!(LPC_I2C->STAT == 0x28));// ACK確認		


	LPC_I2C->CONSET = (1 << 4); // Stopビットセット
	LPC_I2C->CONCLR = (1 << 3); // SIクリア		
		
	delay_micro(100);
}

void I2C_write_command( uint8_t data){

	uint8_t slave_addr = 0x3E;
	
	LPC_I2C->CONSET = (1 << 5); // Startビットセット
	while (!(LPC_I2C->STAT == 0x08)); // Start Conditionの確認
	LPC_I2C->CONCLR = (1 << 5); // Startビットクリア
	LPC_I2C->CONCLR = (1 << 3); // SIクリア

	
  LPC_I2C->DAT = (uint32_t)(slave_addr << 1); // スレーブアドレス送信		
	LPC_I2C->CONCLR = (1 << 3); // SIクリア
  while (!(LPC_I2C->STAT == 0x18));// ACK確認	

  LPC_I2C->DAT = 0x80; // C0=1 Rs=0
	LPC_I2C->CONCLR = (1 << 3); // SIクリア
  while (!(LPC_I2C->STAT == 0x28));// ACK確認		
	
	
  LPC_I2C->DAT = data; // データ送信
	LPC_I2C->CONCLR = (1 << 3); // SIクリア
  while (!(LPC_I2C->STAT == 0x28));// ACK確認	


  LPC_I2C->DAT = 0x00; //  C0=0 Rs=0
	LPC_I2C->CONCLR = (1 << 3); // SIクリア
  while (!(LPC_I2C->STAT == 0x28));// ACK確認		


	LPC_I2C->CONSET = (1 << 4); // Stopビットセット
	LPC_I2C->CONCLR = (1 << 3); // SIクリア		
		
	delay_micro(100);
}


void display(uint32_t time,uint32_t count){
  uint8_t n;
	char text[80];
	char temp[20];
	I2C_write_command(2);
	
	sprintf(temp, "%d", time);
	strcpy(text,"time  = ");
	strcat(text,temp);
	strcat(text,"                     ");
	
	for(n=0x0;n<40;n++){
		I2C_write(text[n]);
	}
	sprintf(temp, "%d", count);
	strcpy(text,"count = ");
	strcat(text,temp);
	strcat(text,"                     ");
	for(n=0x0;n<40;n++){
		I2C_write(text[n]);
	}
	
}

void display_time(uint32_t time){
  uint8_t n;
	char text[80];

	I2C_write_command(0x88);
	for(n=0;n<80;n++){
		text[n] = 0;
	}
	sprintf(text, "%d", time);
	strcat(text,"                  ");


	
	for(n=0x0;n<5;n++){
		I2C_write(text[n]);
	}
	
}



