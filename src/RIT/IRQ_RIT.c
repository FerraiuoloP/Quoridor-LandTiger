/*********************************************************************************************************
**--------------File Info---------------------------------------------------------------------------------
** File name:           IRQ_RIT.c
** Last modified Date:  28/12/2023
** Last Version:        V2.00
** Descriptions:        functions to manage interrupts
** Correlated files:    RIT.h
**--------------------------------------------------------------------------------------------------------
*********************************************************************************************************/
#include "lpc17xx.h"
#include "RIT.h"

#include "../timer/timer.h"
#include "../quoridor/quoridor.h"
/******************************************************************************
** Function name:		RIT_IRQHandler
**
** Descriptions:		REPETITIVE INTERRUPT TIMER handler
**
** parameters:			None
** Returned value:		None
**
******************************************************************************/

volatile int KEY0_down=0;
volatile int KEY1_down=0;
volatile int KEY2_down=0;
extern struct Wall temp_wall;
extern enum Mode current_mode;
extern struct Player player[2];
extern uint16_t player_turn;
uint8_t button_off=0;


void RIT_IRQHandler (void)
{					
	static int joystick_dir[8];
	static int joystick_select;
	int multiple_dir=0;
if(button_off==0){	 
	//top right
	if(((LPC_GPIO1->FIOPIN & (1<<29)) == 0) && (LPC_GPIO1->FIOPIN & (1<<28)) == 0){ 
		joystick_dir[top_right]++;
		multiple_dir=1;
		switch(joystick_dir[top_right]){
			case 1:
				if(current_mode==move)
					select_square(top_right);
			default:
				break;
		}
	}
	else{
			joystick_dir[top_right]=0;
	}
	
		//top left
	if(((LPC_GPIO1->FIOPIN & (1<<29)) == 0) && (LPC_GPIO1->FIOPIN & (1<<27)) == 0){ 
		joystick_dir[top_left]++;
		multiple_dir=1;
		switch(joystick_dir[top_left]){
			case 1:
				if(current_mode==move)
					select_square(top_left);
			default:
				break;
		}
	}
	else{
			joystick_dir[top_left]=0;
	}
	
		//bot right
	if(((LPC_GPIO1->FIOPIN & (1<<26)) == 0) && (LPC_GPIO1->FIOPIN & (1<<28)) == 0){ 
		joystick_dir[bot_right]++;
		multiple_dir=1;
		switch(joystick_dir[bot_right]){
			case 1:
				if(current_mode==move)
					select_square(bot_right);
			default:
				break;
		}
	}
	else{
			joystick_dir[bot_right]=0;
	}
	
		//bot left
	if(((LPC_GPIO1->FIOPIN & (1<<26)) == 0) && (LPC_GPIO1->FIOPIN & (1<<27)) == 0){ 
		joystick_dir[bot_left]++;
		multiple_dir=1;
		switch(joystick_dir[bot_left]){
			case 1:
				if(current_mode==move)
					select_square(bot_left);
			default:
				break;
		}
	}
	else{
			joystick_dir[bot_left]=0;
	}
	
if(multiple_dir==0){
	
		/* Joytick UP*/
	if((LPC_GPIO1->FIOPIN & (1<<29)) == 0){	
		
		joystick_dir[up]++;
		switch(joystick_dir[up]){
			case 1:
				if(current_mode==wall)
				hover_wall(temp_wall.square.x,temp_wall.square.y-1);
				else if(current_mode==move)
  					select_square(up);
				else if(current_mode==main_menu || current_mode==single_board_menu || current_mode==two_board_menu)		//in one of the menus
					(menu_select(up));
			default:
				break;
		}
	}
	else{
			joystick_dir[up]=0;
	}
	
	
			/* Joytick DOWN*/
		if((LPC_GPIO1->FIOPIN & (1<<26)) == 0){	
		
		joystick_dir[down]++;
		switch(joystick_dir[down]){
			case 1:
				if(current_mode==wall)
				hover_wall(temp_wall.square.x,temp_wall.square.y+1);
				else if(current_mode==move)
					select_square(down);
				else if(current_mode==main_menu || current_mode==single_board_menu || current_mode==two_board_menu)		//in one of the menus
					(menu_select(down));
			default:
				break;
		}
	}
	else{
			joystick_dir[down]=0;
	}
	
			/* Joytick LEFT*/
		if((LPC_GPIO1->FIOPIN & (1<<27)) == 0){	
		
		joystick_dir[left]++;
		switch(joystick_dir[left]){
			case 1:	
				if(current_mode==wall)
				hover_wall(temp_wall.square.x-1,temp_wall.square.y);
				if(current_mode==move)
					select_square(left);
			default:
				break;
		}
	}
	else{
			joystick_dir[left]=0;
	}
			
		/* Joytick RIGHT*/
		if((LPC_GPIO1->FIOPIN & (1<<28)) == 0){	
		
		joystick_dir[right]++;
		switch(joystick_dir[right]){
			case 1:
				if(current_mode==wall)
				hover_wall(temp_wall.square.x+1,temp_wall.square.y);
				if(current_mode==move)
					select_square(right);
			default:
				break;
		}
	}
	else{
			joystick_dir[right]=0;
	}
	
	
			/* Joytick SELECT*/
		if((LPC_GPIO1->FIOPIN & (1<<25)) == 0){	
		
		joystick_select++;
		switch(joystick_select){
			case 1:
				if(current_mode==wall)
					place_wall();
				else if(current_mode==move)
					move_player();
				else if(current_mode==main_menu || current_mode==single_board_menu || current_mode==two_board_menu)		//in one of the menus
 					(menu_confirm());
			default:
				break;
		}
	}
	else{
			joystick_select=0;
	}
	
	
	
	/* button management */
		/*KEY0*/
	if(KEY0_down>=1){ 
		if((LPC_GPIO2->FIOPIN & (1<<10)) == 0){	/* KEY1 pressed */
			switch(KEY0_down){				
				case 2:				/* pay attention here: please see slides 19_ to understand value 2 */
					if(current_mode==waiting)
						display_menu();
						
					break;
				default:
					break;
			}
			KEY0_down++;
		}
		else {	/* button released */
			KEY0_down=0;			
			NVIC_EnableIRQ(EINT0_IRQn);							 /* enable Button interrupts			*/
			LPC_PINCON->PINSEL4    |= (1 << 20);     /* External interrupt 0 pin selection */
		}
	}
	
	/*KEY1*/
	if(KEY1_down>=1){ 
		if((LPC_GPIO2->FIOPIN & (1<<11)) == 0){	/* KEY1 pressed */
			switch(KEY1_down){				
				case 2:				/* pay attention here: please see slides 19_ to understand value 2 */
					if(current_mode==move || current_mode==wall)
					switch_mode();
					break;
				default:
					break;
			}
			KEY1_down++;
		}
		else {	/* button released */
			KEY1_down=0;			
			NVIC_EnableIRQ(EINT1_IRQn);							 /* enable Button interrupts			*/
			LPC_PINCON->PINSEL4    |= (1 << 22);     /* External interrupt 0 pin selection */
		}
	}

		/*KEY2*/
	if(KEY2_down>=1){ 
		if((LPC_GPIO2->FIOPIN & (1<<12)) == 0){	/* KEY1 pressed */
			switch(KEY2_down){				
				case 2:				/* pay attention here: please see slides 19_ to understand value 2 */
					if(current_mode==wall)
					rotate_hover_wall();
					break;
				default:
					break;
			}
			KEY2_down++;
		}
		else {	/* button released */
			KEY2_down=0;			
			NVIC_EnableIRQ(EINT2_IRQn);							 /* enable Button interrupts			*/
			LPC_PINCON->PINSEL4    |= (1 << 24);     /* External interrupt 0 pin selection */
		}
	}
}	
	}
  LPC_RIT->RICTRL |= 0x1;	/* clear interrupt flag */
	
  return;
}

/******************************************************************************
**                            End Of File
******************************************************************************/
