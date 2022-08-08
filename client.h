
// 	Writen - HMS April 2017
//  Supports TCP and UDP - both client and server


#ifndef __CLIENT_H__
#define __CLIENT_H__
#define DEBUG_FLAG 1
#define MAX_HANDLE_LEN 100

void chat(int clientSocket, char* handle);
int readFromStdin(char * buffer);
int getNumWords(char* input);
int isNumber(char* str);
void getWords(char* input, char*words[], int words_num);
void checkArgs(int argc, char * argv[]);
void initConnection(int clientSocket, char* handle);
void processInput(int len, char *input, int clientSocket, char* handle);
void processCmdM(int clientSocket, int len, char* input, char* handle);
void processCmdB(int clientSocket, int len,char* input,char* handle);
void processCmdL(int clientSocket);
void processCmdE(int clientSocket);
void parsePacket(uint8_t *buf, int socketNum, char* handle);
#endif
