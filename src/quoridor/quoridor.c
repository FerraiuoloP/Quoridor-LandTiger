/*********************************************************************************************************
**--------------File Info---------------------------------------------------------------------------------
** File name:               quoridor.c
** Descriptions:            Implementation of quoridor game
**
**--------------------------------------------------------------------------------------------------------
** Created by:              Pasquale Ferraiuolo (s332262)
** Created date:            28/12/2023
** Version:                 v1.0
** Descriptions:            The original version
**
**--------------------------------------------------------------------------------------------------------
*********************************************************************************************************/

#include "../GLCD/LCD_quoridor.h"
#include "quoridor.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../timer/timer.h"
#include "queue.h"
#include "../RIT/RIT.h"
#include "../CAN/CAN.h"

struct Wall temp_wall = {undefined_ori, 0, 0};
struct Player player[2] = {{8, 0, 3, 6},{8, 6, 3, 0}};
extern struct SquareCord queue[MAX_SIZE];
enum Mode current_mode = waiting;

uint16_t two_board=0;
uint16_t ai_player=0;
uint16_t my_player_id=0;
extern uint8_t button_off;
uint16_t player_turn = 0;
extern uint8_t skip_sent;
extern volatile int timer;
struct SquareCord adj_squares[8];
enum Direction selected_square=up;

enum Orientation placed_wall[6][6]; //[y][x]==coordinates | values: 0=vertical,1=horizontal,2=no wall
extern struct BoardColor board_color;
uint32_t player_move = 0;
uint8_t handshake_confirmed=0;

extern uint16_t expired_time_untilINT0;
extern uint16_t expired_time_untilHandshake;
extern uint32_t messsage_recived;
void init_quoridor()
{
	LCD_initScreen();
	memset(placed_wall, undefined_ori, 36);
	memset(adj_squares,255,16);
}

void hover_wall(uint8_t x_square, uint8_t y_square)
{
	if (x_square > 5 || y_square > 5)
		return;

	manage_wall_conflicts(temp_wall, 1,placed_wall);

	temp_wall.square.x = x_square;
	temp_wall.square.y = y_square;

	
	
		if (manage_wall_conflicts(temp_wall, 0,placed_wall) == 0)
			LCD_DrawWall(temp_wall, board_color.valid_wall);
		else
			LCD_DrawWall(temp_wall, board_color.invalid_wall);
	
}

void switch_mode()
{

	if (current_mode == move && player[player_turn].walls > 0)
	{
		turn_off_squares_hilight();
		selected_square= undefined_dir;

		temp_wall.orientation = horizontal;
		temp_wall.square.x = 3;
		temp_wall.square.y = 2;
		
		if (manage_wall_conflicts(temp_wall, 0,placed_wall) == 0)
			LCD_DrawWall(temp_wall, board_color.valid_wall);
		else
			LCD_DrawWall(temp_wall, board_color.invalid_wall);
		
		current_mode = wall;
	}
	else if (current_mode == wall)
	{
		manage_wall_conflicts(temp_wall, 1,placed_wall);
		hilight_squares(0);
		current_mode = move;
	}
}



void rotate_hover_wall()
{

	manage_wall_conflicts(temp_wall, 1,placed_wall);
	temp_wall.orientation = (enum Orientation)(1 - temp_wall.orientation);

	if (manage_wall_conflicts(temp_wall, 0,placed_wall) == 0)
			LCD_DrawWall(temp_wall, board_color.valid_wall);
		else
			LCD_DrawWall(temp_wall, board_color.invalid_wall);
}

void start_game()
{
	LCD_DrawPawn(player[0].square, 0, 0);
	LCD_DrawPawn(player[1].square, 1, 0);
	
	selected_square=undefined_dir;
	current_mode = move;
	
	init_timer(0,0x17D7840); 						  /* 1s * 25MHz = 25*10^6 = 0x17D7840 */
	//enable_timer(0);
	LCD_UpdateTimer(timer);
	LCD_UpdateWalls(0, player[0].walls);
	LCD_UpdateWalls(1, player[1].walls);
	enable_timer(0);
	
	if((two_board==1 && ai_player==0 && my_player_id==0)|| two_board==0)
		hilight_squares(0);
	if(ai_player==1 && two_board==1 && my_player_id==0)//if ai enabled, there are multiple board and this board is the first player, start ai for the first time.
		make_move_ai();	
	if(two_board==1 && my_player_id==1)
		button_off=1;
	
}

void create_player_move(uint8_t placing_wall,enum Orientation wall_orientation,uint8_t x,uint8_t y){
player_move = 0;
player_move|= player_turn <<24;
player_move |= placing_wall<<20;
player_move |= (int)wall_orientation<<16;	
player_move |= y<<8;	
player_move |= x;	
	
}

void place_wall()
{
	struct SquareCord tmp={0,0};
	if (manage_wall_conflicts(temp_wall, 0,placed_wall) == 0)
	{
		LCD_DrawWall(temp_wall, board_color.valid_wall);
		placed_wall[temp_wall.square.y][temp_wall.square.x]	=temp_wall.orientation;
		
		if (BFS_search(0,placed_wall,player,&tmp) && BFS_search(1,placed_wall,player,&tmp))
		{

			LCD_DrawWall(temp_wall, board_color.wall);
			placed_wall[temp_wall.square.y][temp_wall.square.x] = temp_wall.orientation;
			LCD_UpdateWalls(player_turn, --player[player_turn].walls); // decrement walls and then update LCD
			create_player_move(1,temp_wall.orientation,temp_wall.square.x,temp_wall.square.y); 
			next_turn();
		}
		 else
		 placed_wall[temp_wall.square.y][temp_wall.square.x]	=undefined_ori;
	}
	else 
		LCD_DrawWall(temp_wall, board_color.invalid_wall);//TODO: NON SERVE?
}



void send_start_msg(){
uint64_t message_tmp;
message_tmp|=(0xFF000000);
send_msg(my_player_id,message_tmp);

}

void send_handshake(){
	uint32_t handshake=0;
		handshake|=0xC0000000;
		handshake|=expired_time_untilHandshake<<15;
		handshake|=(expired_time_untilINT0 &0xEFFF);
		send_msg(2,handshake);
}


void send_msg(int id, uint32_t msg){


CAN_TxMsg.data[0] = (msg  & 0xFF000000) >> 24;
        CAN_TxMsg.data[1] = (msg  & 0xFF0000) >> 16;
        CAN_TxMsg.data[2] = (msg & 0xFF00 ) >> 8;
        CAN_TxMsg.data[3] = msg & 0xFF;
        CAN_TxMsg.len = 4;
        CAN_TxMsg.id = id;
        CAN_TxMsg.format = STANDARD_FORMAT;
        CAN_TxMsg.type = DATA_FRAME;
        CAN_wrMsg (1, &CAN_TxMsg);

}

void send_msg_skip_turn(){

uint64_t message_tmp=0;
message_tmp|=my_player_id<<24;
message_tmp|=(0x1 <<16);
send_msg(my_player_id,message_tmp);

}




void recived_msg(uint32_t message_recived){
int i,x,y,tmp;
if(current_mode!=not_valid_connection	&& current_mode!=game_end	){	
	
if(message_recived>>24==0xFF){ //2nd player reached the drawn board
start_game();

}	
else if(message_recived>>30==0x3){ //handshake
	
	if(current_mode==waiting_other_player){
		NVIC_DisableIRQ(TIMER2_IRQn);
		disable_timer(2);
		
	}
	else { //if this landtiger is the 2nd one
	NVIC_DisableIRQ(TIMER1_IRQn);
	disable_timer(1);
	handshake_confirmed=1; 
	send_handshake();
	
}
	
tmp=(((message_recived<<2)>>17) - ((message_recived)& 0x7FFF));
 if (tmp-expired_time_untilHandshake - expired_time_untilINT0==0) 
	 my_player_id= (current_mode==waiting_other_player) ? 0:1; //if INT0 pressed at the same time the first selecting the multiplayer mode is player0
 else
	my_player_id= (tmp>  expired_time_untilHandshake - expired_time_untilINT0) ? 1 : 0; //get the first player that pressed INT0
	
if(current_mode==waiting_other_player)
	LCD_DrawInfoPlayer(my_player_id);

}



else {
x=((message_recived) & 0xFF);	
y=((message_recived>>8) & 0xFF);		
	
if(((message_recived<<8)>>24)	==0x1 && ((message_recived<<8)>>24)	==0x1 ){	//turn skipped
	next_turn(); //no need to call skip_turn() since if it is opponent turn the board is already "clear"
	return;
}
else if(((message_recived>>20) & 0xF)==1){	 //placing wall
temp_wall.square.x = x;
temp_wall.square.y = y;
temp_wall.orientation=(enum Orientation) ((message_recived>>16) & 0xF);
turn_off_squares_hilight();
place_wall();	
	
}
else  if(((message_recived>>20) & 0xF)==0){	//moving player
	hilight_squares(1);
	
for(i=0;i<8;i++){
	if(x==adj_squares[i].x && y==adj_squares[i].y){
			tmp=i;
				break;
		}
}
select_square((enum Direction)tmp); 
move_player();	
}

}
}}

void next_turn()
{
	
	reset_timer(0);
	
	if(two_board==1 && my_player_id==player_turn && skip_sent==0)//skip message is sent in IRQ_TIMER already
		send_msg(my_player_id,player_move);
	skip_sent=0;		
	

	/*here selected_square is already set to undefined_dir and hilight is off (managed before for each case))*/
	selected_square = undefined_dir;
	current_mode = move;
	player_turn = 1 - (int)player_turn;

	timer = 20;

	

	enable_timer(0);
	LCD_UpdateTimer(timer);
	
	player_move=0;
	player_move=1<<16; //invalid move, to keep if turn is skipped.
	button_off=0;
	
	if(two_board==1 && my_player_id!=player_turn)//if it is turn of the other board
	button_off=1;
		
		
		
	
	else if(ai_player==1 && ((two_board==0 && player_turn==1) || (two_board==1 && my_player_id==player_turn)))//if it is AI turn
	make_move_ai();
	
	else
		hilight_squares(0);
	
		
}

void skip_turn(){

if(current_mode==move)
			turn_off_squares_hilight();
if(current_mode==wall)
		manage_wall_conflicts(temp_wall,1,placed_wall);
	next_turn();
		
}




void make_move_ai(){
int wall=0,i,j,k,tmp2,tmp3,heuristic;

	struct SquareCord next_square_BFS;
		struct SquareCord best_move_ia;
		struct Wall best_wall_ia;

		hilight_squares(1);
		i=BFS_search(0,placed_wall,player,&next_square_BFS);
			if(player_turn==0)
				best_move_ia=next_square_BFS;
		j=BFS_search(1,placed_wall,player,&next_square_BFS);
		if(player_turn==1)
			best_move_ia=next_square_BFS;
	
		heuristic=j-i;// TO MAXIMIZE FOR PLAYER 1 AND MINIMIZE FOR PLAYER 2
		if( ((heuristic<0 && player_turn==0) || (heuristic>0 && player_turn==1)) && player[player_turn].walls>0){//IF OPPONENT IS WINNING, AND AI HAS WALLS LEFT
		for (k=0;k<2;k++){
		for(i=0;i<6;i++){
		for(j=0;j<6;j++){
			if(placed_wall[j][i]==undefined_ori && manage_wall_conflicts(initWall(i,j,(enum Orientation)k),0,placed_wall)==0){
				placed_wall[j][i]=(enum Orientation) k;
				tmp2=BFS_search(0,placed_wall,player,&next_square_BFS);
				tmp3=BFS_search(1,placed_wall,player,&next_square_BFS);
				if(tmp2!=0 && tmp3!=0 && ((tmp3-tmp2>heuristic && player_turn==0) || (tmp3-tmp2<heuristic && player_turn==1))){ //check if it is a blocking wall and if it the best wall you can place
				heuristic=tmp3-tmp2;
				best_wall_ia.square.x=i;
			best_wall_ia.square.y=j;
			best_wall_ia.orientation=(enum Orientation)k;
			wall=1;
				
				}
				placed_wall[j][i]=undefined_ori;
			}
		
		}}}}
		if (wall){
			temp_wall.square.x = best_wall_ia.square.x;
		temp_wall.square.y = best_wall_ia.square.y;
			temp_wall.orientation=best_wall_ia.orientation;
			turn_off_squares_hilight();
			place_wall();	
			
	
				}
			else{
				for(i=0;i<8;i++){
				if(best_move_ia.x==adj_squares[i].x && best_move_ia.y==adj_squares[i].y){
				tmp2=i;
					break;
				}
				}
			select_square((enum Direction) tmp2);
			
			move_player();
			
			}
	

}

void display_menu(){
current_mode=main_menu;
LCD_DrawMainMenu();

}


/*hover menu selection*/
void menu_select(enum Direction dir){
	if(selected_square!=dir){
selected_square=dir;
LCD_SelectMenu(dir);
	}
}

/*confirm menu selection*/
void menu_confirm(){	
if(current_mode==main_menu){
if(selected_square==up){
	LCD_DrawSingleBoardMenu();
	current_mode=single_board_menu;
}


else if (selected_square==down){
		LCD_DrawTwoBoardMenu();
		current_mode=two_board_menu;
}
selected_square=up;
}
else {
	
	ai_player= (selected_square==up) ?  0  : 1;
	if(current_mode==two_board_menu){ //two-board selected
	two_board=1;
	if(handshake_confirmed==0){ //first player to exit menu interface
		disable_timer(1);
		init_timer(2,0x02FAF080); 						  /* 5s * 25MHz = 25*5*10^6 = 0x07735940 */
		enable_timer(2);
		send_handshake();
		
		current_mode=waiting_other_player;//i am the first player to reach "multiplayer"
	  LCD_DrawBoard();
	}
	else{//2nd board to exit menu interface
	LCD_DrawBoard();	
	LCD_DrawInfoPlayer(my_player_id);
	send_start_msg();
	start_game(); //i am the second player to reach "multiplayer"
	}
	}
	else{ //single board selected 
		two_board=0;
		LCD_DrawBoard();
		start_game();
		disable_timer(1); //not needed in single board mode
	}
}

}

void hanshake_failed(){
	
NVIC_DisableIRQ(TIMER0_IRQn);
disable_RIT();
disable_timer(0);
NVIC_DisableIRQ(TIMER2_IRQn);
disable_timer(2);
LCD_DisplayHandshakeFailed();	
current_mode=not_valid_connection;
}



	
/*Used when hovering wall, finds if there are conflicts between wall.
If restore_old_walls flag is set means that the given coordinates are of the old placement, meaning that
the board need to be set to his original state (eventually placing red walls that were there before).

*/
int manage_wall_conflicts(struct Wall wall, uint8_t restore_old_walls,enum Orientation board_walls[6][6])
{
	int conflict_number = 0;
	int i = 0;
	struct Wall conflict[3];

	if (board_walls[wall.square.y][wall.square.x] != undefined_ori)
		conflict[conflict_number++] = initWall(wall.square.x, wall.square.y, board_walls[wall.square.y][wall.square.x]);

	if (wall.orientation == vertical)
	{
		if (wall.square.y < 5 && board_walls[wall.square.y + 1][wall.square.x] == vertical)
			conflict[conflict_number++] = initWall(wall.square.x, wall.square.y + 1, board_walls[wall.square.y + 1][wall.square.x]);
		if (wall.square.y > 0 && board_walls[wall.square.y - 1][wall.square.x] == vertical)
			conflict[conflict_number++] = initWall(wall.square.x, wall.square.y - 1, board_walls[wall.square.y - 1][wall.square.x]);
	}
	else
	{
		if (wall.square.x < 5 && board_walls[wall.square.y][wall.square.x + 1] == horizontal)
			conflict[conflict_number++] = initWall(wall.square.x + 1, wall.square.y, board_walls[wall.square.y][wall.square.x + 1]);
		if (wall.square.x > 0 && board_walls[wall.square.y][wall.square.x - 1] == horizontal)
			conflict[conflict_number++] = initWall(wall.square.x - 1, wall.square.y, board_walls[wall.square.y][wall.square.x - 1]);
	}

	if (restore_old_walls)
	{
		LCD_DrawWall(wall, board_color.background);
		if (conflict_number != 0)
		{
			for (i = 0; i < conflict_number; i++)
			{
				LCD_DrawWall(conflict[i], board_color.wall);
			}
		}
	}

	

	return conflict_number;
}

struct Wall initWall(uint8_t x_square, uint8_t y_square, enum Orientation orientation)
{
	struct Wall tmp;
	tmp.square.x = x_square;
	tmp.square.y = y_square;
	tmp.orientation = orientation;
	return tmp;
}

struct SquareCord initSquareCord(uint8_t x_square, uint8_t y_square)
{
	static struct SquareCord tmp;
	tmp.x = x_square;
	tmp.y = y_square;
	return tmp;
}

void select_square(enum Direction direction)
{
	if (direction != selected_square && adj_squares[direction].x <= 6)
	{
 		if(selected_square!=undefined_dir)
  		LCD_DrawSquare(adj_squares[selected_square], 1, board_color.background);

		selected_square = direction;
		LCD_DrawSquare(adj_squares[selected_square], 1, board_color.hover);
	}
}

void move_player()
{
	if (selected_square != undefined_dir)
	{
		struct SquareCord square = adj_squares[selected_square];
		turn_off_squares_hilight();
		LCD_DrawSquare(player[player_turn].square, 1, board_color.background);
		LCD_DrawPawn(square, player_turn, 0);
		player[player_turn].square.x = square.x;
		player[player_turn].square.y = square.y;

		create_player_move(0,(enum Orientation)0,player[player_turn].square.x,player[player_turn].square.y);
		
		
		//if a player reached end square
		if (player[player_turn].winning_row == player[player_turn].square.y)
		{
			
			if(two_board==1 && my_player_id==player_turn){
				send_msg(my_player_id,player_move);
			  LCD_DisplayVictory(player_turn,1);
				
				}
			else if(two_board==1 && my_player_id!=player_turn){
			  LCD_DisplayVictory(player_turn,0);
				
				}
			else if(two_board==0){
			LCD_DisplayVictory(player_turn,UINT8_MAX);
			
			}
			
			
			NVIC_DisableIRQ(TIMER0_IRQn);
			disable_RIT();
			disable_timer(0);
			current_mode=game_end;
			
		}
		else
			next_turn();
	}
}

/*given a square and a direction will modify adj_square value to contain the adjacent square, return 1 if the adj square is valid in the matrix,
otherwhise 0*/
int get_adiacent_square(struct SquareCord square, enum Direction direction, struct SquareCord *adj_square)
{
	switch (direction)
	{
	case up:
		square.y -= 1;
		break;
	case down:
		square.y += 1;
		break;
	case left:
		square.x -= 1;
		break;
	case right:
		square.x += 1;
		break;

	default:
		break;
	}
	if (square.y <= 6 && square.x <= 6)
	{
		adj_square->y = square.y;
		adj_square->x = square.x;
		return 1;
	}
	else
		return 0;
}

/*checks for validity of a move: check if there is no wall in between 2 positions, IMPORTANT: start_pos and end_pos must be valid values in the matrix,
differing from a single block (horizzontally or vertically)*/
int check_move_if_valid(struct SquareCord start_pos, struct SquareCord end_pos, enum Orientation board_walls[6][6])
{

	if (start_pos.x != end_pos.x)
	{											   /*horizontal movement*/
		start_pos.x = MIN(start_pos.x, end_pos.x); /*assume that start_pos is the square on the left, uses his coord to check for wall*/

		return (start_pos.y == 6 || board_walls[start_pos.y][start_pos.x] != vertical) && (start_pos.y == 0 || board_walls[start_pos.y - 1][start_pos.x] != vertical);
	}
	else if (start_pos.y != end_pos.y)
	{											   /*vertical movement*/
		start_pos.y = MIN(start_pos.y, end_pos.y); /*assume that start_pos is the square on the top, uses his coord to check for wall*/
		return (start_pos.x == 0 || board_walls[start_pos.y][start_pos.x - 1] != horizontal) && (start_pos.x == 6 || board_walls[start_pos.y][start_pos.x] != horizontal);
	}
	else
		return 0;
}

/*checks if a player is on a square*/
int check_square_free(struct SquareCord sel_square, struct Player player_status[2])
{
	if ((player_status[0].square.x == sel_square.x && player_status[0].square.y == sel_square.y) || ((player_status[1].square.x == sel_square.x && player_status[1].square.y == sel_square.y)))
		return 0;
	else
		return 1;
}


void hilight_squares(uint8_t only_add_no_hilight){
struct SquareCord adj[8];	
int i=0;
if(compute_adj(player[player_turn].square,placed_wall,player,adj)>0){// must always be true, otherwise means that player can't move!!
for(i=0;i<8;i++){
adj_squares[i]=adj[i];
	
	
if(adj[i].x!=255 && !only_add_no_hilight)
LCD_DrawSquare(adj[i], 0, board_color.hover_border);


}

}

}

//compute_adj support function
int _compute_adj_support(struct SquareCord start_pos,enum Direction next_step,enum Direction adj_pos,enum Orientation board_walls[6][6],struct SquareCord* result){
	struct SquareCord adj2;
	if (get_adiacent_square(start_pos, next_step, &adj2))
					{
						if (check_move_if_valid(start_pos, adj2,board_walls))
						{
							result[adj_pos] = adj2;
							return 1;
						}
					}
	return 0;
}

/*for every direction find the adj square, check if there is no wall in between, check if there is a player to skip (with check_square_free): if it is not do
	do the same with the next square. If a valid square is found, add it to adj_squares[]*/
int compute_adj(struct SquareCord square,enum Orientation board_walls[6][6],struct Player player_status[2], struct SquareCord* result)
{
	int i = 0;
	struct SquareCord adj;
	struct SquareCord adj2;
	int adj_n=0;
	
	for (i=0; i<8;i++){
	result[i].x = 255;
	result[i].y = 255;		/*not valid values, to later check if value inside is valid*/
	}
	for (i = 0; i < 4; i++)
	{
		if (get_adiacent_square(square, (enum Direction)i, &adj))
		{
			if (check_move_if_valid(square, adj,board_walls))
			{
				if (check_square_free(adj,player_status))
				{
					
					result[i] = adj;
					adj_n++;
				}
				else //square not free
				{
					adj_n=adj_n+_compute_adj_support(adj,(enum Direction)i,(enum Direction)i,board_walls,result);
				
					if(!get_adiacent_square(adj, (enum Direction)i, &adj2) || !check_move_if_valid(adj, adj2,board_walls)){ //wall behind player or over board size
						switch((enum Direction)i){
							case up:
							adj_n=adj_n+_compute_adj_support(adj,right,top_right,board_walls,result);
							adj_n=adj_n+_compute_adj_support(adj,left,top_left,board_walls,result);
							break;
							case down:
							adj_n=adj_n+_compute_adj_support(adj,right,bot_right,board_walls,result);
							adj_n=adj_n+_compute_adj_support(adj,left,bot_left,board_walls,result);
							break;
							case left:
							adj_n=adj_n+_compute_adj_support(adj,up,top_left,board_walls,result);
							adj_n=adj_n+_compute_adj_support(adj,down,bot_left,board_walls,result);
							break;
							case right:
							adj_n=adj_n+_compute_adj_support(adj,up,top_right,board_walls,result);
							adj_n=adj_n+_compute_adj_support(adj,down,bot_right,board_walls,result);
							break;
							default:
								break;
						}
					
					}
					
				}
			}
		}
	}
	return adj_n;
}





void turn_off_squares_hilight()
{
	int i = 0;
	if (selected_square != undefined_dir)
		LCD_DrawSquare(adj_squares[selected_square], 1, board_color.background);
	for (i = 0; i < 8; i++)
	{
		if (adj_squares[i].x <= 6)
		{
			LCD_DrawSquare(adj_squares[i], 0, board_color.square_border);
		}
	}
}

//BFS algorithm to find 

int BFS_search(uint8_t id,enum Orientation board_walls[6][6],struct Player player_status[2],struct SquareCord *first_node)
{
	int i = 0;
	int j=0;
	struct SquareCord tmp[8];
	struct SquareCord next;

	struct SquareCord prev[7][7];
	for(i=0;i<7;i++){
	for(j=0;j<7;j++){
	prev[i][j].x=255;
	prev[i][j].y=255;
	}
	}
	
	reset_queue();
	next = player_status[id].square;
	
	prev[next.y][next.x].x=254;//254==START POSITION
	prev[next.y][next.x].y=254;
	
	
	
	compute_adj(next,board_walls,player_status,tmp);
	for (i = 0; i < 8; i++)
	{
		if(tmp[i].x!=255 && prev[tmp[i].y][tmp[i].x].x==255 ){ //valid square and not yet discovered 
		enqueue(tmp[i]);
			prev[tmp[i].y][tmp[i].x]=next;
		}
	
	}
	
	while (!empty())
	{

		dequeue(&next);
		
			if (player_status[id].winning_row == next.y){
				tmp[0]=next; // re-use tmp as a supp var, using only tmp[0]
				
				i=0;
				while(prev[tmp[0].y][tmp[0].x].x!=254){  //backtrack of path to get next node
					//LCD_DrawSquare(tmp[0],0,Red);
					*first_node=tmp[0];
					tmp[0]=prev[tmp[0].y][tmp[0].x];
				i++;
				}
				return i;
			}
				
			compute_adj(next,board_walls,player_status,tmp);
	for (i = 0; i < 8; i++)
	{
		if(tmp[i].x!=255 && prev[tmp[i].y][tmp[i].x].x==255 ){ //valid square and not yet discovered 
		enqueue(tmp[i]);
			prev[tmp[i].y][tmp[i].x]=next;
		}
	
	}
			
	}

	return 0;
}














