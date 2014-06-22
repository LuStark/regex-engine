#ifndef _QUEUE_H
#define _QUEUE_H

#include "global.h"

#include "typedef.h"
#include "NFA.h"
#include "set_status_wrap.h"

typedef struct _status_queue {
    Status      *Item[ 1000 ];
    int         front,rear;
} Status_Queue; 

typedef struct _statusset_queue {
    StatusSet   Item[ 1000 ];
    int         front,rear;
} StatusSet_Queue;

typedef struct _wrap_queue {
    Set_Status_Wrap Item[ 1000 ];
    int         front,rear;
}Set_Status_Wrap_Queue;

typedef struct _int_queue{
    int     Item[ 1000 ];
    int     front,rear;
}Queue_Int;


#define Enqueue(Q,a)\
    (Q).Item[((Q).rear)++]=(a)

#define Dequeue(Q)\
    (Q).Item[((Q).front)++]

#define EmptyQueue(Q)\
    (((Q).front)>=((Q).rear))

#define Initialize_Queue(Q) (Q).front=(Q).rear=0

#endif
