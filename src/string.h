#ifndef HANDWRITE_STRING
#define HANDWRITE_STRING

#include <stdlib.h>
#include <wchar.h>
#include <stddef.h>

typedef struct CharArray *String;

extern String
create_string(const wchar_t *cstr);

extern size_t
get_string_length(String string);

extern wchar_t
getChar(String string, int i);

extern void
setChar(String *string, int i, wchar_t c);

extern void
push_back(String *string, wchar_t c);

extern void
free_string(String *string);

#endif
