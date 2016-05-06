/***************************************************************************
 *   Copyright (C) 2006 by Zhao ZhengYi                                    *
 *   zzytgx@localhost.localdomain                                          *
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


#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#define __HAVE_TIMER__
#define __HAVE_TCP_UDP__
#define __ACTUAL_SNAKE_MAIN__
#define __HAVE_THREAD__
//#define USE_PRIMITIVE_BROADCAST_FUNC

#ifdef __HAVE_TCP_UDP__
#include <netinet/in.h>
/// Definition of following structures
/// sockaddr_in

#include <netdb.h>
/// Definition of following structures
/// hostent
///
#endif   // __HAVE_TCP_UDP__

#ifdef __HAVE_THREAD__
#include <pthread.h>
void *sub_process(void *);
pthread_t processor;
void *sub_process_tcp(void *);
pthread_t processor_tcp;
#endif // __HAVE_THREAD__

#include <stdio.h>
#include <stdlib.h>
#include "linklist.h"
#include "SnkComm.h"

#include "kbhit.h"
#include "linklist.h"
#include "Snake.h"
#include "TcpUdp.h"
#include "snktimer.h"

/// Template for commenting functions
/**------------------------
 * Name:
 * Purpose:
 * Date of Creating:
 * Notes
 *-------------------------**/

short gsGameStarted =0;
short gsTotalSnakeToJoin = 2;

/**------------------------
 * Name: update_snake_apple_status
 * Purpose:  Snake game-engine, realtime updating the status of both snake and apple
 *        Snake status include: alive_flag
 *                              score
 *        Apple status include: 
 * Date of Creating:
 * Notes
 *-------------------------**/
void update_snake_apple_status()
{
     // To judge whether each snake will die or not
    if(gsGameStarted == 1)
    {
         snake_update_alive_flag();
	 // To judge whether any snake could eat an apple
         snake_update_score();
         // To judge whether an new apple should be generated
         snake_update_apple();
    }
}

/**------------------------
 * Name:  snake_rtug
 * Purpose: Snake game engine, real time updating game
 *          Move snake by the current direction
 *          updating the status of all snakes and apples 
 * Date of Creating:
 * Notes
 *-------------------------**/
void snake_rtug()
{
  short ii;
     // Change snake moving direction, is done during decoding data
         if(gsDebugLevel >= 1)
	 {
           printf("\nBefore Game Logic!!!");
           for(ii=0; ii<__SNK_MAX_PLAYERS__; ii++)
	     {
	       if(lstSnake[ii].usFlagAlive == SNAKE_ALIVE ||
                  lstSnake[ii].usFlagAlive == SNAKE_JOIN_GAME)
		 {
                    printf("Snk-%d: Alive:%d <%d, %d>", ii, 
			   lstSnake[ii].usFlagAlive,
                  lstSnake[ii].pSnakeHead->sPosnX,
		  lstSnake[ii].pSnakeHead->sPosnY);
		 }
	     }
	   printf("\n");
         }
	 
	 move_snake();
         if(gsGameStarted == 1)
         {
	     update_snake_apple_status();
	 }
         if(gsDebugLevel >= 1)
	 {
           printf("\nAfter Game Logic!!!");
           for(ii=0; ii<__SNK_MAX_PLAYERS__; ii++)
	     {
	       if(lstSnake[ii].usFlagAlive == SNAKE_ALIVE ||
                  lstSnake[ii].usFlagAlive == SNAKE_JOIN_GAME)
		 {
                    printf("Snk-%d: Alive:%d <%d, %d>", ii, 
			   lstSnake[ii].usFlagAlive,
                  lstSnake[ii].pSnakeHead->sPosnX,
		  lstSnake[ii].pSnakeHead->sPosnY);
		 }
	     }
	   printf("\n");
         }

}


/**------------------------
 * Name:   snake_rtdi
 * Purpose: Snake Real Time Data Interface, 
 *          Packaging the snake information, 
 *          Packaging the apple information
 *          Then broadcase to all players
 * Date of Creating:
 * Notes
 *-------------------------**/
void snake_rtdi()
{
unsigned short usTotalDataByteSnakeInfo;
unsigned short usTotalDataByteAppleInfo;
unsigned short ii;
	// Read data from TCP/IP buffer
/// Decode command type, command code
/// snk_server_decode_data(msg[0]); 
/// This is moved to the thread immediately after receiving data package from Clients
	 
	 // Package the Snake Information
    if(gsGameStarted == 1)
    {
	 snk_server_package_snake_info(&usTotalDataByteSnakeInfo);
	 snk_server_package_data(__SNK_COMM_DATA_DEF_SNAKE_STATUS_CMD_SRV_2_CLT__,
		                    __SNK_COMM_DATA_DEF_SNAKE_INFO__,
							(char)usTotalDataByteSnakeInfo,
							strDataPackPlanarySnakeInfo);


	 //  Broadcast snake info to all clients
     snake_broadcast_player_with_pack_len_trigger(strOnePack, usTotalDataByteSnakeInfo + 7);
     
	 //  Broadcase apple information to all clients
		 // Package the apple information
     snk_server_package_app_info(&usTotalDataByteAppleInfo);
	 snk_server_package_data(__SNK_COMM_DATA_DEF_APPLE_STATUS_CMD_SRV_2_CLT__,
		                     __SNK_COMM_DATA_DEF_APPLE_INFO__,
							 (char)usTotalDataByteAppleInfo,
							 strDataPackPlanaryAppleInfo);

 	 //  Broadcast apple info to all clients
      snake_broadcast_player_with_pack_len_trigger(strOnePack, usTotalDataByteSnakeInfo + 7);

      if(gsDebugLevel >= 3)
      {
      printf("\nBroadcast apple information to all snakes!!!");
      for(ii=0; ii<usTotalDataByteAppleInfo+7; ii++)
      {
          printf("%d, ", strOnePack[ii]);
       }
      printf("\n");
      }
      
    }
}

/**------------------------
 * Name: snake_rtdi_rtug
 * Purpose:  An entry provided for timer signal triggering, 
 *   so as to be called periodically
 * Date of Creating:
 * Notes: rtdi: Real Time Data Interface
 *        rtug: Real Time Updating Game
 *-------------------------**/
void snake_rtdi_rtug()
{
unsigned short usTotalSnake;
static unsigned short usCount = 0;
//static unsigned short usSetupUdpSocketEveryMin = ;
//         printf("Timer Service Entered\n");
	snake_rtdi();
	snake_rtug();
	if(gsDebugLevel >= 1)
	{
	    usTotalSnake = snk_get_total_snake_number();
	    printf("----------Total number of snakes: %d----------", usTotalSnake);
	}
	
}


#ifdef __HAVE_THREAD__
void snk_rt_monitor()
{
    char cKey;
    
    init_keyboard(); // bakup the current keyboard display setting
    while(1)
    {
       if(kbhit())
       {
           cKey = readch();
           gsDebugLevel = cKey - '0';
	   printf("-----New Debug Level: %d------\n", gsDebugLevel);
	   if(cKey == 'q' || cKey == 'Q')
	   {
	       close_keyboard(); // restore the keyboard settings
	       break;
	   }
       }
    }
}
/**------------------------
 * Name:  sub_process
 * Purpose: An entry provided for thread, 
 * Date of Creating:
 * Notes
 *-------------------------**/
void *sub_process(void *arg)
{

#ifdef __INIT_SERVER_UPD_WITHIN_THREAD__
    startUDPServer();
#else
    snk_UDPServerThread();
#endif  
	// There is a forever while loop in above function,
	// Cannot put decoding consume data here after

}

/**------------------------
 * Name:  sub_process_tcp
 * Purpose: An entry provided for thread, serving tcp
 * Date of Creating:
 * Notes
 *-------------------------**/
void *sub_process_tcp(void *arg)
{

    snk_rt_tcp_thread();
	// There is a forever while loop in above function,
	// Cannot put decoding consume data here after

}

#endif // __HAVE_THREAD__

extern char strMyIpAddr[];

#ifdef __ACTUAL_SNAKE_MAIN__
int main()
{
char cKey;

    printf("SNAKE GAME, debug level: %d\n", gsDebugLevel); 
    init_snake();
	snk_init_apple();
	printf("Created 4 snakes!\n");

#ifdef __HAVE_TIMER__
	init_timer_for_snake();
	printf("Initialize the timer!\n");
#endif  // __HAVE_TIMER__
    snake_get_local_ip_addr(); // @2.1
    if(strlen(strMyIpAddr) == 0)
    {
	    printf("Input a string for local IP Address: ");
	    scanf("%s", strMyIpAddr);
	    printf("Echo string: %s\n", strMyIpAddr);
    }
    else
    {
	printf("local IP address: %s", strMyIpAddr);
    }

    printf("Input total number of snakes to join: ");
    scanf("%d", &gsTotalSnakeToJoin);
    printf("Echo %d\n", gsTotalSnakeToJoin);
#ifdef __HAVE_TCP_UDP__
	
	printf("Start the TPC Server\n");	
	startTCPServer();
    printf("End of TCP Communication.. Lets start the game\n");

    snk_init_UDP_socket();
        
#endif // __HAVE_TCP_UDP__

    snake_broadcast_player_init_udp();
    pthread_create(&processor,NULL ,sub_process,NULL);

    snk_rt_monitor(); // There is a forever loop in rt_monitor.
    
}


#else


int main()
{
FILE *fptr;
short ii = 0;
    init_snake();
	snk_init_apple();
	snk_init_wall();
	debug_snake();
	snake_rtdi();

	fptr = fopen("SnkCommu.txt", "w");

	/*
	  Put some simulation initialization code here
	*/
	lstSnake[0].usFlagAlive = SNAKE_ALIVE;
	lstSnake[1].usFlagAlive = SNAKE_ALIVE;
	lstSnake[2].usFlagAlive = SNAKE_ALIVE;
	lstSnake[3].usFlagAlive = SNAKE_ALIVE;
	gsGameStarted = 1;

	gstSingleAppleInSnakeGame.usPosnX = USER_SIMU_APPLE_POSN_X;
	gstSingleAppleInSnakeGame.usPosnY = USER_SIMU_APPLE_POSN_Y;

	while(1)
	{
		snake_rtdi_rtug();
		ii ++;
		if(ii>=30)
		{
			break;
		}

	}
	
	

}
#endif  // __ACTUAL_SNAKE_MAIN__
