/*********************************************************************************************************
**--------------File Info---------------------------------------------------------------------------------
** File name:               LCD_quoridor.c
** Descriptions:            Implementation of LCD functionalities for quoridor game
**
**--------------------------------------------------------------------------------------------------------
** Created by:              Pasquale Ferraiuolo (s332262)
** Created date:            28/12/2023
** Version:                 v1.0
** Descriptions:            The original version
**
**--------------------------------------------------------------------------------------------------------
*********************************************************************************************************/

#include "LCD_quoridor.h"
#include <stdio.h>

struct Board board={28,5,7};

struct BoardColor board_color={LightBlue,Black,{White,Red},Red,Green,Magenta,Green,Green};


void LCD_DrawRectangle(uint16_t x_start, uint16_t y_start,uint16_t x_size, uint16_t y_size, uint16_t fill,uint16_t color){
int x_value=x_start;
int y_value=y_start;	
if(!fill){	
LCD_DrawLine(x_start,y_start,x_start+x_size,y_start,color);
LCD_DrawLine(x_start,y_start,x_start,y_start+y_size,color);
LCD_DrawLine(x_start+x_size,y_start,x_start+x_size,y_start+y_size,color);
LCD_DrawLine(x_start,y_start+y_size,x_start+x_size,y_start+y_size,color);
}	

else if (fill){
do{
do{
LCD_SetPoint(x_value,y_value,color);
x_value++;	
}while(x_value<=x_start+x_size);
x_value=x_start;
y_value++;
}while(y_value<=y_start+y_size);
	
}

}

/*inter=intersection*/
void LCD_DrawWall(struct Wall wall,uint16_t color){

int wall_length=(board.square_size*2)+board.space_size;
int wall_height=board.space_size-2;
if(wall.orientation==horizontal) /*Horizontal*/
LCD_DrawRectangle(((wall.square.x)*(board.square_size+board.space_size))+board.displacement,((wall.square.y+1)*(board.square_size+board.space_size))+board.displacement+1-board.space_size,wall_length,wall_height,1,color);
if(wall.orientation==vertical) /*Vertical*/
LCD_DrawRectangle(((wall.square.x+1)*(board.square_size+board.space_size))+board.displacement+1-board.space_size,(wall.square.y*(board.square_size+board.space_size))+board.displacement,wall_height,wall_length,1,color);
		
}

void LCD_DrawPawn(struct SquareCord square,uint8_t id, uint8_t clear ){
int pos_x=(square.x*(board.square_size+board.space_size))+board.displacement+11; 
int pos_y=(square.y*(board.square_size+board.space_size))+board.displacement+3;
int row;	
uint16_t color= ( (clear) ? board_color.background : board_color.pawn[id]);
	
for (row=0;row<6;row=row+2){
LCD_DrawLine(pos_x-row,pos_y+row,pos_x+row+6,pos_y+row,color);		
LCD_DrawLine(pos_x-row,pos_y+row+1,pos_x+row+6,pos_y+row+1,color);		
}	

pos_y=pos_y+6;
pos_x=pos_x-2;

for (row=0;row<4;row=row+2){
LCD_DrawLine(pos_x+row,pos_y+row,pos_x-row+10,pos_y+row,color);		
LCD_DrawLine(pos_x+row,pos_y+row+1,pos_x-row+10,pos_y+row+1,color);		
}	

pos_y=pos_y+4;
pos_x=pos_x+1;

for(row=0;row<10;row=row+2){
LCD_DrawLine(pos_x-row,pos_y+row,pos_x+row+8,pos_y+row,color);	
LCD_DrawLine(pos_x-row,pos_y+row+1,pos_x+row+8,pos_y+row+1,color);
}
row=row-2;
LCD_DrawRectangle(pos_x-row,pos_y+row,16+8,3,1,color);	
}

void LCD_DrawSquare(struct SquareCord square,uint8_t fill,uint16_t color){	
	
/*parameters 1 and 2 are the cooridinates to place the rectangle (add fill so that the border pixel is left out if fill==1),
	parameters 3 and 4 calculate the square size (again, uses fill variable to eventually decrement by 2 the size and left our borders) */	
LCD_DrawRectangle(square.x*(board.square_size+board.space_size)+board.displacement+fill,square.y*(board.square_size+board.space_size)+board.displacement+fill,board.square_size-(fill*2),board.square_size-(fill*2),fill,color);	

}

void LCD_SelectMenu(enum Direction dir){
if(dir==up){
LCD_DrawRectangle(40,200,160,40,0,Black);	
LCD_DrawRectangle(40,130,160,40,0,Red);	
}
else if (dir==down){
LCD_DrawRectangle(40,130,160,40,0,Black);	
LCD_DrawRectangle(40,200,160,40,0,Red);	
}
}




void LCD_DrawMainMenu(){
	/*CLEAR*/
	LCD_DrawRectangle(30,110,180,50,0,board_color.background);	
GUI_Text(102,120,(uint8_t*)"INT 0",board_color.background,board_color.background);
GUI_Text(92,140,(uint8_t*)"to play",board_color.background,board_color.background);
	
	GUI_Text(78,62,(uint8_t*)"Select the",Black,board_color.background);
	GUI_Text(83,90,(uint8_t*)"GAME MODE",Black,board_color.background);
	
	LCD_DrawRectangle(40,130,160,40,0,Red);	
	GUI_Text(73,145,(uint8_t*)"Single board",Black,board_color.background);
	LCD_DrawRectangle(40,200,160,40,0,board_color.square_border);	
	GUI_Text(80,215,(uint8_t*)"Two boards",Black,board_color.background);

	
}

void LCD_DrawSingleBoardMenu(){

	GUI_Text(44,62,(uint8_t*)"Single board: select",Black,board_color.background);
	GUI_Text(48,90,(uint8_t*)"the opposite player",Black,board_color.background);
	
		
	
	/*CLEAR and WRITE*/
	GUI_Text(73,145,(uint8_t*)"Single board",board_color.background,board_color.background);
	GUI_Text(100,145,(uint8_t*)"Human",Black,board_color.background);
	GUI_Text(80,215,(uint8_t*)"Two boards",board_color.background,board_color.background);
	GUI_Text(106,215,(uint8_t*)"NPC",Black,board_color.background);



	
}


void LCD_DrawTwoBoardMenu(){
	LCD_DrawRectangle(40,200,160,40,0,board_color.square_border);
		
		
	GUI_Text(48,62,(uint8_t*)"Two boards: select",Black,board_color.background);
	GUI_Text(77,90,(uint8_t*)"your player",Black,board_color.background);
	
	
	LCD_DrawRectangle(40,130,160,40,0,Red);	
	/*CLEAR and WRITE*/
	GUI_Text(73,145,(uint8_t*)"Single board",board_color.background,board_color.background);
	GUI_Text(100,145,(uint8_t*)"Human",Black,board_color.background);
	GUI_Text(80,215,(uint8_t*)"Two boards",board_color.background,board_color.background);
	GUI_Text(106,215,(uint8_t*)"NPC",Black,board_color.background);
	
}



void LCD_initScreen(){
	if(board_color.background!=White)
	LCD_Clear(board_color.background);
	
LCD_DrawRectangle(30,110,180,50,0,Red);	
GUI_Text(102,120,(uint8_t*)"INT 0",Black,board_color.background);
GUI_Text(92,140,(uint8_t*)"to play",Black,board_color.background);

}

void LCD_DrawBoard(){
	
int x_start=board.displacement, y_start=board.displacement;	
struct SquareCord square;	
	
GUI_Text(44,62,(uint8_t*)"Single board: select",board_color.background,board_color.background);
GUI_Text(48,90,(uint8_t*)"the opposite player",board_color.background,board_color.background);
GUI_Text(100,145,(uint8_t*)"Human",board_color.background,board_color.background);	
LCD_DrawRectangle(40,130,160,40,0,board_color.background);
GUI_Text(106,215,(uint8_t*)"NPC",board_color.background,board_color.background);	
LCD_DrawRectangle(40,200,160,40,0,board_color.background);		
	
for(square.y=0;square.y<7;square.y++){
for(square.x=0; square.x<7;square.x++){
LCD_DrawSquare(square,0,board_color.square_border);	
x_start=x_start+board.square_size+board.space_size;
}
x_start=board.displacement;
y_start=y_start+board.square_size+board.space_size;
	

}
	
LCD_DrawUtils();

}
void LCD_UpdateTimer(int timer){
	 char displayT[2]={0,0};
	
	sprintf(displayT,"%d",timer);
	if (timer<10){
		displayT[1]=displayT[0];
		displayT[0]='0';
	}
	GUI_Text(110,275,(uint8_t*)displayT,Black,board_color.background);

} 

void LCD_UpdateWalls(uint8_t id, uint8_t walls){
	char display[1];
	sprintf(display,"%d",walls);
	
	if(id==0)
	GUI_Text(36,275,(uint8_t*)display,board_color.pawn[0],board_color.background);
	else
		GUI_Text(199,275,(uint8_t*)display,board_color.pawn[1],board_color.background);
}


void LCD_DrawUtils(){
	
	
LCD_DrawRectangle(5,244,70,55,0,board_color.square_border);	
LCD_DrawRectangle(85,244,70,55,0,board_color.square_border);	
LCD_DrawRectangle(165,244,70,55,0,board_color.square_border);	

GUI_Text(9,255,(uint8_t*)"Walls P1",board_color.pawn[0],board_color.background);
GUI_Text(105,255,(uint8_t*)"Time",Black,board_color.background);

GUI_Text(169,255,(uint8_t*)"Walls P2",board_color.pawn[1],board_color.background);

	
}

void LCD_DrawInfoPlayer(uint8_t player_id){

if(player_id==0)
GUI_Text(13,304,(uint8_t*)"Playing as player 1 (white)",Yellow,board_color.background);
	else
GUI_Text(25,304,(uint8_t*)"Playing as player 2 (red)",Yellow,board_color.background);	

}

void LCD_DisplayVictory(uint8_t id,uint8_t this_board_won){ //this_board_won==UINT8_MAX in singleplayer
//LCD_DrawRectangle(5,244,230,70,1,board_color.background);	

	LCD_DrawRectangle(5,244,70,55,0,board_color.background);	
LCD_DrawRectangle(85,244,70,55,0,board_color.background);	
LCD_DrawRectangle(165,244,70,55,0,board_color.background);	
	
GUI_Text(9,255,(uint8_t*)"Walls P1",board_color.background,board_color.background);
GUI_Text(36,275,(uint8_t*)"0",board_color.background,board_color.background);
GUI_Text(101,255,(uint8_t*)"Time:",board_color.background,board_color.background);
GUI_Text(110,275,(uint8_t*)"00",board_color.background,board_color.background);
GUI_Text(169,255,(uint8_t*)"Walls P2",board_color.background,board_color.background);	
GUI_Text(199,275,(uint8_t*)"0",board_color.background,board_color.background);

	

LCD_DrawRectangle(20,250,200,40,0,Red);
	
if(this_board_won!=UINT8_MAX){
	if(this_board_won==1)
  GUI_Text(90,265,(uint8_t*)"YOU WON!",Black,board_color.background);	
	else
	GUI_Text(80,265,(uint8_t*)"YOU LOST :(",Black,board_color.background);	
}
else{

if (id==0)
GUI_Text(70,265,(uint8_t*)"PLAYER 1 WINS!",Black,board_color.background);	

else
GUI_Text(70,265,(uint8_t*)"PLAYER 2 WINS!",Black,board_color.background);	
}}

void LCD_DisplayHandshakeFailed(){

LCD_Clear(board_color.background);
LCD_DrawRectangle(30,110,180,50,0,Red);	
GUI_Text(100,120,(uint8_t*)"ERROR:",Black,board_color.background);
GUI_Text(62,140,(uint8_t*)"Handshake Failed",Black,board_color.background);	

}











