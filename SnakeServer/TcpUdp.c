
/* needed, but not byte after bytes. All the received data is read.      */
/* This means only one call to recv(), instead of one call for           */
/* each received byte.                                                   */
/* You can set END_CHAR to whatever means endofline for you. (0x0A is \n)*/
/* read_lin returns the number of bytes returned in line_to_return       */

/* tcp Server code  */

#define __HAVE_TCP_UDP__
#ifdef __HAVE_TCP_UDP__

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <unistd.h> /* close */

#include "Snake.h"
#include "TcpUdp.h"
#include "linklist.h"
int currSize = 0;
char msg[MAX_MSG][MAX_SIZE];

// ip address of client
char clientIpAddr[__TCP_IP_MAX_CLIENT__][100];

extern short gsTotalSnakeToJoin;
// ip address of me, the server
char strMyIpAddr[64];

#define  USER_SIMU_APPLE_POSN_X    30
#define  USER_SIMU_APPLE_POSN_Y    20

#include <stdio.h>
#include <unistd.h>
#include <string.h> /* for strncpy */

#include <sys/ioctl.h>
#include <net/if.h>

void snake_get_local_ip_addr()
{
 int fd;
 struct ifreq ifr;

 fd = socket(AF_INET, SOCK_DGRAM, 0);
 /* I want to get an IPv4 IP address */
 ifr.ifr_addr.sa_family = AF_INET;

 /* I want IP address attached to "eth0" */
 strncpy(ifr.ifr_name, "eth0", IFNAMSIZ-1);
 ioctl(fd, SIOCGIFADDR, &ifr);
 close(fd);

 /* display result */
 sprintf(strMyIpAddr, "%s\n", inet_ntoa(((struct sockaddr_in *)&ifr.ifr_addr)->sin_addr));

 return ;
}
/**------------------------
 * Name:
 * Purpose:
 * Date of Creating:
 * Notes
 *-------------------------**/
int read_line(int newSd, char *line_to_return, int count) {
  
  static int rcv_ptr=0;
  static char rcv_msg[MAX_MSG];
  static int n;
  int offset;
 char cDataEcho;

         cDataEcho = (char) (count - 1);
         offset=0;

  while(1) {
    if(rcv_ptr==0) {
      /* read data from socket */
      memset(rcv_msg,0x0,MAX_MSG); /* init buffer */
      n = recv(newSd, rcv_msg, MAX_MSG, 0); /* wait for data */
      if (n<0) {
	printf(" cannot receive data ");
	return ERROR;
      } else if (n==0) {
	printf(" connection closed by client\n");
	close(newSd);
	return ERROR;
      }else{
       printf("Going to Send Data %d", cDataEcho);
         send(newSd, &cDataEcho, 1, 0);

         printf("Send Data %d", cDataEcho);
         return 5;
      }
    }
  
    /* if new data read on socket */
    /* OR */
    /* if another line is still in buffer */

    /* copy line into 'line_to_return' */
    while(*(rcv_msg+rcv_ptr)!=END_LINE && rcv_ptr<n) {
      memcpy(line_to_return+offset,rcv_msg+rcv_ptr,1);
      offset++;
      rcv_ptr++;
      return ++offset; 
   }
   
    /* end of line + end of buffer => return line */
    if(rcv_ptr==n-1) { 
      /* set last byte to END_LINE */
      *(line_to_return+offset)=END_LINE;
      rcv_ptr=0;
      return ++offset;
    } 
    
    /* end of line but still some data in buffer => return line */
    if(rcv_ptr <n-1) {
      /* set last byte to END_LINE */
      *(line_to_return+offset)=END_LINE;
      rcv_ptr++;
      return ++offset;
    }

    /* end of buffer but line is not ended => */
    /*  wait for more data to arrive on socket */
    if(rcv_ptr == n) {
      rcv_ptr = 0;
      return ++offset;
    } 
    
  } /* while */
}

/**------------------------
 * Name:
 * Purpose:
 * Date of Creating:
 * Notes
 *-------------------------**/
int snk_tcp_server_realtime_join_game()
{
 
  int sd, cliLen, newSd;
  struct sockaddr_in cliAddr, servAddr;
  char line[MAX_MSG];
  int count = 0;
  char cDataEcho;


    memset(msg,0x0,MAX_MSG);
  /* create socket */
  sd = socket(AF_INET, SOCK_STREAM, 0);
   if(sd<0) {
    perror("cannot open socket ");
    return ERROR;
  }
  
  /* bind server port */
  servAddr.sin_family = AF_INET;
  servAddr.sin_port = htons(TCP_SERVER_PORT);

  #define __GET_ADDRESS_FROM_STRING_VAR__
  /* bind server port */
 #ifdef __GET_ADDRESS_FROM_STRING_VAR__
 printf("Assigning s_addr from string: %s\n", strMyIpAddr);
 servAddr.sin_addr.s_addr = inet_addr(strMyIpAddr);
 #endif
 
 #ifdef __GET_ADDRESS_BY_FUNC__
 h = gethostbyname(strMyIpAddr);
 servAddr.sin_family = h->h_addrtype;
 memcpy((char *) &servAddr.sin_addr.s_addr,
         h->h_addr_list[0], h->h_length);
 #endif

  printf("Going to bind ip Addr: %s", inet_ntoa(servAddr.sin_addr)); 
  if(bind(sd, (struct sockaddr *) &servAddr, sizeof(servAddr))<0) {
    perror("cannot bind port ");
    return ERROR;
  }
  else
  {
     sprintf(strMyIpAddr, "%s", inet_ntoa(servAddr.sin_addr));
     printf("My (Server) IP Address Stored in global var: %s\n", strMyIpAddr);
  }
  

  listen(sd,5);
  
  while(1) {

    printf("Server waiting for data on port TCP %u\n", TCP_SERVER_PORT);
///    printf("My IP Address: %s\n", servAddr.sin_addr.s_addr);

    cliLen = sizeof(cliAddr);
    newSd = accept(sd, (struct sockaddr *) &cliAddr, &cliLen);
    if(newSd<0) {
      printf("cannot accept connection ");
      return ERROR;
    }else{
      count++;
      printf("Server accepted connection %d", count);
    }
    
    /* init line */
    memset(line,0x0,MAX_MSG);
    memset(clientIpAddr[count-1],0x0,MAX_MSG);   
 
    /* receive segments */
    while(read_line(newSd,msg[0], count)!=ERROR) {
     printf("Sevver received data %s",msg[0]); 
     printf("Server received from %s:TCP%d : %s\n",  
	     inet_ntoa(cliAddr.sin_addr),
	     ntohs(cliAddr.sin_port),msg[0]);

     sprintf(clientIpAddr[count-1], "%s", inet_ntoa(cliAddr.sin_addr));
     printf("Ipaddress is %s \n", clientIpAddr[count-1]);
   
      	 cDataEcho = (char) (count - 1);

        printf("Going to Send Data %d", cDataEcho);
	 send(newSd, &cDataEcho, 1, 0);

         printf("Send Data %d", cDataEcho); 
   
     /* init line */
     // memset(line,0x0,MAX_MSG);
     //printf("test%s\n",line); 
      
    } /* while(read_line) */
    
  } /* while (1) */

}

/**------------------------
 * Name:
 * Purpose:
 * Date of Creating:
 * Notes
 *-------------------------**/
int startTCPServer()
{
 
  int sd, cliLen, newSd;
  struct sockaddr_in cliAddr, servAddr;
  char line[MAX_MSG];
  int count = 0;
  char cDataEcho;


    memset(msg,0x0,MAX_MSG);
  /* create socket */
  sd = socket(AF_INET, SOCK_STREAM, 0);
   if(sd<0) {
    perror("cannot open socket ");
    return ERROR;
  }
  
  /* bind server port */
  servAddr.sin_family = AF_INET;
  servAddr.sin_port = htons(TCP_SERVER_PORT);

  //#define  __HARD_CODED_IP_ADDR__
  #define __GET_ADDRESS_FROM_STRING_VAR__
  /* bind server port */
 
 #ifdef __DEFAULT_ADDR_LOCAL_TO_LOCAL__
  servAddr.sin_addr.s_addr = htonl(INADDR_ANY);
 #endif
 
 #ifdef __HARD_CODED_IP_ADDR__ 
 // servAddr.sin_addr.s_addr = inet_addr("10.10.2.141");
  servAddr.sin_addr.s_addr = inet_addr("192.168.1.101"); 
 #endif
 
 #ifdef __GET_ADDRESS_FROM_STRING_VAR__
 printf("Assigning s_addr from string: %s\n", strMyIpAddr);
 servAddr.sin_addr.s_addr = inet_addr(strMyIpAddr);
 #endif
 
 #ifdef __GET_ADDRESS_BY_FUNC__
 h = gethostbyname(strMyIpAddr);
 servAddr.sin_family = h->h_addrtype;
 memcpy((char *) &servAddr.sin_addr.s_addr,
         h->h_addr_list[0], h->h_length);
 #endif

  printf("Going to bind ip Addr: %s", inet_ntoa(servAddr.sin_addr)); 
    
  if(bind(sd, (struct sockaddr *) &servAddr, sizeof(servAddr))<0) {
    perror("cannot bind port ");
    return ERROR;
  }
  else
  {
     sprintf(strMyIpAddr, "%s", inet_ntoa(servAddr.sin_addr));
     printf("My (Server) IP Address Stored in global var: %s\n", strMyIpAddr);
  }
  

  listen(sd,5);
  
  while(1) {

    printf("Server waiting for data on port TCP %u\n", TCP_SERVER_PORT);
///    printf("My IP Address: %s\n", servAddr.sin_addr.s_addr);

    cliLen = sizeof(cliAddr);
    newSd = accept(sd, (struct sockaddr *) &cliAddr, &cliLen);
    if(newSd<0) {
      printf("cannot accept connection ");
      return ERROR;
    }else{
      count++;
      printf("Server accepted connection %d", count);
    }

    
    /* init line */
    memset(line,0x0,MAX_MSG);
    memset(clientIpAddr[count-1],0x0,MAX_MSG);   
 
    /* receive segments */
    while(read_line(newSd,msg[0], count)!=ERROR) {
     printf("Sevver received data %s",msg[0]); 
     printf("Server received from %s:TCP%d : %s\n",  
	     inet_ntoa(cliAddr.sin_addr),
	     ntohs(cliAddr.sin_port),msg[0]);

     sprintf(clientIpAddr[count-1], "%s", inet_ntoa(cliAddr.sin_addr));
     printf("Ipaddress is %s \n", clientIpAddr[count-1]);
   
      	 cDataEcho = (char) (count - 1);

        printf("Going to Send Data %d", cDataEcho);
	 send(newSd, &cDataEcho, 1, 0);

         printf("Send Data %d", cDataEcho); 
   
     /* init line */
     // memset(line,0x0,MAX_MSG);
     //printf("test%s\n",line); 
      
    } /* while(read_line) */
    
 int ii;
   if(count== gsTotalSnakeToJoin){
//   if(count==1){
        for(ii=0 ; ii<__SNK_MAX_PLAYERS__; ii++)
        {
		printf("Id--%d, Ipaddress is %s \n", ii, clientIpAddr[ii]);
	}
        printf("Count of %d reached stop TCP Server", __SNK_MAX_PLAYERS__);
	close(newSd);
       return 1;
    }

  } /* while (1) */

}

/* END  TCP SERVER */

int iServerTcpSocketId;

/**------------------------
 * Name:      snk_init_rt_tcp_socketid
 * Purpose:   initialize the real-time tcp socket id in server
 * Date of Creating:
 * Notes
 *-------------------------**/
int snk_init_rt_tcp_socketid()
{
  struct sockaddr_in servAddr;
  
  /* create socket */
  iServerTcpSocketId = socket(AF_INET, SOCK_STREAM, 0);
   if(iServerTcpSocketId<0) {
    perror("cannot open socket ");
    return ERROR;
  }
  
  /* bind server port */
  servAddr.sin_family = AF_INET;
  servAddr.sin_port = htons(TCP_SERVER_PORT);

  /* bind server port */
 printf("Assigning s_addr from string: %s\n", strMyIpAddr);
 servAddr.sin_addr.s_addr = inet_addr(strMyIpAddr);
    
  if(bind(iServerTcpSocketId, (struct sockaddr *) &servAddr, sizeof(servAddr))<0) {
    perror("cannot bind port ");
    return ERROR;
  }
  else
  {
     sprintf(strMyIpAddr, "%s", inet_ntoa(servAddr.sin_addr));
     printf("My (Server) IP Address Stored in global var: %s\n", strMyIpAddr);
  }
	
}
/**------------------------
 * Name:      snk_rt_tcp_thread
 * Purpose:   real time tcp resides in thread to accept dynamic join in
 * Date of Creating:
 * Notes
 *-------------------------**/
int snk_rt_tcp_thread()
{
 
  int iServerTcpSocketId, cliLen, newSd;
  struct sockaddr_in cliAddr, servAddr;
  char line[MAX_MSG];
  int count = 0;
  char cDataEcho;


   memset(msg,0x0,MAX_MSG);

  listen(iServerTcpSocketId,5);
  
  while(1) 	{

    printf("Server waiting for data on port TCP %u\n", TCP_SERVER_PORT);
///    printf("My IP Address: %s\n", servAddr.sin_addr.s_addr);

    cliLen = sizeof(cliAddr);
    newSd = accept(iServerTcpSocketId, (struct sockaddr *) &cliAddr, &cliLen);
    if(newSd<0) {
      printf("cannot accept connection ");
      return ERROR;
    }else{
      count++;
      printf("Server accepted connection %d", count);
    }

    
    /* init line */
    memset(line,0x0,MAX_MSG);
    memset(clientIpAddr[count-1],0x0,MAX_MSG);   
 
    /* receive segments */
    while(read_line(newSd,msg[0], count)!=ERROR) {
     printf("Sevver received data %s",msg[0]); 
     printf("Server received from %s:TCP%d : %s\n",  
	     inet_ntoa(cliAddr.sin_addr),
	     ntohs(cliAddr.sin_port),msg[0]);

     sprintf(clientIpAddr[count-1], "%s", inet_ntoa(cliAddr.sin_addr));
     printf("Ipaddress is %s \n", clientIpAddr[count-1]);
   
      	 cDataEcho = (char) (count - 1);

        printf("Going to Send Data %d", cDataEcho);
	 send(newSd, &cDataEcho, 1, 0);

         printf("Send Data %d", cDataEcho); 
   
     /* init line */
     // memset(line,0x0,MAX_MSG);
     //printf("test%s\n",line); 
      
    } /* while(read_line) */

  } /* while (1) */

}

/**------------------------
 * Name:
 * Purpose:
 * Date of Creating:
 * Notes
 *-------------------------**/
int startUDPServer()
{

  int sd, rc, n, cliLen;
  struct sockaddr_in cliAddr, servAddr;

  /* socket creation */
  sd=socket(AF_INET, SOCK_DGRAM, 0);
  if(sd<0) {
    printf("Server: cannot open socket \n");
    exit(1);
  }

  /* bind local server port */
  servAddr.sin_family = AF_INET;
  servAddr.sin_port = htons(UDP_SERVER_PORT);
//  servAddr.sin_addr.s_addr = htonl(INADDR_ANY);
//  servAddr.sin_addr.s_addr = inet_addr("10.10.2.141");
//  servAddr.sin_addr.s_addr = inet_addr("192.168.1.101");
  servAddr.sin_addr.s_addr = inet_addr(strMyIpAddr);
  rc = bind (sd, (struct sockaddr *) &servAddr,sizeof(servAddr));
  if(rc<0) {
    printf("Server: cannot bind port number %d \n", 
	    UDP_SERVER_PORT);
    exit(1);
  }

  printf("Server: waiting for data on port UDP %u\n", 
	   UDP_SERVER_PORT);

#ifdef __INCREASE_OFST__
    memset(msg[currSize],0x0,MAX_MSG);
#else
    memset(msg[0], 0x0, MAX_MSG);
#endif

  /* server infinite loop */
  while(1) {
#ifdef __INCREASE_OFST__    
    memset(msg[currSize],0x0,MAX_MSG);
#else
    memset(msg[0], 0x0, MAX_MSG);
#endif
    /* receive message */
    cliLen = sizeof(cliAddr);

#ifdef __INCREASE_OFST__
    n = recvfrom(sd, msg[currSize], MAX_MSG, 0, 
		 (struct sockaddr *) &cliAddr, &cliLen);
#else
    n = recvfrom(sd, msg[0], MAX_MSG, 0, (struct sockaddr *)&cliAddr, &cliLen);

#endif
    if(n<0) {
      printf("Server: cannot receive data \n");
      continue;
    }
      
    /* print received message */
    if(gsDebugLevel >= 2)
    {
        printf("\nUdp Cmd from Addr: %s: Port: %u\n", 
	   inet_ntoa(cliAddr.sin_addr),
	   ntohs(cliAddr.sin_port));
    }

    currSize++;

 snk_server_decode_data(msg[0]);

    if(gsDebugLevel >= 2)
    {
    //  printf("Curr Size is %d \n", currSize);
    //    if(currSize == MAX_SIZE){
    //    printf("Buffer FULL!! \n");
      printf("\nContents of Buffer are \n");
      int i=0;
      for(i=0; i<cliLen; i++)
      {
        printf("%d", msg[0][i]);
      }
      printf("\n");
     // currSize = 0;
     // }
    }
  }/* end of server infinite loop */

return 0;

}


int iServerUdpSocketId, iServerReceiveBind;
struct sockaddr_in stServerAddr;

void snk_init_UDP_socket()
{

  /* socket creation */
  iServerUdpSocketId=socket(AF_INET, SOCK_DGRAM, 0);
  if(iServerUdpSocketId<0) {
    printf("Server: cannot open socket \n");
    exit(1);
  }

  /* bind local server port */
  stServerAddr.sin_family = AF_INET;
  stServerAddr.sin_port = htons(UDP_SERVER_PORT);
  stServerAddr.sin_addr.s_addr = inet_addr(strMyIpAddr);
  iServerReceiveBind = bind (iServerUdpSocketId, 
  (struct sockaddr *) &stServerAddr,sizeof(stServerAddr));
  if(iServerReceiveBind<0) {
    printf("Server: cannot bind port number %d \n", 
	    UDP_SERVER_PORT);
    exit(1);
  }

  printf("Server: waiting for data on port UDP %u\n", 
	   UDP_SERVER_PORT);

}

/**------------------------
 * Name:
 * Purpose:
 * Date of Creating:
 * Notes
 *-------------------------**/
int snk_UDPServerThread()
{
 
  int n, cliLen;
  struct sockaddr_in  cliAddr;

    memset(msg[0], 0x0, MAX_MSG);

  /* server infinite loop */
    while(1)
    {
        memset(msg[0], 0x0, MAX_MSG);
    
	/* receive message */
	cliLen = sizeof(cliAddr);
	
	n = recvfrom(iServerUdpSocketId, msg[0], MAX_MSG, 0, 
	(struct sockaddr *)&cliAddr, &cliLen);
	
	if(n<0) {
	printf("Server: cannot receive data \n");
	continue;
	}
	
	/* print received message */
	if(gsDebugLevel >= 2)
	{
		printf("\nUdp Cmd from Addr: %s: Port: %u\n", 
		inet_ntoa(cliAddr.sin_addr),
		ntohs(cliAddr.sin_port));
	}
	
	snk_server_decode_data(msg[0]);

	if(gsDebugLevel >= 2)
	{
		printf("\nContents of Buffer are \n");
		int i=0;
		for(i=0; i<cliLen; i++)
		{
			printf("%d", msg[0][i]);
		}
		printf("\n");
	}
    }/* end of server infinite loop */

return 0;

}


extern char strOnePack[];

// Primitive function 
void snake_broadcast_player(int iLen)
{
#ifdef __HAVE_TCP_UDP__

     // Package each snake's information
     // send data to all clients
    struct sockaddr_in cliAddr, remoteServAddr;
    struct hostent *h;
    int sd, rc, i, c;
 printf("Sending Client Broadcast data to %s %s %s %s\n", clientIpAddr[0], clientIpAddr[1], clientIpAddr[2], clientIpAddr[3]);


 for(c=0; c<1; c++)
   {
  h = gethostbyname(clientIpAddr[c]);

  remoteServAddr.sin_family = h->h_addrtype;
  memcpy((char *) &remoteServAddr.sin_addr.s_addr,
         h->h_addr_list[0], h->h_length);
  remoteServAddr.sin_port = htons(UDP_SERVER_PORT);

  /* socket creation */
  sd = socket(AF_INET,SOCK_DGRAM,0);
  if(sd<0) {
    printf("cannot open socket \n");
  }

  /* bind any port */
  cliAddr.sin_family = AF_INET;
  cliAddr.sin_addr.s_addr = htonl(INADDR_ANY);
  cliAddr.sin_port = htons(0);

  rc = bind(sd, (struct sockaddr *) &cliAddr, sizeof(cliAddr));
  if(rc<0) {
    printf("cannot bind port\n");
  }
   if(gsDebugLevel >= 1)
{
  printf("Data Send \n");
     int ij = 0;
    for(ij =0 ; ij<iLen; ij++){
    printf("%d,", strOnePack[ij]);
    }
    printf("\n");
}
    rc = sendto(sd, strOnePack,iLen , 0,
(struct sockaddr *) &remoteServAddr,
                sizeof(remoteServAddr));
   }
#endif   // __HAVE_TCP_UDP__

}


///
void snake_broadcast_player_with_pack_len(char *strPack, int iLen)
{

     // Package each snake's information
     // send data to all clients
    struct sockaddr_in cliAddr, remoteServAddr;
    struct hostent *h;
    int sd, rc, i, c;

if (gsDebugLevel >=2)
{
 printf("Sending Client Broadcast data to %s %s %s %s\n", clientIpAddr[0], clientIpAddr[1], clientIpAddr[2], clientIpAddr[3]);
}

 for(c=0; c<__SNK_MAX_PLAYERS__; c++)
   {
   if(lstSnake[c].usFlagAlive == SNAKE_ALIVE)
   {
  h = gethostbyname(clientIpAddr[c]);

  remoteServAddr.sin_family = h->h_addrtype;
  memcpy((char *) &remoteServAddr.sin_addr.s_addr,
         h->h_addr_list[0], h->h_length);
  remoteServAddr.sin_port = htons(UDP_SERVER_PORT);

  /* socket creation */
  sd = socket(AF_INET,SOCK_DGRAM,0);
  if(sd<0) {
    printf("cannot open socket \n");
  }

  /* bind any port */
  cliAddr.sin_family = AF_INET;
  cliAddr.sin_addr.s_addr = htonl(INADDR_ANY);
  cliAddr.sin_port = htons(0);

  rc = bind(sd, (struct sockaddr *) &cliAddr, sizeof(cliAddr));
  if(rc<0) {
    printf("cannot bind port\n");
  }
   if(gsDebugLevel >= 3)
{
  printf("Data Send \n");
     int ij = 0;
    for(ij =0 ; ij<iLen; ij++){
    printf("%d,", strPack[ij]);
    }
    printf("\n");
}
    rc = sendto(sd, strPack,iLen , 0,
(struct sockaddr *) &remoteServAddr,
                sizeof(remoteServAddr));
		}
   }

}

int iServerUdpSocketBroadcaseId;

void snake_broadcast_player_init_udp()
{
struct sockaddr_in cliAddr;  
int rc;

  /* socket creation */
  iServerUdpSocketBroadcaseId = socket(AF_INET,SOCK_DGRAM,0);
  if(iServerUdpSocketBroadcaseId<0) {
    printf("cannot open socket \n");
  }

  /* bind any port */
  cliAddr.sin_family = AF_INET;
  cliAddr.sin_addr.s_addr = htonl(INADDR_ANY);
  cliAddr.sin_port = htons(0);

  rc = bind(iServerUdpSocketBroadcaseId, (struct sockaddr *) &cliAddr, sizeof(cliAddr));
  if(rc<0) {
    printf("cannot bind port\n");
  }

}

void snake_broadcast_player_with_pack_len_trigger(char *strPack, int iLen)
{

    struct sockaddr_in remoteServAddr;
    struct hostent *h;
    int i, c, rc;

    if (gsDebugLevel >=2)
    {
        printf("Sending Client Broadcast data to %s %s %s %s\n", clientIpAddr[0], clientIpAddr[1], clientIpAddr[2], clientIpAddr[3]);
	}

    for(c=0; c<__SNK_MAX_PLAYERS__; c++)
    {
       if(lstSnake[c].usFlagAlive == SNAKE_ALIVE)
       {
           h = gethostbyname(clientIpAddr[c]);
	   remoteServAddr.sin_family = h->h_addrtype;
	   memcpy((char *) &remoteServAddr.sin_addr.s_addr,
	            h->h_addr_list[0], h->h_length);
           remoteServAddr.sin_port = htons(UDP_SERVER_PORT);

	   if(gsDebugLevel >= 3)
	   {
	        printf("Data Send \n");
	        int ij = 0;
		for(ij =0 ; ij<iLen; ij++)
		{
			printf("%d,", strPack[ij]);
		}
		printf("\n");
	}
	rc = sendto(iServerUdpSocketBroadcaseId, strPack,iLen , 0,
	(struct sockaddr *) &remoteServAddr,
        sizeof(remoteServAddr));
	}
    }
}

#endif //__HAVE_TCP_UDP__
