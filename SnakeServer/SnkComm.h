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

 #ifndef __SNKCOMM_H__
 #define __SNKCOMM_H__
 
 #define __SNK_COMM_ONE_PACK_OFST_SENDER_ID__   0
 #define __SNK_COMM_ONE_PACK_OFST_RECVER_ID__   1
 #define __SNK_COMM_ONE_PACK_OFST_CMD_TYPE__    2
 #define __SNK_COMM_ONE_PACK_OFST_CMD_CODE__    3
 #define __SNK_COMM_ONE_PACK_OFST_DATA_LEN__    4
 #define __SNK_COMM_ONE_PACK_OFST_CHKSUM__      5
 #define __SNK_COMM_ONE_PACK_OFST_RESERVE__     6
 #define __SNK_COMM_ONE_PACK_OFST_DATA_START__  7
 
 //// Available Data for communication 
 #define __SNK_COMM_DATA_DEF_SERVER_ID__        0x7F
 
 
 //// Available Command Type for communication
 #define __SNK_COMM_DATA_DEF_GAME_CMD_CLT_2_SVR__     0x1
 #define __SNK_COMM_DATA_DEF_MTN_CMD_CLT_2_SVR__      0x2
 #define __SNK_COMM_DATA_DEF_GAME_CMD_SVR_2_CLT__     0x41
 #define __SNK_COMM_DATA_DEF_SNAKE_STATUS_CMD_SRV_2_CLT__   0x42
 #define __SNK_COMM_DATA_DEF_APPLE_STATUS_CMD_SRV_2_CLT__   0x43
 
 
 //// Command Code for Command Type 0x1
 #define __SNK_COMM_DATA_DEF_RESERVE_CLT_2_SRV__      0x0
 #define __SNK_COMM_DATA_DEF_JOIN_GAME__              0x1
 #define __SNK_COMM_DATA_DEF_EXIT_GAME__              0x2
 #define __SNK_COMM_DATA_DEF_START_GAME__             0x3
 #define __SNK_COMM_DATA_DEF_GIVEUP_GAME__            0x4
 #define __SNK_COMM_DATA_DEF_ABORT_GAME__             0x5
 
  //// Command Code for Command Type 0x2
 #define __SNK_COMM_DATA_DEF_SNK_MOVE_X_POS__          0x0
 #define __SNK_COMM_DATA_DEF_SNK_MOVE_Y_POS__          0x1
 #define __SNK_COMM_DATA_DEF_SNK_MOVE_X_NEG__          0x2
 #define __SNK_COMM_DATA_DEF_SNK_MOVE_Y_NEG__          0x3

 //// Command Code for Command Type 0x81
 #define __SNK_COMM_DATA_DEF_RESERVE_SRC_2_CLT__      0x0
 #define __SNK_COMM_DATA_DEF_GAME_START__             0x1
 #define __SNK_COMM_DATA_DEF_GAME_OVER__              0x2
 
 
 //// Command Code for Command Type 0x82
 #define __SNK_COMM_DATA_DEF_SNAKE_INFO__             0x1
 
 //// Command Code for Command Type 0x83
 #define __SNK_COMM_DATA_DEF_APPLE_INFO__             0x1
  

 #define __SNK_COMMU_MAXIMUM_DATA_SIZE__    1024

extern char strDataPackPlanarySnakeInfo[];
extern char strDataPackPlanaryAppleInfo[];

extern char strOnePack[];

extern short snk_server_package_snake_info(unsigned short *pusTotalByte);
extern short snk_server_package_app_info(unsigned short *pusTotalByte);
extern short snk_server_package_data(char cCmdType, char cCmdCode, char cDataLen, const char *strData);

extern void snk_server_broadcast_game_over();
 #endif
 
 
 
