/***************************************************************************
* cclient.c
*
* CPE 464 - Program 1
****************************************************************************/

#include <ctype.h>
#include "networks.h"
#include "sendRecvPdu.h"
#include "pollLib.h"
#include "packet.h"
#include "client.h"

int main(int argc, char * argv[])
{
	//client socket descriptor
	int clientSocket = 0;         
	checkArgs(argc, argv);
	//setup client socket
	clientSocket = tcpClientSetup(argv[2], argv[3], -1);
	//connect with server
	initConnection(clientSocket, argv[1]);
	//start chatting
	chat(clientSocket, argv[1]);
	close(clientSocket);
	return 0;
}

//connect with server by sending a flag 1 packet to server 
void initConnection(int clientSocket, char* handle) {
	//poll listener
	int socketNum = -1;
	uint16_t packetLen;
	uint8_t buf[MAXBUF];
	uint16_t numbytes;
	//set up poll to listen for activities
	setupPollSet();
	//watch for incomming connection
	addToPollSet(clientSocket);

	//set up flag = 1 packet
	packetLen = buildFlag1Packet(buf, handle);
	//send packet to ask for connection from server
	sendBuf(clientSocket, buf, packetLen);
	while(1) {
		if ((socketNum = pollCall(0)) != -1) {
			if (socketNum == clientSocket) {
				numbytes = recvBuf(clientSocket, buf, MAXBUF);
				break;
			}
		}
	}
	//received response from server
	if (numbytes > 0) {
		parsePacket(buf, clientSocket, handle);
		return;
	}
	//server did not respond
	else if (numbytes == 0) {
		printf("Server Terminated\n");
		exit(-1);
	}
}

//continually processing user command and sending/receiving packets
// to/from server
void chat(int clientSocket, char* handle) {
	int socketNum = -1;
	uint8_t buf[MAXBUF ];
	char input[MAXBUF ];
	uint16_t numbytes;
	int inputLen = 0;
	//watch for stdin activities
	addToPollSet(STDIN_FILENO);
	while(1) {
		if ((socketNum = pollCall(0)) != -1) {
			//when there is a new incoming packet
			if (socketNum == clientSocket) {
				numbytes = recvBuf(clientSocket, buf, MAXBUF);
				if (numbytes > 0) {
					//parser the packet when packet size valid
					parsePacket(buf, clientSocket, handle);
					
				}
				else if (numbytes == 0) {
					printf("Server Terminated\n");
					exit(-1);
				}
				//waiting for next activity
				socketNum = -1;
			}
			//when there is activity from stdin
			else if (socketNum == STDIN_FILENO) {
				inputLen = readFromStdin(input);
				if (inputLen < 0) {
					continue;
				}
				//process user input
				processInput(inputLen, input, clientSocket, handle);
			}
		}

	}
}

//process user input to process different commands
void processInput(int len, char *input, int clientSocket, char* handle){
	if(input[0] != '%'){ 
		if (input[0] != '\0') {
			printf("Invalid Command\n$: "); 
		}
		fflush(stdout);
		return; 
	}

	switch(input[1]){
		case 'M':
		case 'm':
			processCmdM(clientSocket, len, input, handle);
			fflush(stdout);
			break;
		case 'E':
		case 'e':
			processCmdE(clientSocket);
			break;
		case 'L':
		case 'l':
			processCmdL(clientSocket);
			break;
		case 'B':
		case 'b':
			processCmdB(clientSocket, len, input, handle);
			fflush(stdout);
			break;
		default:
			printf("Invalid Command\n$: ");
			break;
	}

	fflush(stdout);
}

//message command, send flag = 5 packet to server with dest clients and 
//message
void processCmdM(int clientSocket, int len, char* input, char* handle) {
	uint8_t buf[MAXBUF];
	char inputTemp[len];
	sstrcpy(inputTemp, input);
	//get a list of words for handling clients list
	int words_num = getNumWords(inputTemp);
	sstrcpy(inputTemp, input);
    char* words[words_num];
    getWords(inputTemp, words, words_num);
    //number of handles prompted by user
    uint8_t handle_num = atoi(words[1]);
    //if parameter size is wrong or the second parameter is not a 
    //valid number	
	if (words_num < 4 || isNumber(words[1]) == -1 
		|| (words_num -2) <= handle_num){
		printf("Invalid command format\n$: ");
		return;
	}
	//build and send flag = 5 packet to server
	buildFlag5Packets(clientSocket, buf, handle, words, 
		words_num, len, input);	
	
}

//message command, send flag = 5 packet to server with message
void processCmdB(int clientSocket, int len,char* input,char* handle) {
	uint8_t buf[MAXBUF];
	char inputTemp[len];
	sstrcpy(inputTemp, input);
	int words_num = getNumWords(inputTemp);
	//when parameter size is wrong
	if (words_num < 2){
		printf("Invalid command format\n$: ");
		return;
	}
	//build and send flag = 4 packet to server
	buildFlag4Packets(clientSocket, buf, handle, len, input);
}

//message command, send flag = 10 packet to server to get 
//a list of handle name
void processCmdL(int clientSocket) {	
	uint8_t buf[MAXBUF ];
	uint16_t packetLen;
	packetLen = buildFlagPacket(buf, 10);
	sendBuf(clientSocket, buf, packetLen);
}

//exit command, send flag = 8 packet to server to ask for exit
void processCmdE(int clientSocket) {
	uint8_t buf[MAXBUF ];
	uint16_t packetLen;
	packetLen = buildFlagPacket(buf, 8);
	sendBuf(clientSocket, buf, packetLen);
}

//read from stdin for user input
int readFromStdin(char * buffer)
{
	char aChar = 0;
	int inputLen = 0;        
	
	// Important you don't input more characters than you have space 
	buffer[0] = '\0';
	printf("$: ");
	while (inputLen < (MAXBUF  - 1) && aChar != '\n')
	{
		aChar = getchar();
		if (aChar != '\n')
		{
			buffer[inputLen] = aChar;
			inputLen++;
		}
	}
	if (inputLen == (MAXBUF  - 1)) {
		fprintf(stderr, "Input exceeds 1400 characters\n");
		printf("$: ");
		fflush(stdout);
		return(-1);
	}
	buffer[inputLen++] = '\0';
	fflush(stdout);
	return inputLen;
}

//based on the received packet's flag number, process accordingly
void parsePacket(uint8_t *buf, int socketNum, char* client_handle){
	chat_header * header = (chat_header *)buf;
    uint8_t flag = header->flag;
	uint32_t table_size;
	char handle[MAX_HANDLE_LEN] = "";
    switch(flag){
	    case 2:
	        //succussfully connected withs server
	        printf("$: ");
			fflush(stdout);
	        break;
	    case 3:
	    	//unsuccussful connection
	        printf("Handle already in use: %s\n", client_handle);
	        printf("$: ");
			fflush(stdout);
   			exit(-1);
   		case 4:
	    	//received a broadcast msg
	    	processFlag4Packet(buf);
	    	printf("$: ");
			fflush(stdout);
	        break;
	    case 5:
	    	// received a message
	    	processFlag5Packet(buf);
	    	printf("$: ");
			fflush(stdout);
	        break;
   		case 7:
   			//indicate some handle in the dest handles from the
   			//messgae command does not exist on the server side
			getHandleFromBuf(buf+sizeof(chat_header), handle);
			printf("Client with handle %s does not exist\n", handle);
			printf("$: ");
			fflush(stdout);
	        break;
	    case 9:
			//server ack the exit
			exit(0);
	        break;
   		case 11:
   			//first response to the %l command, getting number of handles
			smemcpy(&table_size, buf+sizeof(chat_header), sizeof(uint32_t));
   			table_size = ntohs(table_size);
	        printf("Number of clients: %d\n", table_size);
			fflush(stdout);
	        break;
	    case 12:
	    	//response to the %l command following flag = 11 packet,
	    	// getting a packet for each handle
			getHandleFromBuf(buf+sizeof(chat_header), handle);
			printf("\t%s\n", handle);
	        break;
	    case 13:
	    	//final response to the %l command following flag = 12 packets
	    	//able to proceed
			printf("$: ");
			fflush(stdout);
	        break;
		default:
			printf("Not defined\n");
			break;
    }
}


void checkArgs(int argc, char * argv[])
{
	/* check command line arguments  */
	if (argc != 4)
	{
		fprintf(stderr, "usage: %s handlel host-name port-number \n", argv[0]);
		exit(1);
	}

	/* check handle length */
	if(sstrlen(argv[1]) > MAX_HANDLE_LEN){
		fprintf(stderr, "Invalid handle, handle longer than 100 characters: %s\n", argv[1]);
		exit(1);
	}

	if (isdigit(argv[1][0])) {
		fprintf(stderr, "Invalid handle, handle starts with a number\n");
		exit(1);
	}
}

//check if a string is a valid number
int isNumber(char* str) {
	int i;
	for (i = 0; i < sstrlen(str); i++) {
		if (!isdigit(str[i])) {
			return -1;
		}
	}
	return 1;
}

//get the number of words in a string, separate by space
int getNumWords(char* input) {
	char* token = strtok(input, " ");
	int words_num = 0;
	while (token != NULL) {
        words_num++;
        token = strtok (NULL, " ");
    }
    return words_num;
}

//get the words in a string, separate by space
void getWords(char* input, char*words[], int words_num) {
	// int words_num = getNumWords(input);
	int next = 0;
	char* token = strtok(input, " ");
	while (token != NULL) {
        words[next] = token;
        token = strtok (NULL, " ");
        next++;
    }
}
