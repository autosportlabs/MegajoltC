/*
 * mod_string.c
 *
 *  Created on: Oct 19, 2012
 *      Author: brent
 */
#include "mod_string.h"

void * memcpy(void * __restrict s1, const void * __restrict s2, size_t n)
{
	register char *r1 = s1;
	register const char *r2 = s2;

	while (n) {
		*r1++ = *r2++;
		--n;
	}
	return s1;
}

void *memset(void *s, int c, size_t n)
{
	register unsigned char *p = (unsigned char *) s;

	while (n) {
		*p++ = (unsigned char)c;
		--n;
	}
	return s;
}


int strncmp(register const char *s1, register const char *s2, size_t n)
{
	int r = 0;

	while (n--
		   && ((r = ((int)(*((unsigned char *)s1))) - *((unsigned char *)s2++))
			== 0)
		   && *s1++);

	return r;
}

char *strcpy(char * __restrict s1, const char * __restrict s2)
{
	register char *s = s1;

	while ( (*s++ = *s2++) != 0 );

	return s1;
}

size_t strlen(const char *s)
{
	register const char *p;

	for (p=s ; *p ; p++);

	return p - s;
}

char *strncpy(char * __restrict s1, register const char * __restrict s2, size_t n)
{
	register char *s = s1;

	while (n) {
		if ((*s = *s2) != 0) s2++; /* Need to fill tail with 0s. */
		++s;
		--n;
	}

	return s1;
}

int strcmp(register const char *s1, register const char *s2)
{
	int r;

	while (((r = ((int)(*((unsigned char *)s1))) - *((unsigned char *)s2++))
			== 0) && *s1++);

	return r;
}

char *strtok(char * __restrict s1, const char * __restrict s2)
{
	static char *next_start;	/* Initialized to 0 since in bss. */
	return strtok_r(s1, s2, &next_start);
}

char *strtok_r(char * __restrict s1, const char * __restrict s2, char ** __restrict next_start)
{
	register char *s;
	register char *p;

	if (((s = s1) != NULL) || ((s = *next_start) != NULL)) {
		if (*(s += strspn(s, s2))) {
			if ((p = strpbrk(s, s2)) != NULL) {
				*p++ = 0;
			}
		} else {
			p = s = NULL;
		}
		*next_start = p;
	}
	return s;
}

size_t strspn(const char *s1, const char *s2)
{
	register const char *s = s1;
	register const char *p = s2;

	while (*p) {
		if (*p++ == *s) {
			++s;
			p = s2;
		}
	}
	return s - s1;
}

char *strpbrk(const char *s1, const char *s2)
{
	register const char *s;
	register const char *p;

	for ( s=s1 ; *s ; s++ ) {
		for ( p=s2 ; *p ; p++ ) {
			if (*p == *s) return (char *) s; /* silence the warning */
		}
	}
	return NULL;
}


