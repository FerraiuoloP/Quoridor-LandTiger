/****************************************Copyright (c)****************************************************
**                                      
**                                 http://www.powermcu.com
**
**--------------File Info---------------------------------------------------------------------------------
** File name:               main.c
** Descriptions:            The GLCD application function
**
**--------------------------------------------------------------------------------------------------------
** Created by:              AVRman
** Created date:            2010-11-7
** Version:                 v1.0
** Descriptions:            The original version
**
**--------------------------------------------------------------------------------------------------------
** Modified by:             Paolo Bernardi
** Modified date:           03/01/2020
** Version:                 v2.0
** Descriptions:            basic program for LCD and Touch Panel teaching
**
**--------------------------------------------------------------------------------------------------------
**
** Modified by:             Pasquale Ferraiuolo (s332262)
** Modified date:           28/12/2023
** Version:                 v3.0
** Descriptions:            implementation of quoridor game
**
*********************************************************************************************************/
/* Includes ------------------------------------------------------------------*/
#include "LPC17xx.h"
#include "GLCD/GLCD.h" 
#include "button_EXINT/button.h"
#include "timer/timer.h"
#include "RIT/RIT.h"
#include "joystick/joystick.h"
#include "quoridor/quoridor.h"
#include "CAN/CAN.H"
#define SIMULATOR 1

#ifdef SIMULATOR
extern uint8_t ScaleFlag; // <- ScaleFlag needs to visible in order for the emulator to find the symbol (can be placed also inside system_LPC17xx.h but since it is RO, it needs more work)
#endif


int main(void)
{
  SystemInit();  												/* System Initialization (i.e., PLL)  */
	CAN_Init();
  LCD_Initialization();
	
  //TP_Init();
	//TouchPanel_Calibrate();
	
	//LCD_Clear(White);
		
	
	
	BUTTON_init();												/* BUTTON Initialization              */
	joystick_init();											/* Joystick Initialization            */
	
	init_RIT(0x004C4B40);									/* RIT Initialization 50 msec       	*/
	enable_RIT();													/* RIT enabled												*/

	
	init_timer(1,0x002625A0); 						  /* 0.1s * 25MHz = 25*10^5 = 0x2625A0 */
	enable_timer(1);
	
	
	
		init_quoridor();
	
	LPC_SC->PCON |= 0x1;									/* power-down	mode										*/
	LPC_SC->PCON &= ~(0x2);						
	
  while (1)	
  {
		__ASM("wfi");
  }
	
	
}

/*********************************************************************************************************
      END FILE
*********************************************************************************************************/
