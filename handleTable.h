


#ifndef __HANDLETABLE_H__
#define __HANDLETABLE_H__

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "safeUtil.h"
#define INIT_TABLE_SIZE 10
#define INCRE_TABLE_SIZE 10
#define MAX_HANDLE_LEN 100
#define TRUE 1
#define FALSE -1
struct socket_handle{
   int socket;
   char handle[MAX_HANDLE_LEN + 1];
   int is_free;
} __attribute__((packed));

typedef struct socket_handle socket_handle;

typedef struct socket_handle socket_handle;
void initHandleTable();
void expandHandleTable(int new_capacity);
void printClients() ;
int addToHandleTable(int socket, char* handle);
int removeFromHandleTable(int socket);
int getTableSize();
int getTableCapacity();
void getTableHandles(char* handles[]);
void getTableSockets(int sockets[]);
int getSocketFromHandle(char *handle);
#endif
