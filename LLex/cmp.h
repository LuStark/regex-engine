/* This file declares some function used
 * for compare some struct 
 */

#ifndef _CMP_H
#define _CMP_H
#include <wchar.h>

extern int  
wstrcmp_allow_nullpointer(wchar_t *,wchar_t*);

extern int
strcmp_allow_nullpointer(wchar_t *,wchar_t*)
;

#endif
