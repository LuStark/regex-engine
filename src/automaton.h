#ifndef AUTO_MATHON_H
#define AUTO_MATHON_H

//typedef struct Automaton *Automaton;

#include "NFA.h"
#include "Array.h"


typedef struct Automaton
{
    Status  start, end;
    Array_T statusArray;
    Array_T edgeArray;
    bool    yucha;
}Automaton;

extern  void     adjustStatusID  (Automaton*);
extern  void     link_Two_Status_In_Automaton (Automaton*, int, int, int);
extern  void     print_Automaton (Automaton*);
extern  void     free_Automaton  (Automaton*);
extern  Array_T  getStatusArray  (Automaton*);
extern  Array_T  getEdgeArray    (Automaton*);

extern  Status  getStartStatus  (Automaton*);
extern  Status  getEndStatus    (Automaton*);
extern  Edge    getEdge (Automaton*, int);
extern  Status  getStatus (Automaton*, int);

extern  Edge    getLink (Automaton*,int,int);
extern  bool    isYuCha (Automaton*);

#endif
