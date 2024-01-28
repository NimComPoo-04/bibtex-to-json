#ifndef _B2J_H_
#define _B2J_H_

#include <stdio.h>
#include <stdlib.h>

// Utility structures and functions

typedef struct
{
	char *value;
	size_t length;
} string_t;

int nocasestrncmp(const char *this, const char *other, size_t len);

#endif
