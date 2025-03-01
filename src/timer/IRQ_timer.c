/*********************************************************************************************************
**--------------File Info---------------------------------------------------------------------------------
** File name:           IRQ_timer.c
** Last modified Date:  28/12/2023
** Last Version:        V2.00
** Descriptions:        functions to manage T0 interrupt
** Correlated files:    timer.h
**--------------------------------------------------------------------------------------------------------
*********************************************************************************************************/
#include <string.h>
#include "lpc17xx.h"
#include "timer.h"
#include "../GLCD/GLCD.h" 
#include "../quoridor/quoridor.h"
#include "../GLCD/LCD_quoridor.h"
int timer=20;
uint16_t expired_time_untilINT0=0;
uint16_t expired_time_untilHandshake=0;
uint8_t skip_sent=0;
extern enum Mode current_mode;
extern struct Wall temp_wall;
extern enum Orientation placed_wall[6][6];
extern uint16_t two_board;
extern uint16_t my_player_id;
extern uint16_t player_turn;
/******************************************************************************
** Function name:		Timer0_IRQHandler
**
** Descriptions:		Timer/Counter 0 interrupt handler
**
** parameters:			None
** Returned value:		None
**
******************************************************************************/

void TIMER0_IRQHandler (void)
{
	timer--;
	
	if(timer<=0){
		
		if ((two_board==0 || (two_board==1 && player_turn==my_player_id))&& skip_sent==0){ //if local play or it is this landiger's turn
			if(two_board==1)//2 boards
				send_msg_skip_turn();
			skip_sent=1;
			skip_turn();
			
		}
		
	}
	else
		LCD_UpdateTimer(timer);
	
	
  LPC_TIM0->IR = 1;			/* clear interrupt flag */
  return;
}

void TIMER1_IRQHandler (void)
{
	//TO DETERMINATE FIRST PLAYER TO CLICK INT0
	if(current_mode==waiting){
	expired_time_untilINT0++;
	}
	expired_time_untilHandshake++;
  LPC_TIM1->IR = 1;			/* clear interrupt flag */
  return;
}

void TIMER2_IRQHandler (void) //USED FOR HANDSHAKE
{
	hanshake_failed();
  LPC_TIM1->IR = 1;			/* clear interrupt flag */
  return;
}
/******************************************************************************
**                            End Of File
******************************************************************************/
