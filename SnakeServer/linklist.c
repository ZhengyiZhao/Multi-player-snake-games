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

#include <stdlib.h>
#include <time.h>

#include "linklist.h"
#include "stdio.h"
#include "Snake.h"

/**------------------------
 * Name: Snake Implementation
 * Purpose:
 * Date of Creating:
 * Notes
 *-------------------------**/
// 18032006
static PLANARY_SNAKE_MOVING_STEP stSnakeMoveDir[__SNK_MAX_FEASIBLE_DIR__]
= {{1, 0}, {0, -1}, {-1,0}, {0, 1}};
static unsigned short usActualTotalSnake;
// 18032006
static LIST_NODE  lstNode[__SNK_MAX_PLAYERS__][__SNK_MAX_SNAKE_LEN__];

unsigned short snk_get_total_snake_number()
{
	return usActualTotalSnake;
}

void snk_total_snake_number_increase_one()
{
       usActualTotalSnake = usActualTotalSnake + 1;
}

void snk_total_snake_number_decrease_one()
{
       usActualTotalSnake = usActualTotalSnake - 1;
}

POINTER_NODE snk_create_link_list(unsigned short usId, unsigned short usMaxNode, short sStartX, short sStartY, short sDir)
{
	unsigned short ii;
	POINTER_NODE pHead, pCurr, pNext;


	if(usMaxNode == 0 || usId >= __SNK_MAX_PLAYERS__ || usMaxNode >= __SNK_MAX_SNAKE_LEN__)
	{
		return NULL;
	}
	pHead = &lstNode[usId][0];
	pHead ->sPosnX = sStartX;
	pHead ->sPosnY = sStartY;
	pHead ->pNextNode = NULL;
	pCurr = pHead;

	for(ii=1; ii< usMaxNode; ii++)
	{
		pNext = &lstNode[usId][ii];
		pCurr ->pNextNode = pNext;
#ifndef __UNI_DIRECTION_LIST__
		pNext ->pPrevNode = pCurr;
#endif
		if(sDir == __MOVING_DIR_X_POS__)
		{
			pNext ->sPosnX = pCurr ->sPosnX - 1;
			pNext ->sPosnY = pCurr ->sPosnY;
		}
		else if(sDir == __MOVING_DIR_X_NEG__)
		{
			pNext ->sPosnX = pCurr ->sPosnX + 1;
			pNext ->sPosnY = pCurr ->sPosnY;
		}
		else if(sDir == __MOVING_DIR_Y_POS__)
		{
			pNext ->sPosnX = pCurr ->sPosnX;
			pNext ->sPosnY = pCurr ->sPosnY - stSnakeMoveDir[__MOVING_DIR_Y_POS__].sStepY;
		}
		else if(sDir == __MOVING_DIR_Y_NEG__)
		{
			pNext ->sPosnX = pCurr ->sPosnX;
			pNext ->sPosnY = pCurr ->sPosnY - stSnakeMoveDir[__MOVING_DIR_Y_NEG__].sStepY;
		}
		else
		{
		}

		pCurr = pNext;
#ifdef __UNI_DIRECTION_LIST__
		pNext ->pNextNode = NULL;
#else
		pNext ->pNextNode = pHead;
#endif
	}

#ifndef __UNI_DIRECTION_LIST__
	pHead->pPrevNode = pNext;
#endif
	return pHead;
}

short snk_create_snake(unsigned short usId, unsigned short usMaxNode, short sStartX, short sStartY, short sDir, SNAKE_LINK *pstSnake)
{
	POINTER_NODE pHead;
	pHead = snk_create_link_list(usId, usMaxNode, sStartX, sStartY, sDir);
	if(pHead == NULL)
	{
		return SNK_ERR;
	}
	else
	{
		pstSnake->pSnakeHead = pHead;
		pstSnake->usLength = usMaxNode;
		pstSnake->ucSnakeScore = 0;
		pstSnake->usId = usId;
		pstSnake->sMovingDir = sDir;
//		pstSnake->usFlagAlive = SNAKE_JOIN_GAME;   // When game just begin register, snake is create, 
		                                           // not yet SNAKE_ALIVE;
                pstSnake->usFlagAlive = SNAKE_DEAD;
		return SNK_OK;
	}
}

short snk_output_file(char *sFilename, SNAKE_LINK *pstSnake)
{
	unsigned short ii;
	POINTER_NODE pCurr;
	FILE *fptr;

	fptr = fopen(sFilename, "w");
	if(fptr != NULL)
	{
		fprintf(fptr, "Snake id: %d\n", pstSnake->usId);
		fprintf(fptr, "Snake alive: %d\n", pstSnake->usFlagAlive);
		fprintf(fptr, "Snake length: %d\n", pstSnake->usLength);
		fprintf(fptr, "Snake moving dir: %d\n", pstSnake->sMovingDir);
		fprintf(fptr, "Snake Current Score: %d\n", pstSnake->ucSnakeScore);
		fprintf(fptr, "Snake node list: <x, y>\n");
		pCurr = pstSnake->pSnakeHead;
		
		for(ii=0; ii < pstSnake->usLength; ii ++)
		{
			fprintf(fptr, "<%d,%d>\n", pCurr->sPosnX, pCurr->sPosnY);
			pCurr = pCurr->pNextNode;
		}
		fclose(fptr);
	}
	return SNK_OK;
}
//18032006
short snk_move_one_unit(SNAKE_LINK *pstSnake)
{
POINTER_NODE pCurr, pNext;
#ifdef __UNI_DIRECTION_LIST__
short ii;
#endif

    if(pstSnake->usFlagAlive == SNAKE_ALIVE)
	{
#ifdef __UNI_DIRECTION_LIST__
		// go to the end of the snake first
		// then assign the locaiton of the previous unit to current unit
		pCurr = pstSnake->pSnakeHead;
		pNext = pCurr->pNextNode;

		for(ii = 0; ii<pstSnake->usLength - 1; ii++)
		{
			pNext->sPosnX = pCurr->sPosnX;
			pNext->sPosnY = pCurr->sPosnY;
			pCurr = pNext;
			pNext = pCurr->pNextNode;
		}
		pstSnake->pSnakeHead->sPosnX = pstSnake->pSnakeHead->sPosnX + 
			stSnakeMoveDir[pstSnake->sMovingDir].sStepX;
		pstSnake->pSnakeHead->sPosnY = pstSnake->pSnakeHead->sPosnY +
			stSnakeMoveDir[pstSnake->sMovingDir].sStepY;
#else  // __DUAL_DIRECTION_CYCLE_LIST__

		pNext = pstSnake->pSnakeHead;
		pCurr = pNext->pPrevNode;
		pCurr->sPosnX = pNext->sPosnX + 
			stSnakeMoveDir[pstSnake->sMovingDir].sStepX;
		pCurr->sPosnY = pNext->sPosnY +
			stSnakeMoveDir[pstSnake->sMovingDir].sStepY;


		pstSnake->pSnakeHead = pCurr;

#endif
		
		return SNK_OK;
	}
	else
	{
		return SNK_ERR;
		
	}
}

//18032006
short snk_add_one_unit(SNAKE_LINK *pstSnake)
{
POINTER_NODE pCurr, pNext;
#ifdef __UNI_DIRECTION_LIST__
short ii;
#endif

    if(pstSnake->usFlagAlive == SNAKE_ALIVE && pstSnake->usLength <__SNK_MAX_SNAKE_LEN__ - 1)
	{
#ifdef __UNI_DIRECTION_LIST__
		// go to the end of the snake first
		// then assign the locaiton of the previous unit to current unit
#else  // __DUAL_DIRECTION_CYCLE_LIST__

		pNext = pstSnake->pSnakeHead;
		pCurr = &lstNode[pstSnake->usId][pstSnake->usLength];
		pCurr->sPosnX = pNext->sPosnX + 
			stSnakeMoveDir[pstSnake->sMovingDir].sStepX;
		pCurr->sPosnY = pNext->sPosnY +
			stSnakeMoveDir[pstSnake->sMovingDir].sStepY;

		pNext->pPrevNode->pNextNode = pCurr;
		pCurr->pNextNode = pNext;
		pCurr->pPrevNode = pNext->pPrevNode;
		pNext->pPrevNode = pCurr;

		pstSnake->pSnakeHead = pCurr;
		pstSnake->usLength =pstSnake->usLength + 1;

#endif
		
		return SNK_OK;
	}
	else
	{
		return SNK_ERR;
		
	}
}

//18032006
short snk_change_dir(SNAKE_LINK *pstSnake, unsigned short usNewDir)
{
	if(usNewDir < __SNK_MAX_FEASIBLE_DIR__)
	{
#ifndef  __NOT_ALLOW_REVERSE__
		if(pstSnake->sMovingDir == __MOVING_DIR_X_POS__ && usNewDir == __MOVING_DIR_X_NEG__
			|| pstSnake->sMovingDir == __MOVING_DIR_X_NEG__ && usNewDir == __MOVING_DIR_X_POS__
			|| pstSnake->sMovingDir == __MOVING_DIR_Y_POS__ && usNewDir == __MOVING_DIR_Y_NEG__
			|| pstSnake->sMovingDir == __MOVING_DIR_Y_NEG__ && usNewDir == __MOVING_DIR_Y_POS__
			)
		{
			return SNK_ERR;
		}
#endif
		pstSnake->sMovingDir = usNewDir;
		return SNK_OK;
	}
	else
	{
		return SNK_ERR;
	}
}

SNAKE_LINK lstSnake[__SNK_MAX_PLAYERS__];
SNK_APPLE_INFO  gstSingleAppleInSnakeGame;

void move_snake()
{
short ii;
	for(ii=0; ii<__SNK_MAX_PLAYERS__; ii++)
	{
		snk_move_one_unit(&lstSnake[ii]);
	}
}
#define  __SNK_0_INIT_POSN_X__   40
#define  __SNK_1_INIT_POSN_X__   40
#define  __SNK_2_INIT_POSN_X__   40
#define  __SNK_3_INIT_POSN_X__   40
#define  __SNK_0_INIT_POSN_Y__   10
#define  __SNK_1_INIT_POSN_Y__   20
#define  __SNK_2_INIT_POSN_Y__   30
#define  __SNK_3_INIT_POSN_Y__   40

#define __SNAKE_LEVEL__
//#define __SNAKE_CROSS__
#ifdef __SNAKE_CROSS__
void init_snake()
{
	snk_create_snake(0, __SNK_INIT_LEN__, __SNK_2_INIT_POSN_X__, __SNK_2_INIT_POSN_Y__, __MOVING_DIR_Y_NEG__, &lstSnake[0]);
	snk_create_snake(1, __SNK_INIT_LEN__, __SNK_3_INIT_POSN_X__, __SNK_3_INIT_POSN_Y__, __MOVING_DIR_Y_POS__, &lstSnake[1]);
	snk_create_snake(2, __SNK_INIT_LEN__, __SNK_1_INIT_POSN_X__, __SNK_1_INIT_POSN_Y__, __MOVING_DIR_X_NEG__, &lstSnake[2]);
	snk_create_snake(3, __SNK_INIT_LEN__, __SNK_0_INIT_POSN_X__, __SNK_0_INIT_POSN_Y__, __MOVING_DIR_X_POS__, &lstSnake[3]);
}
#endif
#ifdef __SNAKE_LEVEL__
void init_snake()
{
	snk_create_snake(0, __SNK_INIT_LEN__, __SNK_0_INIT_POSN_X__, __SNK_0_INIT_POSN_Y__, __MOVING_DIR_X_POS__, &lstSnake[0]);
	snk_create_snake(1, __SNK_INIT_LEN__, __SNK_1_INIT_POSN_X__, __SNK_1_INIT_POSN_Y__, __MOVING_DIR_X_POS__, &lstSnake[1]);
	snk_create_snake(2, __SNK_INIT_LEN__, __SNK_2_INIT_POSN_X__, __SNK_2_INIT_POSN_Y__, __MOVING_DIR_X_POS__, &lstSnake[2]);
	snk_create_snake(3, __SNK_INIT_LEN__, __SNK_3_INIT_POSN_X__, __SNK_3_INIT_POSN_Y__, __MOVING_DIR_X_POS__, &lstSnake[3]);
}
#endif


void debug_snake()
{
	snk_output_file("snk0.txt", &lstSnake[0]);
	snk_output_file("snk1.txt", &lstSnake[1]);
	snk_output_file("snk2.txt", &lstSnake[2]);
	snk_output_file("snk3.txt", &lstSnake[3]);

	snk_move_one_unit(&lstSnake[0]);
	snk_move_one_unit(&lstSnake[1]);
	snk_move_one_unit(&lstSnake[2]);
	snk_move_one_unit(&lstSnake[3]);

	snk_output_file("snk0_1.txt", &lstSnake[0]);
	snk_output_file("snk1_1.txt", &lstSnake[1]);
	snk_output_file("snk2_1.txt", &lstSnake[2]);
	snk_output_file("snk3_1.txt", &lstSnake[3]);

	snk_move_one_unit(&lstSnake[0]);
	snk_output_file("snk0_2.txt", &lstSnake[0]);

	snk_add_one_unit(&lstSnake[0]);
	snk_output_file("snk0_3.txt", &lstSnake[0]);
	snk_move_one_unit(&lstSnake[0]);
	snk_output_file("snk0_4.txt", &lstSnake[0]);

	snk_change_dir(&lstSnake[0], __MOVING_DIR_Y_POS__);

	snk_move_one_unit(&lstSnake[0]);
	snk_output_file("snk0_5.txt", &lstSnake[0]);

	printf("tested move, add, change dir operations!\n");


}
extern short gsGameStarted;
void snk_server_broadcast_game_over();

void snake_update_alive_flag()
{
	unsigned short ii, jj, kk, usTotalSnake;
	short sTargetSnakeHead_x, sTargetSnakeHead_y;
	POINTER_NODE pTargetSnake, pCurrSnake;
     // For each target_snake,
	for(ii=0; ii<__SNK_MAX_PLAYERS__; ii++)
	{
     //   if target_snake dead
     //       continue to next snake
     //   end
		if(lstSnake[ii].usFlagAlive == SNAKE_ALIVE)
		{
			pTargetSnake = lstSnake[ii].pSnakeHead;
			sTargetSnakeHead_x = pTargetSnake->sPosnX;
			sTargetSnakeHead_y = pTargetSnake->sPosnY;

			if(sTargetSnakeHead_x >= __SNK_MAX_POSN_X__ || 
				sTargetSnakeHead_x <= 0 ||
				sTargetSnakeHead_y >= __SNK_MAX_POSN_Y__ ||
				sTargetSnakeHead_y <= 0)
			{
				lstSnake[ii].usFlagAlive = SNAKE_DEAD;
                                snk_total_snake_number_decrease_one();
                                printf("\n Snake %d Dead!\n", ii);
			}
			for(jj=0; jj< __SNK_MAX_PLAYERS__; jj++)
			{
				pCurrSnake = lstSnake[jj].pSnakeHead;
				if(lstSnake[jj].usFlagAlive == SNAKE_ALIVE)
				/* 
					|| lstSnake[jj].usFlagAlive == SNAKE_JOIN_GAME)*/
				{
					for(kk=0; kk<lstSnake[jj].usLength; kk++)
					{
						if(ii == jj && kk ==0)
						{
						}
						else
						{
							if((pCurrSnake->sPosnX == sTargetSnakeHead_x) &&
								(pCurrSnake->sPosnY == sTargetSnakeHead_y))
							{
								lstSnake[ii].usFlagAlive = SNAKE_JOIN_GAME;
							}
						}
						pCurrSnake = pCurrSnake->pNextNode;
					}
				}
			}
			
		}
		else if(lstSnake[ii].usFlagAlive == SNAKE_JOIN_GAME)
		{
			lstSnake[ii].usFlagAlive = SNAKE_DEAD;
                        snk_total_snake_number_decrease_one();

                        printf("\n Snake %d Dead!\n", ii);
		}
     //   get the head position, target_snake_head_x, target_snake_head_y
     //   for each snake
     //       if snake alive
     //           for each node
     //               if(target_snake == each snake && node == head)
     //                  no action
     //               else
     //                  if(node.x == target_snake_head_x && node.y == target_snake_head_y)
     //                      snake.aliveFlag = dead
     //                  end
     //               end
     //           loop // node of each snake
     //       end // snake is alive
     //   loop  // all snake
     // loop // each target_snake
	}// loop // each target_snake

             usTotalSnake = snk_get_total_snake_number();
              if(usTotalSnake == 0)
              { // Broadcast game over command
                   gsGameStarted= 0;
                   snk_server_broadcast_game_over();
              }

}

void snake_update_one_snk_alive_flag(unsigned short usSnakeId)
{
	unsigned short jj, kk, usTotalSnake;
	short sTargetSnakeHead_x, sTargetSnakeHead_y;
	POINTER_NODE pTargetSnake, pCurrSnake;
     // For each target_snake,
     //   if target_snake dead
     //       continue to next snake
     //   end
	if(usSnakeId < __SNK_MAX_PLAYERS__)
	{
		if(lstSnake[usSnakeId].usFlagAlive == SNAKE_ALIVE)
		{
			pTargetSnake = lstSnake[usSnakeId].pSnakeHead;
			sTargetSnakeHead_x = pTargetSnake->sPosnX;
			sTargetSnakeHead_y = pTargetSnake->sPosnY;

			if(sTargetSnakeHead_x >= __SNK_MAX_POSN_X__ || 
				sTargetSnakeHead_x <= 0 ||
				sTargetSnakeHead_y >= __SNK_MAX_POSN_Y__ ||
				sTargetSnakeHead_y <= 0)
			{
				lstSnake[usSnakeId].usFlagAlive = SNAKE_DEAD;
                                snk_total_snake_number_decrease_one();
                                printf("\n Snake %d Dead!\n", usSnakeId);
			}
			for(jj=0; jj< __SNK_MAX_PLAYERS__; jj++)
			{
				pCurrSnake = lstSnake[jj].pSnakeHead;
				if(lstSnake[jj].usFlagAlive == SNAKE_ALIVE)
				/* 
					|| lstSnake[jj].usFlagAlive == SNAKE_JOIN_GAME)*/
				{
					for(kk=0; kk<lstSnake[jj].usLength; kk++)
					{
						if(usSnakeId == jj && kk ==0)
						{
						}
						else
						{
							if((pCurrSnake->sPosnX == sTargetSnakeHead_x) &&
								(pCurrSnake->sPosnY == sTargetSnakeHead_y))
							{
								lstSnake[usSnakeId].usFlagAlive = SNAKE_JOIN_GAME;
							}
						}
						pCurrSnake = pCurrSnake->pNextNode;
					}
				}
			}
			
		}
		else if(lstSnake[usSnakeId].usFlagAlive == SNAKE_JOIN_GAME)
		{
			lstSnake[usSnakeId].usFlagAlive = SNAKE_DEAD;
                        snk_total_snake_number_decrease_one();

                        printf("\n Snake %d Dead!\n", usSnakeId);
		}
     //   get the head position, target_snake_head_x, target_snake_head_y
     //   for each snake
     //       if snake alive
     //           for each node
     //               if(target_snake == each snake && node == head)
     //                  no action
     //               else
     //                  if(node.x == target_snake_head_x && node.y == target_snake_head_y)
     //                      snake.aliveFlag = dead
     //                  end
     //               end
     //           loop // node of each snake
     //       end // snake is alive
     //   loop  // all snake
     // loop // each target_snake

             usTotalSnake = snk_get_total_snake_number();
              if(usTotalSnake == 0)
              { // Broadcast game over command
                   gsGameStarted= 0;
                   snk_server_broadcast_game_over();
              }

    }
}

void snake_update_score()
{
short ii;

	for(ii=0; ii<__SNK_MAX_PLAYERS__; ii++)
	{
		if(lstSnake[ii].usFlagAlive == SNAKE_ALIVE)
		{
			if(lstSnake[ii].pSnakeHead->sPosnX == gstSingleAppleInSnakeGame.usPosnX &&
				lstSnake[ii].pSnakeHead->sPosnY == gstSingleAppleInSnakeGame.usPosnY )
			{
				lstSnake[ii].ucSnakeScore = lstSnake[ii].ucSnakeScore + 1;
				gstSingleAppleInSnakeGame.sAppleStatus = __SNK_APPLE_NONE__;
snk_add_one_unit(&lstSnake[ii]);
			}

		}
	}

}

void snake_update_one_snk_score(unsigned short usSnakeId)
{
	if(usSnakeId < __SNK_MAX_PLAYERS__)
	{
		if(lstSnake[usSnakeId].usFlagAlive == SNAKE_ALIVE)
		{
			if(lstSnake[usSnakeId].pSnakeHead->sPosnX == gstSingleAppleInSnakeGame.usPosnX &&
				lstSnake[usSnakeId].pSnakeHead->sPosnY == gstSingleAppleInSnakeGame.usPosnY )
			{
				lstSnake[usSnakeId].ucSnakeScore = lstSnake[usSnakeId].ucSnakeScore + 1;
				gstSingleAppleInSnakeGame.sAppleStatus = __SNK_APPLE_NONE__;
				snk_add_one_unit(&lstSnake[usSnakeId]);
			}

		}
	}

}

unsigned short snk_generate_fruit(unsigned short maxval)
{
unsigned short usPosn;
    /* Set seed (initial seed) */
//    srand( (unsigned)time( NULL ) );
//    return (short)((int) maxval * rand() / (RAND_MAX + 1.0));
      srand(time(NULL));
      usPosn = (unsigned short)rand()% maxval;
      if(usPosn >= maxval)
      {
         usPosn = maxval - 1;
      }
      else if (usPosn == 0)
      {
         usPosn = 1;
      }
	 
      return (usPosn);
}

void snk_init_apple()
{
short ii, jj;
POINTER_NODE pCurrSnake;
	
	if(gstSingleAppleInSnakeGame.sAppleStatus == __SNK_APPLE_NONE__)
	{// If there is no apple in the field, an apple will be generated
#ifndef __PRIME_GEN__
		gstSingleAppleInSnakeGame.usPosnX = snk_generate_fruit((unsigned short)__SNK_MAX_POSN_X__) ;
		gstSingleAppleInSnakeGame.usPosnY = snk_generate_fruit((unsigned short)__SNK_MAX_POSN_Y__) ;
		
#else
                gstSingleAppleInSnakeGame.usPosnX += 37;
                gstSingleAppleInSnakeGame.usPosnY += 23;
                gstSingleAppleInSnakeGame.usPosnX = (gstSingleAppleInSnakeGame.usPosnX % __SNK_MAX_POSN_X__);
                gstSingleAppleInSnakeGame.usPosnY = (gstSingleAppleInSnakeGame.usPosnY % __SNK_MAX_POSN_Y__);
#endif
		gstSingleAppleInSnakeGame.sAppleStatus = __SNK_APPLE_EXIST__;


		for(ii=0; ii<__SNK_MAX_PLAYERS__; ii++)
		{
			if(lstSnake[ii].usFlagAlive == SNAKE_JOIN_GAME ||
				lstSnake[ii].usFlagAlive == SNAKE_READY_START ||
				lstSnake[ii].usFlagAlive == SNAKE_ALIVE)
			{
				pCurrSnake = lstSnake[ii].pSnakeHead;

				for(jj=0; jj<lstSnake[ii].usLength ; jj++)
				{
					if(gstSingleAppleInSnakeGame.usPosnX == pCurrSnake->sPosnX &&
						gstSingleAppleInSnakeGame.usPosnY == pCurrSnake->sPosnY )
					{
						gstSingleAppleInSnakeGame.sAppleStatus = __SNK_APPLE_NONE__;
						break; // Fail to generate a new apple
					}
					pCurrSnake = pCurrSnake->pNextNode;
				}
			}
			if(gstSingleAppleInSnakeGame.sAppleStatus == __SNK_APPLE_NONE__)
			{
				break; // Fail to generate a new apple
			}
		}
	}
	if(gstSingleAppleInSnakeGame.sAppleStatus == __SNK_APPLE_EXIST__)
	{
		gstSingleAppleInSnakeGame.sAppleId = gstSingleAppleInSnakeGame.sAppleId +1;
	}
}

void snake_update_apple()
{
	snk_init_apple();
}

void snk_init_wall()
{
}
