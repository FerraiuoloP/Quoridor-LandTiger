#include "queue.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
struct SquareCord queue[MAX_SIZE];
int front = -1;
int rear = -1;

void enqueue(struct SquareCord element)
{
    if (rear == MAX_SIZE - 1)
    {
        return;
    }
    if (front == -1)
    {
        front = 0;
    }
    rear++;
    queue[rear] = element;
}

int dequeue(struct SquareCord *element)
{
    if (empty())
    {

        return 0;
    }
    *element = queue[front];
    front++;
    return 1;
}

int empty()
{
    if (front == -1 || front > rear)
    {

        return 1;
    }

    else
        return 0;
}

void reset_queue()
{
    front = -1;
    rear = -1;
    memset(queue, 0, MAX_SIZE);
}
