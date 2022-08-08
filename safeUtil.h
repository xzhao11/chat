// 
// Writen by Hugh Smith, April 2020
//
// Put in system calls with error checking.

#ifndef __SAFEUTIL_H__
#define __SAFEUTIL_H__

void * srealloc(void *ptr, size_t size);
void * sCalloc(size_t nmemb, size_t size);
size_t sstrlen(char *str);
void *smemcpy(void *dest, const void *src, size_t n);
void *smemset(void *s, int c, size_t n);
void sstrcpy(char *dest, const char *src);
#endif