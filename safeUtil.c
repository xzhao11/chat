
// 
// Writen by Hugh Smith, April 2020
//
// Put in system calls with error checking
// and and an s to the name: srealloc()
// keep the function paramaters same as system call

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "safeUtil.h"


void * srealloc(void *ptr, size_t size)
{
	void * returnValue = NULL;
	
	if ((returnValue = realloc(ptr, size)) == NULL)
	{
		printf("Error on realloc (tried for size: %d\n", (int) size);
		exit(-1);
	}
	
	return returnValue;
} 

void * sCalloc(size_t nmemb, size_t size)
{
	void * returnValue = NULL;
	if ((returnValue = calloc(nmemb, size)) == NULL)
	{
		perror("calloc");
		exit(-1);
	}
	return returnValue;
}

size_t sstrlen(char *str){
   size_t len = strlen(str);

   if(len < 0){
      perror("strlen");
      exit(-1);
   }
   return len;
}


void *smemcpy(void *dest, const void *src, size_t n){
   void *ptr = NULL;
   ptr = memcpy(dest, src, n);
   
   if(ptr == NULL){
      perror("memcpy");
      exit(-1);
   }

   return ptr;
}

void *smemset(void *s, int c, size_t n){
   void *ptr = NULL;
   ptr = memset(s, c, n);

   if(ptr == NULL){
      perror("memset");
      exit(-1);
   }

   return ptr;
}

void sstrcpy(char *dest, const char *src){
   void *ptr = NULL;
   ptr = strcpy(dest, src);
   if(ptr == NULL){
      perror("strcpy");
      exit(-1);
   }
}