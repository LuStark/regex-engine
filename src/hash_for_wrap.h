#ifndef _HASH_FOR_SET_H
#define _HASH_FOR_SET_H
#include "typedef.h"
#include "set_status_wrap.h"

#define MAX_HASH_SIZE   5000
#define Collusion_MAX_SIZE  100

typedef struct  _hash_node  *HashLinkList,*Hash_LinkNode;
typedef struct  _hashtable  HashTable_For_Wrap;

struct  _hash_node{
    Set_Status_Wrap         wrap;
    struct  _hash_node      *next;
};

struct  _hashtable{
    HashLinkList    slot[ MAX_HASH_SIZE ];
};

extern  int
Hash(   StatusSet sset );

extern  Hash_LinkNode 
Find( HashTable_For_Wrap *HT, StatusSet sset );

extern  bool
HashTable_Insert( HashTable_For_Wrap *HT, Set_Status_Wrap Wrap);

extern void
free_hash_table( HashTable_For_Wrap *HT );

extern void
free_hash_node( Hash_LinkNode *node );

#endif
