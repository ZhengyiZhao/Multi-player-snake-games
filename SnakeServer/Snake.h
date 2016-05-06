
#ifndef __SNAKE_H__
#define __SNAKE_H__

#define SUCCESS 0
#define ERROR   1

#define SNK_OK    0
#define SNK_ERR   1

#include "linklist.h"

extern short gsDebugLevel; // defined in SnkComm.c
extern SNAKE_LINK lstSnake[]; // declared in linklist.c

extern void move_snake();
extern void init_snake();
extern void debug_snake();
extern void snake_update_alive_flag();

extern void snake_update_score();
extern void snake_update_apple();

extern void snake_rtdi_rtug();

#endif

