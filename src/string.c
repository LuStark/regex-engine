#include "string.h"

struct CharArray
{
    wchar_t *array;
};

String create_string(const wchar_t *cstr)
{
    String string;
    string = malloc(sizeof(struct CharArray));

    assert(string);

    if (cstr == NULL)
    {
        string -> array = NULL;
    }
    else
    {
        string -> array = malloc(sizeof(wchar_t) * (wcslen(cstr) + 1));    
        wcscpy(string -> array, cstr);
    }
    return string;
}

size_t  get_string_length(String string)
{
    assert(string);
    if (string -> array == NULL)
        return 0;
    else
        return wcslen(string -> array);
}

wchar_t getChar(String string, int i)
{
    assert(string && string->array);
    return string->array[i];
}

void    setChar(String *string, int i, wchar_t c)
{
    assert((*string) && (*string)->array);
    (*string) -> array[i] = c;
}

void    push_back(String *string, wchar_t c)
{
    size_t  len;
    assert(*string);

    len = wcslen((*string) -> array); 
    if (len == 0)
    {
        (*string) -> array = malloc(sizeof(wchar_t) * 2);
        (*string) -> array[0] = c;
        (*string) -> array[1] = L'\0';
    }else{
        (*string) -> array = realloc((*string) -> array, len + 2);
        (*string) -> array[len] = c;
        (*string) -> array[len+1] = L'\0';
    }

}

void    free_string(String *string)
{
    assert(*string);
    if ((*string) -> array != NULL)
        free((*string) -> array);
    (*string) -> array = NULL;
}
