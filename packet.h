


#ifndef __PACKET_H__
#define __PACKET_H__

#include "safeUtil.h"
#include "handleTable.h"

#define MAXBUF 1400
#define MAX_MSG_LEN 200
struct chat_header{
   uint16_t length;
   uint8_t flag;
} __attribute__((packed));

typedef struct chat_header chat_header;

int buildFlag1Packet(uint8_t* buf, char* handle);
int buildFlagPacket(uint8_t* buf, uint8_t flag) ;
void buildFlag5Packets(int clientSocket, uint8_t* buf, char* handle, char*words[], int wordsLen, 
   int inputLen, char* input) ;
void buildFlag4Packets(int clientSocket, uint8_t* buf, char* handle, 
   int inputLen, char* input);
void buildMsgPacket(int clientSocket, uint8_t *buf, uint8_t *ptr, 
   uint8_t flag, char raw_msg[], uint16_t length);
int buildFlagPacketWithLength(uint8_t* buf, uint8_t flag, uint16_t length);
int buildFlagPacketWithHandle(uint8_t* buf, uint8_t flag, char* handle);
int buildFlag11Packet(uint8_t* buf); 
void processFlag1Packet(uint8_t* buf, int serverSocket);
void processFlag5Packet(uint8_t* buf);
void processFlag4Packet(uint8_t* buf);
void forwardFlag5Packet(uint8_t* buf);
void forwardFlag4Packet(uint8_t* buf);
void processFlag8Packet(int clientSocket);
void processFlag10Packet(int clientSocket);
uint8_t * addHandleToBuf(uint8_t *buf, char *handle);
int plainChatHeader(uint8_t flag, chat_header* header);
uint8_t * getHandleFromBuf(uint8_t* buf, char* handle);
#endif
