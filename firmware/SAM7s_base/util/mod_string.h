/*
 * mod_string.h
 *
 *  Created on: Oct 19, 2012
 *      Author: brent
 */

#ifndef MOD_STRING_H_
#define MOD_STRING_H_
#include <stddef.h>

void *memcpy(void *__restrict __dest, __const void *__restrict __src, size_t __n);

void *memset(void *s, int c, size_t n);

int strncmp(register const char *s1, register const char *s2, size_t n);

char *strcpy(char * __restrict s1, const char * __restrict s2);

size_t strlen(const char *s);

char *strncpy(char * __restrict s1, register const char * __restrict s2, size_t n);

int strcmp(register const char *s1, register const char *s2);

char *strtok(char * __restrict s1, const char * __restrict s2);

char *strtok_r(char * __restrict s1, const char * __restrict s2, char ** __restrict next_start);

size_t strspn(const char *s1, const char *s2);

char *strpbrk(const char *s1, const char *s2);

#endif /* MOD_STRING_H_ */
