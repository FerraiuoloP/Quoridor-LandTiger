/*********************************************************************************************************
**--------------File Info---------------------------------------------------------------------------------
** File name:           joystick.h
** Last modified Date:  2018-12-30
** Last Version:        V1.00
** Descriptions:        Atomic joystick init functions
** Correlated files:    lib_joystick.c, funct_joystick.c
**--------------------------------------------------------------------------------------------------------       
*********************************************************************************************************/

#include "lpc17xx.h"
#include "joystick.h"

/*----------------------------------------------------------------------------
  Function that initializes joysticks and switch them off
 *----------------------------------------------------------------------------*/

void joystick_init(void) {
	/* joystick up functionality */
  LPC_PINCON->PINSEL3 &= ~(3<<26);	//PIN mode GPIO (00b value per P1.29)
	LPC_GPIO1->FIODIR   &= ~(1<<29);	//P1.25 Input (joysticks on PORT1 defined as Input) 
	/* joystick down functionality */
	LPC_PINCON->PINSEL3 &= ~(3<<20);	
	LPC_GPIO1->FIODIR   &= ~(1<<26);	
	/* joystick left functionality */
	LPC_PINCON->PINSEL3 &= ~(3<<22);	
	LPC_GPIO1->FIODIR   &= ~(1<<27);	
	/* joystick right functionality */
	LPC_PINCON->PINSEL3 &= ~(3<<24);	
	LPC_GPIO1->FIODIR   &= ~(1<<28);	
	/* joystick select functionality */
	LPC_PINCON->PINSEL3 &= ~(3<<18);	
	LPC_GPIO1->FIODIR   &= ~(1<<25);	
	
}
