
#ifndef QUORIDOR_H_
#define QUORIDOR_H_ 
#include "LPC17xx.h"
#define MIN(X, Y) (((X) < (Y)) ? (X) : (Y))
#define MAX(X, Y) (((X) > (Y)) ? (X) : (Y))
enum Orientation{
vertical,
horizontal,
undefined_ori
};

enum Direction{
up,
down,
right,
left,
top_right,
top_left,
bot_right,
bot_left,	
undefined_dir
};

enum Mode{
move,
wall,
main_menu,
single_board_menu,
two_board_menu,
waiting,
waiting_other_player,
not_valid_connection,
game_end
};


struct SquareCord{
uint8_t x;
uint8_t y;
};

struct Player{
uint8_t walls;
uint8_t winning_row;
struct SquareCord square;
};

struct Wall{
enum Orientation orientation;	
struct SquareCord square;
};




int minmax(uint8_t player_id,enum Orientation board_walls[6][6],struct Player player_status[2],int depth,int alpha, int beta,int *best_val_index, struct Wall *best_wall_coord);
void init_quoridor(void);
void switch_mode(void);
void hover_wall(uint8_t x_square,uint8_t y_square);
void rotate_hover_wall(void);
void start_game(void);
void place_wall(void);
void next_turn(void);
int manage_wall_conflicts(struct Wall wall,uint8_t restore_old_walls,enum Orientation board_walls[6][6]); 
struct Wall initWall(uint8_t x_square,uint8_t y_square,enum Orientation orientation);
void move_player(void);
void select_square(enum Direction direction);
void get_movements(struct SquareCord, struct SquareCord*);
void hilight_squares(uint8_t only_add_no_hilight);
int compute_adj(struct SquareCord square,enum Orientation board_walls[6][6],struct Player player_status[2], struct SquareCord* result);
int get_adiacent_square (struct SquareCord square, enum Direction direction, struct SquareCord* adj_square);
int check_move_if_valid(struct SquareCord start_pos, struct SquareCord end_pos, enum Orientation board_walls[6][6]);
void turn_off_squares_hilight(void);
int BFS_search(uint8_t id,enum Orientation board_walls[6][6],struct Player player_status[2],struct SquareCord *first_node);
void menu_select(enum Direction dir);
void menu_confirm(void);
void make_move_ai(void);	
void display_menu(void);
void send_msg(int id, uint32_t msg);
void recived_msg(uint32_t message_recived);
void hanshake_failed(void);
void skip_turn(void);
void send_msg_skip_turn(void);
#endif


