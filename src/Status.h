#ifndef STATUS_INCLUEDED
#define STATUS_INCLUEDED

#include "typedef.h"
#include "Array.h"

typedef struct Status *Status;
#define S Status


#include "Edge.h"

/* 产生一个状态点 */
extern S    allocStatus ();

extern Array_T allocStatusArray(int n);
extern void outputStatus (S);
// 调试专用函数
extern S    nextStatus (S, wchar_t);

extern void setStatusID (S s, int id);

extern Array_T getInEdges(S s);
extern Array_T getOutEdges(S s);

extern int  getStatusID (S s);
extern void ensureFinalStatus (S s);
extern void cancelFinalStatus (S s);
extern void appendInEdge (S s, int e);
extern void appendOutEdge (S s, int e);

extern bool  isFinalStatus (S s);
extern int sizeOfStatus();

extern void initInEdges(S s);
extern void initOutEdges(S s);
extern void freeStatus(S s);

extern void freeEdgesInStatus (S s);
extern void clearEdges(S s);


#undef S
#endif
