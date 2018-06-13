#pragma once

#include <cglm\cglm.h>

int asprintf (char **, const char *, ...);
int vasprintf (char **, const char *, va_list);
char* load_string_from_file(const char*);