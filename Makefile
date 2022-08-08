# Makefile for CPE464 tcp test code
# written by Hugh Smith - April 2019

CC= gcc
CFLAGS= -g -Wall
LIBS = 


all:   clean cclient server

cclient: cclient.c sendRecvPdu.c networks.o packet.o handleTable.o safeUtil.o pollLib.o gethostbyname.o
	$(CC) $(CFLAGS) -o cclient cclient.c sendRecvPdu.c networks.o packet.o handleTable.o safeUtil.o pollLib.o gethostbyname.o $(LIBS)

server: server.c sendRecvPdu.c networks.o packet.o handleTable.o safeUtil.o pollLib.o gethostbyname.o
	$(CC) $(CFLAGS) -o server server.c sendRecvPdu.c networks.o packet.o handleTable.o safeUtil.o pollLib.o gethostbyname.o $(LIBS)

.c.o:
	gcc -c $(CFLAGS) $< -o $@ $(LIBS)

cleano:
	rm -f *.o

clean:
	rm -f server cclient *.o




