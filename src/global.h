#ifndef _GLOBAL_H
#define _GLOBAL_H

#include "typedef.h"
//#include "NFA.h"
#include "hash_for_wrap.h"
#include "set_status_wrap.h"
#include "scan.h"
#include "nfa_to_dfa.h"
#include "status_set.h"
#include <stdio.h>
#include <wchar.h>
#include "constant.h"

extern xchar_t  *RegExp[ MAXNUM_REGEXP ] ;
extern int       numOfRegExps ;
extern Index     current_RegExp_Index ;
extern Index     current_RegExpArray_Index ;

#endif

