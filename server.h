
// 	Writen - HMS April 2017
//  Supports TCP and UDP - both client and server


#ifndef __SERVER_H__
#define __SERVER_H__
#define DEBUG_FLAG 1

void recvFromClient(int clientSocket);
int checkArgs(int argc, char *argv[]);
void connectSockets(int serverSocket);
void acceptNewClient(int serverSocket);
void removeClient(int clientSocket);
void parsePacket(uint8_t *buf, int socketNum);
#endif
