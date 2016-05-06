/***************************************************************************
 *   Copyright (C) 2006 by Zhao ZhengYi                                    *
 *   zzytgx@yahoo.com.cn                                                   *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/

 #ifndef __LINK_LIST_H__
#define __LINK_LIST_H__

#define SNAKE_ALIVE 1
#define SNAKE_DEAD  0
#define SNAKE_JOIN_GAME      2
#define SNAKE_READY_START    3
#define SNAKE_GIVE_UP_GAME   4
#define SNAKE_ABORT_GAME     5

typedef struct _LIST_NODE * POINTER_NODE;

typedef struct _LIST_NODE
{
	short sPosnX;
	short sPosnY;
	POINTER_NODE pNextNode;
	POINTER_NODE pPrevNode;
}LIST_NODE;

#define __SNK_GENERAL_SNAKE_INFO__  5

typedef struct _SNAKE_LINK  //18032006
{
	unsigned short usLength;
	POINTER_NODE pSnakeHead;
	unsigned short usId;
	unsigned char ucSnakeScore;
	short sMovingDir;
	unsigned short usFlagAlive;
}SNAKE_LINK;

#define __SNK_MAX_SNAKE_LEN__   400
#define __SNK_MAX_POSN_X__		70
#define __SNK_MAX_POSN_Y__		50
#define __SNK_MAX_PLAYERS__		3
#define __SNK_INIT_LEN__		15
#define __SNK_INIT_MOVING_SPEED__		5


#ifdef DEFINE_MOVING_DIR_BY_BIT
// Following definition is to make use of the 
#define  __MOVING_DIR_X_POS__     1
#define  __MOVING_DIR_Y_POS__     2
#define  __MOVING_DIR_X_NEG__     (-2)
#define  __MOVING_DIR_Y_NEG__     (-3)

#else
#define  __MOVING_DIR_X_POS__     0
#define  __MOVING_DIR_Y_POS__     1
#define  __MOVING_DIR_X_NEG__     2
#define  __MOVING_DIR_Y_NEG__     3

#endif


// Creat a link-list,
// Input:
//   unsigned short usMaxNode: Maximum number of nodes in the snake
//   short sStartX: Start Position (Head position x of the snake)
//   short sStartY: Start Position (Head position y of the snake)
//   short iDir: 1: x-positive dir
//               2: y-positive dir
//               -2: x-negative dir
//               -3: y-negative dir
extern POINTER_NODE snk_create_link_list(unsigned short usId, unsigned short usMaxNode, short sStartX, short sStartY, short iDir);
extern short snk_create_snake(unsigned short usId, unsigned short usMaxNode, short sStartX, short sStartY, short iDir, SNAKE_LINK *pstSnake);
extern short snk_output_file(char *sFilename, SNAKE_LINK *pstSnake);
extern short snk_append_file(char *sFilename, SNAKE_LINK *pstSnake);


// Motion of a snake implemented by each unit step
#define __SNK_MAX_FEASIBLE_DIR__    4
typedef struct _PLANARY_SNAKE_MOVING_STEP
{
	short sStepX;
	short sStepY;
}PLANARY_SNAKE_MOVING_STEP;

extern short snk_move_one_unit(SNAKE_LINK *pstSnake);
extern short snk_add_one_unit(SNAKE_LINK *pstSnake);
extern short snk_change_dir(SNAKE_LINK *pstSnake, unsigned short usNewDir);
extern void snk_total_snake_number_decrease_one();
extern void snk_total_snake_number_increase_one();
extern void snake_update_one_snk_alive_flag(unsigned short usSnakeId);
extern void snake_update_one_snk_score(unsigned short usSnakeId);


#define __SNK_SCORE_PENALTY_ABORT_GAME__   -10

extern void move_snake();
extern void init_snake();
extern void snake_rtdi_rtug();

//// stucture defination for apple
#define __SNK_APPLE_NONE__    0
#define __SNK_APPLE_EXIST__   1

typedef struct
{
	unsigned short usPosnX;
	unsigned short usPosnY;
	short sAppleId;
	short sAppleStatus;

}SNK_APPLE_INFO;

extern SNK_APPLE_INFO gstSingleAppleInSnakeGame;

void snk_init_apple();

//// structure defination for Wall

void snk_init_wall();
#endif
