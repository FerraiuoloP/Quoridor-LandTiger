/*********************************************************************************************************
**--------------File Info---------------------------------------------------------------------------------
** File name:               LCD_quoridor.h
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

#include "LPC17xx.h"
#include "GLCD.h"
#include "../quoridor/quoridor.h"

struct Board{
uint8_t square_size;
uint8_t space_size;	
uint8_t displacement;
};

struct BoardColor{
uint16_t background;
uint16_t square_border;/*of border*/
uint16_t pawn[2];
uint16_t wall;
uint16_t valid_wall;
uint16_t invalid_wall;
uint16_t hover;
uint16_t hover_border;	
};

void LCD_DrawBoard(void);
void LCD_DrawWall(struct Wall inter,uint16_t color);
void LCD_DrawPawn(struct SquareCord square,uint8_t id, uint8_t clear);
void LCD_DrawSquare(struct SquareCord square,uint8_t fill,uint16_t color);
void LCD_UpdateTimer(int timer);
void LCD_DrawUtils(void);
void LCD_UpdateWalls(uint8_t id, uint8_t walls);
void LCD_DisplayVictory(uint8_t id,uint8_t this_board_won);
void LCD_DrawMainMenu(void);
void LCD_DrawSingleBoardMenu(void);
void LCD_DrawTwoBoardMenu(void);
void LCD_SelectMenu(enum Direction dir);
void LCD_initScreen(void);
void LCD_DisplayHandshakeFailed(void);
void LCD_DrawInfoPlayer(uint8_t id);



