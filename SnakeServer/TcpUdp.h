

#ifndef __TCP_UDP_H__
#define __TCP_UDP_H__

#define END_LINE 0x0
#define TCP_SERVER_PORT 1500
#define UDP_SERVER_PORT 2500
#define MAX_MSG 100
#define MAX_SIZE 100

#define __TCP_IP_MAX_CLIENT__        __SNK_MAX_PLAYERS__
//#define __TCP_IP_MAX_CLIENT__        6

//#define __INIT_SERVER_UPD_WITHIN_THREAD__
extern void snake_get_local_ip_addr(); // @v2.1
extern int startTCPServer();
extern int startUDPServer();
extern void snake_broadcast_player_with_pack_len(char *strPack, int iLen);
extern void snake_broadcast_player(int iLen);

/// To store the client ip address
extern char clientIpAddr[__TCP_IP_MAX_CLIENT__][100];

/// To store the incoming UDP data
extern char msg[MAX_MSG][MAX_SIZE];


#ifdef __INIT_SERVER_UPD_WITHIN_THREAD__
#else
extern void snk_init_UDP_socket();
extern int snk_UDPServerThread();
extern int iServerUdpSocketId, iServerReceiveBind;
extern struct sockaddr_in stServerAddr;

#endif


extern void snake_broadcast_player_init_udp();
extern void snake_broadcast_player_with_pack_len_trigger(char *strPack, int iLen);

#endif  //__TCP_UDP_H__

