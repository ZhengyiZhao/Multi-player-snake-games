makefile:
all: timersnake2

clean:
	-rm kbhit.o	linklist.o	SnkComm.o	snktimer.o	TcpUdp.o	timersnake2.o	timersnake2
  
timersnake2: kbhit.o linklist.o SnkComm.o snktimer.o TcpUdp.o timersnake2.o
	gcc -g -o timersnake2 kbhit.o linklist.o SnkComm.o snktimer.o TcpUdp.o timersnake2.o -lpthread
  

SNAKESERVER_SRC     = kbhit.c linklist.c SnkComm.c snktimer.c TcpUdp.c timersnake2.c 

SNAKESERVER_OBJ     = $(SNAKESERVER_SRC:.c=.o) 

$(SNAKESERVER_OBJ): %.o: %.c
	gcc -c  $< -o $@