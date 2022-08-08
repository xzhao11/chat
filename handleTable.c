


#include "handleTable.h"
#include "safeUtil.h"
socket_handle* table;
int size;
int capacity;

//init empty table
void initHandleTable() {
	socket_handle* entry;
	char handle[MAX_HANDLE_LEN] = "\0";
	table = sCalloc(INIT_TABLE_SIZE, sizeof(socket_handle));
	capacity = INIT_TABLE_SIZE;
	size = 0;
	int i;
	for (i = 0; i<capacity; i++) {
		//init empty entry
		entry = &table[i];
		entry->socket = -1;
		smemcpy(entry->handle, handle, sizeof(handle));
		entry->is_free = TRUE;
	}

}

//expand table to new capacity
void expandHandleTable(int new_capacity) {
	table = srealloc(table, new_capacity* sizeof(socket_handle));
	socket_handle* entry;
	char handle[MAX_HANDLE_LEN] = "\0";
	int i;
	for (i = capacity; i < new_capacity; i++) {
		entry = &table[i];
		entry->socket = -1;
		smemcpy(entry->handle, handle, sizeof(handle));
		entry->is_free = TRUE;
	}
	capacity = new_capacity;
}

//return the table size, which is the number of handles 
//that is valid
int getTableSize() {
	return size;
}

//get the table's capacity
int getTableCapacity() {
	return capacity;
}

//get a list of table handles names
void getTableHandles(char* handles[]) {
	socket_handle* entry;
	int count = 0;
	int i;
	for (i = 0; i<capacity; i++) {
		entry = &table[i];
		if (entry->is_free == FALSE) {
			handles[count] = entry->handle;
			count++;
		}
		
	}
}

//get a list of sockets numbers
void getTableSockets(int sockets[]) {
	socket_handle* entry;
	int count = 0;
	int i;
	for (i = 0; i<capacity; i++) {
		entry = &table[i];
		if (entry->is_free == FALSE) {
			sockets[count] = entry->socket;
			count++;
		}
	}
}

//check if table contains handle
int containHandle(char* handle) {
	socket_handle* entry;
	int i;
	for (i = 0; i<capacity; i++) {
		entry = &table[i];
		if (strcmp(entry->handle, handle) == 0) {
			return 1;
		}
	}
	return 0;
}

//check if table contains sockets
int containSocket(int socket) {
	socket_handle* entry;
	int i;
	for (i = 0; i<capacity; i++) {
		entry = &table[i];
		if (entry->socket == socket) {
			return 1;
		}
	}
	return 0;
}

//print all handles and its socket number
void printClients() {
	socket_handle* entry;
	int i;
	for (i = 0; i<capacity; i++) {
		entry = &table[i];
		if (entry->is_free == FALSE) {
			printf("handle %d %s %d\n", i, entry->handle, entry->socket);
		}
	}
}

//add a entry of socket number to handle name to table
int addToHandleTable(int socket, char* handle) {
	socket_handle* entry;
	if (containHandle(handle) == 1) {
		return -1;
	}
	//if full, expand the table
	if (size == capacity) {
		expandHandleTable(capacity + INCRE_TABLE_SIZE);
	}
	int i;
	for (i = 0; i<capacity; i++) {
		entry = &table[i];
		//find the next empty entry
		if (entry->is_free == TRUE) {
			entry->socket = socket;
			smemcpy(entry->handle, handle, sstrlen(handle)+1);
			entry->is_free = FALSE;
			size++;
			break;
		}
		
	}
	return 1;
}

//remove the socket's entry from the table
int removeFromHandleTable(int socket) {
	char handle[MAX_HANDLE_LEN] = "\0";
	socket_handle* entry;
	if (size == 0 || containSocket(socket) == 0) return -1;
	int i;
	for (i = 0; i< capacity; i++) {
		entry = &table[i];
		if (entry->socket == socket) {
			entry->socket = -1;
			smemcpy(entry->handle, handle, sizeof(handle));
			entry->is_free = TRUE;
			size--;
			break;
		}
	}
	
	return 1;
}

//get socket from handle name
int getSocketFromHandle(char *handle) {
	socket_handle* entry;
	int i;
	for (i = 0; i<capacity; i++) {
		entry = &table[i];
		if (strcmp(entry->handle, handle) == 0) {
			return entry->socket;
		}
	}
	return -1;
}
