#ifndef DFA_CONSTRUCT
#define DFA_CONSTRUCT

#include "NFA.h"

typedef struct Automaton *DFA;

/* NFA转换为DFA */
extern DFA Subset_Construct (NFA);


extern DFA MINI_Status_for_DFA (DFA);

extern DFA MINIMUN_DFAStatus (DFA);

extern DFA CreateDFA(int n, int e);

extern void addEdge(DFA dfa, Edge e);

#endif
