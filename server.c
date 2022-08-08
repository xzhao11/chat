/****************************************************************************
* server.c
*
* CPE 464 - Program 1
****************************************************************************/


#include "networks.h"
#include "sendRecvPdu.h"
#include "pollLib.h"
#include "packet.h"
#include "handleTable.h"
#include "server.h"

int main(int argc, char *argv[])
{
	//server socket descriptor
	int serverSocket = 0;   
	int portNumber = 0;
	
	portNumber = checkArgs(argc, argv);
	
	//create the server socket
	serverSocket = tcpServerSetup(portNumber);
	//accpting connection
	connectSockets(serverSocket);
	close(serverSocket);
	return 0;
}

//receiving packets from clients and respond with different packets/
//forward packets to different clients
void connectSockets(int serverSocket) {
	int socketNum = -1;
	uint16_t numbytes;
	uint8_t buf[MAXBUF];
	//init empty handle table
	initHandleTable();
	//set up poll to listen for activities
	setupPollSet();
	//watch for incomming connection
	addToPollSet(serverSocket);

	while(1) {
		if ((socketNum = pollCall(0)) != -1) {
			//new activity is new incomming connection
			if (socketNum == serverSocket) {
				acceptNewClient(serverSocket);
				socketNum = -1;
				continue;
			}
			//new activity is new packets arriving
			numbytes = recvBuf(socketNum, buf, MAXBUF);
			// printf("numbytes is %d\n", numbytes);
			//client terminated
			if (numbytes == 0) {
				removeClient(socketNum);
				removeFromHandleTable(socketNum);
				socketNum = -1;
				continue;
			}
			parsePacket(buf, socketNum);
		}
	}
}

//parse different packets and process them based on flag
void parsePacket(uint8_t *buf, int socketNum){
	chat_header * header = (chat_header *)buf;
    uint8_t flag = header->flag;
    // printf("flag is %d\n", flag);
    switch(flag){
        case 1:
	        processFlag1Packet(buf, socketNum);
	        break;
	    case 10:
	        processFlag10Packet(socketNum);
	        break;
	    case 8:
	        processFlag8Packet(socketNum);
	         break;
	    case 5:
	    	//message packet only forwarding
	        forwardFlag5Packet(buf);
	        break;
	    case 4:
	    	//broadcast packet only forwarding
	        forwardFlag4Packet(buf);
	        break;
	default:
		 printf("Not defined\n");
		 break;
    }
}

//accepting new client and listen for its incoming packets
void acceptNewClient(int serverSocket) {
	int clientSocket = tcpAccept(serverSocket, -1);
	addToPollSet(clientSocket);	
}

//removing client
void removeClient(int clientSocket) {
	removeFromPollSet(clientSocket);
	close(clientSocket);
}

int checkArgs(int argc, char *argv[])
{
	// Checks args and returns port number
	int portNumber = 0;

	if (argc > 2)
	{
		fprintf(stderr, "Usage %s [optional port number]\n", argv[0]);
		exit(-1);
	}
	
	if (argc == 2)
	{
		portNumber = atoi(argv[1]);
	}
	
	return portNumber;
}

