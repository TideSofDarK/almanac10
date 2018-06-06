#include "util.h"

#include <stdio.h>
#include <stdlib.h>

#include <string.h>
#include <malloc.h>

char* util_strcat( char* dest, char* src )
{
	while (*dest) dest++;
	while (*dest++ = *src++);
	return --dest;
}

char* load_string_from_file(const char* filename)
{
	FILE* f;
	fopen_s(&f, filename, "rb");
	char* buf = NULL;
	if (!f)
	{
		fprintf(stderr, "Could not open shader file: %s\n", filename);
		return NULL;
	}
	else
	{
		fseek(f, 0, SEEK_END);
		unsigned int len = ftell(f);
		buf = (char*)malloc(1 + len * sizeof(char));
		fseek(f, 0, SEEK_SET);
		if (buf)
		{
			fread(buf, sizeof(char), len, f);
		}
		else
		{
			fprintf(stderr, "Could not allocate memory for shader\n");
			return NULL;
		}
		buf[len] = '\0';
	}
	return buf;
}