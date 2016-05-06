
#include <stdio.h>

#include "linklist.h"
#include "SnkComm.h"
#include "Snake.h"
#include "TcpUdp.h"

extern SNAKE_LINK lstSnake[];
extern snk_get_total_snake_number();

short gsDebugLevel = 0;

char strOnePack[__SNK_COMMU_MAXIMUM_DATA_SIZE__];
char strDataPackPlanarySnakeInfo[2* __SNK_MAX_PLAYERS__ *__SNK_MAX_SNAKE_LEN__ + __SNK_GENERAL_SNAKE_INFO__ * __SNK_MAX_PLAYERS__]; 
	/// Each snake information includes: SnakeId, SnakeScore, SnakeAlive, SnakeLen,  SnakeMoveDir,
    /// SnakeHead->x, SnakeHead->y, SnakeHead->pNext->x, SnakeHead->pNext->y, ...
    /// should be 2* __SNK_MAX_SNAKE_LEN__ + __SNK_GENERAL_SNAKE_INFO__
    /// Totally for __SNK_MAX_PLAYERS__ snakes, there will be 
    ///   __SNK_MAX_PLAYERS__  * ( 2* __SNK_MAX_SNAKE_LEN__ + __SNK_GENERAL_SNAKE_INFO__ )
char strDataPackPlanaryAppleInfo[2 + 1];

/**------------------------
 * Name:   snk_server_package_snake_info
 * Purpose:
 * Date of Creating:
 * Notes
 *-------------------------**/
short snk_server_package_snake_info(unsigned short *pusTotalByte)
{
short ii, jj, sCurrIndex=0;
short sCurrSnakeLen;
LIST_NODE *pCurrNode;

    for(ii=0; ii< __SNK_MAX_PLAYERS__; ii++)
	{
                if(lstSnake[ii].usFlagAlive == SNAKE_ALIVE)
         {
		sCurrSnakeLen = lstSnake[ii].usLength;
		pCurrNode = lstSnake[ii].pSnakeHead;

		strDataPackPlanarySnakeInfo[sCurrIndex ++] = (char)lstSnake[ii].usId;
        strDataPackPlanarySnakeInfo[sCurrIndex ++] = (char)lstSnake[ii].ucSnakeScore ;
		strDataPackPlanarySnakeInfo[sCurrIndex ++] = (char)lstSnake[ii].usFlagAlive;
		strDataPackPlanarySnakeInfo[sCurrIndex ++] = (char)lstSnake[ii].usLength;
		strDataPackPlanarySnakeInfo[sCurrIndex ++] = (char)lstSnake[ii].sMovingDir;

		for(jj=0; jj<sCurrSnakeLen; jj++)
		{
			strDataPackPlanarySnakeInfo[sCurrIndex ++] = (char)pCurrNode->sPosnX;
			strDataPackPlanarySnakeInfo[sCurrIndex ++] = (char)pCurrNode->sPosnY;
			pCurrNode = pCurrNode->pNextNode;
		}

          }
	}

	(*pusTotalByte) = sCurrIndex;

	return SNK_OK;
}

/**------------------------
 * Name:
 * Purpose:
 * Date of Creating:
 * Notes
 *-------------------------**/
short snk_server_package_app_info(unsigned short *pusTotalByte)
{
	strDataPackPlanaryAppleInfo[0] = (char)gstSingleAppleInSnakeGame.sAppleStatus;
	strDataPackPlanaryAppleInfo[1] = (char)gstSingleAppleInSnakeGame.usPosnX;
	strDataPackPlanaryAppleInfo[2] = (char)gstSingleAppleInSnakeGame.usPosnY;

	(*pusTotalByte) = 3;

	return SNK_OK;
}

/**------------------------
 * Name:
 * Purpose:
 * Date of Creating:
 * Notes
 *-------------------------**/
short snk_server_package_data(char cCmdType, char cCmdCode, char cDataLen, const char *strData)
{
short ii, sChkSum=0;
    
    strOnePack[__SNK_COMM_ONE_PACK_OFST_SENDER_ID__] = __SNK_COMM_DATA_DEF_SERVER_ID__;
	strOnePack[__SNK_COMM_ONE_PACK_OFST_RECVER_ID__] = 0; /// Should broadcase to all the clients, this data is meaningless
	strOnePack[__SNK_COMM_ONE_PACK_OFST_CMD_TYPE__] = cCmdType;
	strOnePack[__SNK_COMM_ONE_PACK_OFST_CMD_CODE__] = cCmdCode;
	strOnePack[__SNK_COMM_ONE_PACK_OFST_DATA_LEN__] = cDataLen;
	strOnePack[__SNK_COMM_ONE_PACK_OFST_RESERVE__] = (char)snk_get_total_snake_number();

	if(cCmdType == __SNK_COMM_DATA_DEF_SNAKE_STATUS_CMD_SRV_2_CLT__ || 
		cCmdType == __SNK_COMM_DATA_DEF_APPLE_STATUS_CMD_SRV_2_CLT__)
	{
		for(ii=0; ii<cDataLen; ii++)
		{
			strOnePack[__SNK_COMM_ONE_PACK_OFST_DATA_START__ + ii] = strData[ii];
		}
	}

	sChkSum = cCmdType + cCmdCode + cDataLen;
	for(ii=0; ii<cDataLen; ii++)
	{
		sChkSum = sChkSum + strData[ii];
	}
	strOnePack[__SNK_COMM_ONE_PACK_OFST_CHKSUM__] = (char)sChkSum;

	return SNK_OK;

}
short snk_server_create_new_snake_for_join_game(unsigned char ucSnakeId)
{
	short sRet  = SNK_OK;
	unsigned short usTotalSnake;

	usTotalSnake = snk_get_total_snake_number();

	if(ucSnakeId >= __SNK_MAX_PLAYERS__)
	{
		sRet = SNK_ERR;
	}
	else
	{
#ifdef __PREV_GAME__
		switch(usTotalSnake)
		{
		case 0:
//			snk_create_snake(ucSnakeId, __SNK_INIT_LEN__, 20, //20, __MOVING_DIR_X_POS__, &lstSnake[ucSnakeId]);
			break;
		case 1:
//			snk_create_snake(ucSnakeId, __SNK_INIT_LEN__, 50, //30, __MOVING_DIR_X_NEG__, &lstSnake[ucSnakeId]);
			break;
		case 2:
//			snk_create_snake(ucSnakeId, __SNK_INIT_LEN__, 50, //20, __MOVING_DIR_Y_POS__, &lstSnake[ucSnakeId]);
			break;
		case 3:
//			snk_create_snake(ucSnakeId, __SNK_INIT_LEN__, 20, 30, __MOVING_DIR_Y_NEG__, &lstSnake[ucSnakeId]);
			break;
		default:
			break;
		}
#endif // __PREV_GAME__
		if(lstSnake[ucSnakeId].usFlagAlive == SNAKE_DEAD)
		{
                        lstSnake[ucSnakeId].usFlagAlive = SNAKE_JOIN_GAME;
			snk_total_snake_number_increase_one();
		}

	}

	return sRet;
}

extern short gsGameStarted;

short snk_server_execute_game_cmd(unsigned char ucSnakeId, unsigned char ucGameCmdCode)
{
	unsigned short ii, usTotalSnake, usFlagAllSnakeReady = 1;
        int iLen;
	unsigned long ll;

        printf("Game Command from Client: %d", ucGameCmdCode);

	switch(ucGameCmdCode)
	{
	case __SNK_COMM_DATA_DEF_JOIN_GAME__:
		snk_server_create_new_snake_for_join_game(ucSnakeId);
		break;
	case __SNK_COMM_DATA_DEF_EXIT_GAME__:
		lstSnake[ucSnakeId].usFlagAlive = SNAKE_DEAD;
                snk_total_snake_number_decrease_one();

		break;
	case __SNK_COMM_DATA_DEF_START_GAME__:
		lstSnake[ucSnakeId].usFlagAlive = SNAKE_READY_START;
		break;
	case __SNK_COMM_DATA_DEF_GIVEUP_GAME__:
		lstSnake[ucSnakeId].usFlagAlive = SNAKE_DEAD;
		snk_total_snake_number_decrease_one();

                break;
	case __SNK_COMM_DATA_DEF_ABORT_GAME__:
		lstSnake[ucSnakeId].usFlagAlive = SNAKE_DEAD;
		lstSnake[ucSnakeId].ucSnakeScore = __SNK_SCORE_PENALTY_ABORT_GAME__;
                snk_total_snake_number_decrease_one();

		break;
	default:
		break;
	}

	//// To judge whether game should start
	if(ucGameCmdCode == __SNK_COMM_DATA_DEF_START_GAME__)
	{
		/// The following coding could be built into a function, and could also improved to be with delay counter
		usTotalSnake = snk_get_total_snake_number();
		for(ii = 0; ii<usTotalSnake; ii++)
		{
			if(lstSnake[ii].usFlagAlive != SNAKE_READY_START)
			{
				usFlagAllSnakeReady = 0;
				break;
			}
		}
		if(usFlagAllSnakeReady == 1)
		{
			for(ii = 0; ii<usTotalSnake; ii++)
			{
				lstSnake[ii].usFlagAlive = SNAKE_ALIVE;
			}
                        gsGameStarted = 1;
#define GAME_START_WAIT_CLIENT_START_TCP    100
  
                        snk_server_package_data(__SNK_COMM_DATA_DEF_GAME_CMD_SVR_2_CLT__, 
__SNK_COMM_DATA_DEF_GAME_START__, 1, "\0");
                      iLen = 8;

                      printf("BroadCast Game start!!!\n");
                      for(ll = 0; ll<iLen; ll++)
                      {
                           printf("%d, ", strOnePack[ll]);
                      }

                      for(ll = 0; ll< GAME_START_WAIT_CLIENT_START_TCP; ll++)
                        {;
                        }
                        snake_broadcast_player_with_pack_len(strOnePack,  iLen);

		}
	}

        if(lstSnake[ucSnakeId].usFlagAlive == SNAKE_DEAD)
        {
              usTotalSnake = snk_get_total_snake_number();
              if(usTotalSnake == 0)
              { // Broadcast game over command
                   gsGameStarted= 0;
                   snk_server_broadcast_game_over();
//                   init_snake(); // @2.2
              }


        }
	return SNK_OK;
}

void snk_server_broadcast_game_over()
{
unsigned long ll;
unsigned int iLen;

snk_server_package_data(__SNK_COMM_DATA_DEF_GAME_CMD_SVR_2_CLT__, 
__SNK_COMM_DATA_DEF_GAME_OVER__, 1, "\0");
                      iLen = 8;

                      printf("BroadCast Game Over!!!\n");
                      for(ll = 0; ll<iLen; ll++)
                      {
                           printf("%d, ", strOnePack[ll]);
                      }

                      for(ll = 0; ll< GAME_START_WAIT_CLIENT_START_TCP; ll++)
                        {;
                        }
                        snake_broadcast_player_with_pack_len(strOnePack,  iLen);

}


short snk_server_execute_motion_cmd(unsigned char ucSnakeId, unsigned char ucMotionCmdCode)
{
	short sRet  = SNK_OK;

	if(ucMotionCmdCode >= __SNK_MAX_FEASIBLE_DIR__ ||
		 ucSnakeId >= __SNK_MAX_PLAYERS__)
	{
		sRet = SNK_ERR;
	}
	else
	{
		lstSnake[ucSnakeId].sMovingDir = ucMotionCmdCode;
	}
	snk_move_one_unit(&lstSnake[ucSnakeId]);
        snake_update_one_snk_alive_flag(ucSnakeId);
        snake_update_one_snk_score(ucSnakeId);

	return sRet;
}
/**------------------------
 * Name:
 * Purpose:
 * Date of Creating:
 * Notes
 *-------------------------**/
short snk_server_decode_data(const char *strRecvdData)
{
unsigned char ucSnakeId;
unsigned char ucCmdType;
unsigned char ucCmdCode;
unsigned char ucDataLen;
unsigned char ucChkSum;
unsigned char ucActualCheckSum = 0;
unsigned short ii;
short sRet = SNK_OK;

	ucSnakeId = strRecvdData[__SNK_COMM_ONE_PACK_OFST_SENDER_ID__];
	ucCmdType = strRecvdData[__SNK_COMM_ONE_PACK_OFST_CMD_TYPE__]; 
	ucCmdCode = strRecvdData[__SNK_COMM_ONE_PACK_OFST_CMD_CODE__]; 
	ucDataLen = strRecvdData[__SNK_COMM_ONE_PACK_OFST_DATA_LEN__];
	ucChkSum  = strRecvdData[__SNK_COMM_ONE_PACK_OFST_CHKSUM__];
        if(gsDebugLevel >= 3)
	{
	    printf("\nRaw Data: %d, %d, %d, %d, %d",
		   strRecvdData[0], strRecvdData[1], strRecvdData[2],
                               strRecvdData[3], strRecvdData[4]  );
	}
        for(ii = 0; ii<=6; ii++)
	{
		ucActualCheckSum = ucActualCheckSum + strRecvdData[ii];
	}

#ifdef __CHK_SUM__
	if(ucActualCheckSum != ucChkSum)
	{
		sRet = SNK_ERR;
	}
	else
	{
#endif
		switch(ucCmdType)
		{
		    case __SNK_COMM_DATA_DEF_GAME_CMD_CLT_2_SVR__:

				snk_server_execute_game_cmd(ucSnakeId, ucCmdCode);
				break;
		    case __SNK_COMM_DATA_DEF_MTN_CMD_CLT_2_SVR__:
				snk_server_execute_motion_cmd(ucSnakeId, ucCmdCode);
				break;
		    default:
				break;
		}
#ifdef __CHK_SUM__
	}
#endif
	return sRet;
}

