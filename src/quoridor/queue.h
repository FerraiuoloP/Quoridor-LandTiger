#include "quoridor.h"  
#define MAX_SIZE 200  
void enqueue(struct SquareCord element);
int dequeue(struct SquareCord *element);
int empty(void);
void reset_queue(void);
