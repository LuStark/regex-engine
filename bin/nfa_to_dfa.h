#ifndef _NFA_TO_DFA_H
#define _NFA_TO_DFA_H

#include "NFA.h"
#include "queue.h"
#include "status_set.h"
#include "constant.h"
#include "set_status_wrap.h"
#include "hash_for_wrap.h"
#include "Set.h"

extern DFA
*Subset_Construct( NFA *nfa );

extern DFA  
*MINI_Status_for_DFA( DFA *dfa );

extern DFA
*MINIMUN_DFAStatus( DFA *RawDfa );

#endif
