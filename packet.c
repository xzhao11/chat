

#include <netinet/in.h>
#include "packet.h"
#include "sendRecvPdu.h"

/***********general usage************/
//build a plain chat_header packet
int plainChatHeader(uint8_t flag, chat_header* header){
    header->flag = flag;
    //PDU len = chat-header
    header->length = htons(sizeof(chat_header));
    return ntohs(header->length);
}

//assign flag to header
int buildFlagPacket(uint8_t* buf, uint8_t flag) {
	chat_header * header = (chat_header *)buf;
	plainChatHeader(flag, header);
	return ntohs(header->length);
}

//build a packet with one handle (for flag 7 and 12)
int buildFlagPacketWithHandle(uint8_t* buf, uint8_t flag, char* handle) {
	chat_header* header =  (chat_header *)buf;
	header->length = htons(sizeof(chat_header) + 
				sizeof(uint8_t) + sstrlen(handle));
	header->flag = flag;
	//apend handle len and handle to the packet
	addHandleToBuf(buf + sizeof(chat_header), handle);
	return ntohs(header->length);
}


//build packet with set length (for flag 4 and 5)
int buildFlagPacketWithLength(uint8_t* buf, uint8_t flag, uint16_t length) {
	chat_header* header =  (chat_header *)buf;
	header->length = htons(length);
	header->flag = flag;
	return ntohs(header->length);
}


/***********client usage************/
//build a flag = 1 packet to connect with sever
int buildFlag1Packet(uint8_t* buf, char* handle) {
	chat_header * header = (chat_header *)buf;
	//PDU len =  chat-header, then 1 byte handle length then the handle
	header->length = htons(sizeof(chat_header) + 
		sizeof(uint8_t) + sstrlen(handle));
	header->flag = 1;
	//apend handle len and handle to the packet
	addHandleToBuf(buf + sizeof(chat_header), handle);
	return ntohs(header->length);
}

//process received message packet forwarded from server
void processFlag5Packet(uint8_t* buf) {
	uint8_t handle_num;
	char src_handle[MAX_HANDLE_LEN] = "";
	char dest_handle[MAX_HANDLE_LEN] = "";
	char msg[MAX_MSG_LEN];
	//get src handle from packet
	uint8_t * ptr = getHandleFromBuf(buf+sizeof(chat_header), src_handle);
	printf("\n%s: ", src_handle);
	smemcpy(&handle_num, ptr, sizeof(uint8_t));
	ptr += 1;
	int i;
	for(i = 0; i < handle_num; i++) {
		//get each dest handle from packet
		ptr = getHandleFromBuf(ptr, dest_handle);
	}   
	smemcpy(msg, ptr, MAX_MSG_LEN);
	// printf("%lu\n",strlen( msg));  
	printf("%s\n", msg);   
}

//process received broadcast packet forwarded from server
void processFlag4Packet(uint8_t* buf) {
	char src_handle[MAX_HANDLE_LEN] = "";
	char msg[MAX_MSG_LEN];
	//get src handle from packet
	uint8_t * ptr = getHandleFromBuf(buf+sizeof(chat_header), src_handle);
	printf("\n%s: ", src_handle);
	smemcpy(msg, ptr, MAX_MSG_LEN);
	// printf("%lu\n",strlen( msg));  
	printf("%s\n", msg);   
}

//build message packet based on user input
void buildFlag5Packets(int clientSocket, uint8_t* buf, 
	char* handle, char*words[], int wordsLen, 
	int inputLen, char* input) {
	uint8_t handle_num = atoi(words[1]);
	uint16_t length = 0;
	//the length of command part(%m handle-num [handle-name]) including space
	uint8_t nonMsgLen = sstrlen(words[0])+1+sstrlen(words[1])+1;
	//apend src handle len and handle to the packet
	uint8_t *ptr = addHandleToBuf(buf + sizeof(chat_header), handle);
	smemcpy(ptr, &handle_num, sizeof(uint8_t));
	ptr = ptr + 1;
	int i;
	for(i = 0; i < handle_num; i++) {
		//apend each dest handle len and handle to the packet
		ptr = addHandleToBuf(ptr, words[i+2]);
		length += sizeof(uint8_t) + sstrlen(words[i+2]);
		nonMsgLen += sstrlen(words[i+2])+1;
	}
	//message part of the input
	char msg[inputLen-nonMsgLen];
	smemcpy(msg, input + nonMsgLen, inputLen-nonMsgLen);
	length+= sizeof(chat_header) + sizeof(uint8_t) + sstrlen(handle)
	 + sizeof(uint8_t);
	 buildMsgPacket(clientSocket, buf, ptr, 5, msg, length);
}

//build broadcast packet based on user input
void buildFlag4Packets(int clientSocket, uint8_t* buf, char* handle, 
	int inputLen, char* input) {
	uint16_t length = 0;
	//apend handle len and handle to the packet
	uint8_t *ptr = addHandleToBuf(buf + sizeof(chat_header), handle);
	//message part of the input
	char msg[inputLen-3];
	smemcpy(msg, input + 3, inputLen-3);
	length+= sizeof(chat_header) + sizeof(uint8_t) + sstrlen(handle);
	buildMsgPacket(clientSocket, buf, ptr, 4, msg, length);
}

void buildMsgPacket(int clientSocket, uint8_t *buf, uint8_t *ptr, 
	uint8_t flag, char msg[], uint16_t length) {
	uint16_t packetLen;
	char* msg_ptr = msg;
	 uint16_t msgLeft = sstrlen(msg);
	 while(msgLeft > 0) {
	 	//break message into 200 pieces and send multiple packets if so
	 	if (msgLeft >= 199) {
	 		smemcpy(ptr, msg_ptr, 199);
	 		smemset(ptr+199, '\0', sizeof(char));
			packetLen = buildFlagPacketWithLength(buf, flag, length+200);
			msg_ptr = msg_ptr + 199;
			msgLeft -= 199;			
			sendBuf(clientSocket, buf, packetLen);
	 	}
	 	else {
			smemcpy(ptr, msg_ptr, msgLeft);
			smemset(ptr+msgLeft, '\0', sizeof(char));
			packetLen = buildFlagPacketWithLength(buf, flag, length+msgLeft+1);
			msgLeft = 0;
			sendBuf(clientSocket, buf, packetLen);
			break;
	 	} 
	 }
}


/***********server usage************/

//process flag = 1 packet to accept connection
void processFlag1Packet(uint8_t* buf, int clientSocket) {
	chat_header header;
	char handle[MAX_HANDLE_LEN] = "";
	//get handle from packet
	getHandleFromBuf(buf+sizeof(chat_header), handle);     
	if (addToHandleTable(clientSocket, handle) < 0) {
		//handle already exist, send flag = 3 packet
		plainChatHeader(3, &header);
	}
	else {
		//handle can be added, send flag = 2 packet
		plainChatHeader(2, &header);
	}
	sendBuf(clientSocket, (uint8_t *)&header, sizeof(chat_header));
}

//build a flag = 11 packet to give the list of handles
int buildFlag11Packet(uint8_t* buf) {
	chat_header* header =  (chat_header *)buf;
	//PDU len = chat-header, 4 byte number (integer, in network order) 
	//stating how many handles are currently known by the server
	header->length = htons(sizeof(chat_header) + 
		sizeof(uint32_t));
	header->flag = 11;
	uint32_t size =  htons(getTableSize());
	//appending handle table size to the packets
	smemcpy(buf + sizeof(chat_header), &size, sizeof(uint32_t));
	return ntohs(header->length);
}

//processing flag = 8 packet, granting client's exiting
void processFlag8Packet(int clientSocket) {
	chat_header header; 
	removeFromHandleTable(clientSocket);
	//sending ACK packet
	plainChatHeader(9, &header);
	sendBuf(clientSocket, (uint8_t *)&header, sizeof(chat_header));
}

//forward message packet to each dest client
void forwardFlag5Packet(uint8_t* buf) {
	chat_header * header = (chat_header *)buf;
	uint8_t handle_num;
	char src_handle[MAX_HANDLE_LEN] = "";
	char dest_handle[MAX_HANDLE_LEN] = "";
	//get src handle from packet
	uint8_t * ptr = getHandleFromBuf(buf+sizeof(chat_header), src_handle);
	smemcpy(&handle_num, ptr, sizeof(uint8_t));
	ptr += 1;
	//for each dest client, forward the original packet
	int i;
	for(i = 0; i < handle_num; i++) {
		//get each dest handle from packet
		ptr = getHandleFromBuf(ptr, dest_handle);
		if (getSocketFromHandle(dest_handle) > 0) {
			//handle exits
			sendBuf(getSocketFromHandle(dest_handle), buf, 
				ntohs(header->length));
		} else {
			//handle does not exist, send flag = 7 packet to the src client
			int length = buildFlagPacketWithHandle(buf, 7, src_handle);
			sendBuf(getSocketFromHandle(src_handle), buf, length);
		}
		
	}    
}

//forward message packet to all other clients
void forwardFlag4Packet(uint8_t* buf) {
	chat_header * header = (chat_header *)buf;
	char src_handle[MAX_HANDLE_LEN] = "";
	//get src handle from packet
	getHandleFromBuf(buf+sizeof(chat_header), src_handle);
	int table_size = getTableSize();
	int sockets[table_size];
	getTableSockets(sockets);
	int i;
	for(i = 0; i < table_size; i++) {
		//only broadcast to others, not the src client
		if (getSocketFromHandle(src_handle) != sockets[i]) {
			sendBuf(sockets[i], buf, ntohs(header->length));
		}
	}    
}

//processing flag = 10 packet asked for list of clients
//send 11, 12 and 13 packets respectfully
void processFlag10Packet(int clientSocket) {
	uint8_t buf[MAXBUF];
	uint16_t packetLen;
	chat_header header;
	packetLen = buildFlag11Packet(buf);
	//send one 11 packet indicating number of clients
	sendBuf(clientSocket, buf, packetLen);
	char *handles[getTableSize()];
	getTableHandles(handles);
	int i;
	for(i = 0; i < getTableSize(); i++) {
		packetLen = buildFlagPacketWithHandle(buf, 12, handles[i]);
		//send one 12 packet for each existing client
		sendBuf(clientSocket, buf, packetLen);
	}
	plainChatHeader(13, &header);
	//send one 13 packet indicating the end of response to 10
	sendBuf(clientSocket, (uint8_t *)&header, sizeof(chat_header));
}

//add handle length and handle name to buffer
uint8_t * addHandleToBuf(uint8_t *buf, char *handle){
   uint8_t handle_len = sstrlen(handle);
   //append handle length
   smemcpy(buf, &handle_len, sizeof(handle_len));
   //append handle name
   smemcpy(buf + sizeof(handle_len), handle, 
   	sstrlen(handle) * sizeof(uint8_t));
   //return new ptr to the address after the handle name
   return buf + sizeof(handle_len) + sstrlen(handle);
}

uint8_t * getHandleFromBuf(uint8_t* buf, char* handle) {
	uint8_t handle_len;
	//get handle length
	smemcpy(&handle_len, buf, sizeof(handle_len));
	//get handle name
	smemcpy(handle, buf +sizeof(handle_len), handle_len);
	handle[handle_len+1] = '\0';
	//return new ptr to the address after the handle name
	return buf + sizeof(handle_len) + sstrlen(handle);
}









