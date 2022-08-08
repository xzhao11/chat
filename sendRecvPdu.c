
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <netinet/in.h>

#include "sendRecvPdu.h"

int recvBuf(int clientSocket, uint8_t * dataBuffer, int bufferLen) {
	uint16_t numbytes;
	uint16_t PDUlen;
	uint16_t recv1;
	//receive pdu length
	if ((recv1 = recv(clientSocket, &PDUlen, 2, MSG_WAITALL)) < 0) {
		perror("recieve PDU length:");
		exit(-1);
	}

	// //connection closed
	if (recv1 == 0) {
		return 0;
	}
	memcpy(dataBuffer, &PDUlen, 2);
	PDUlen = ntohs(PDUlen);

	// //no space for buffer to store PDU
	if (PDUlen > bufferLen) {
		printf("PDU length %d is larger than data buffer size %d\n", PDUlen, bufferLen);
		exit(-1);
	}

	//receive actual PDU
	if ((numbytes = recv(clientSocket, dataBuffer+2, PDUlen-2, MSG_WAITALL)) < 0) {
		perror("recieve PDU");
		exit(-1);
	}
	// printf("received pdu %d\n", PDUlen);
	return numbytes;
}

int sendBuf(int socketNumber, uint8_t * dataBuffer, int lengthOfData){
	uint16_t numbytes;
	// uint16_t PDUlen = lengthOfData+2;
	uint16_t PDUlen = lengthOfData;
	uint8_t PDUbuffer[1500];
	// printf("send pdu len: %d \n", PDUlen);
	
	PDUlen = htons(PDUlen);
	//set up PDU to send
	// smemcpy(PDUbuffer, &PDUlen, 2);
	// smemcpy(PDUbuffer+2, dataBuffer, lengthOfData);
	smemcpy(PDUbuffer, dataBuffer, lengthOfData);

	if ((numbytes = send(socketNumber, PDUbuffer, lengthOfData, 0)) < 0) {
		perror("send PDU");
		exit(-1);
	}
	return numbytes;

}


