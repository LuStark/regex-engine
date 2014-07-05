#include "hash_for_wrap.h"
#include "status_set.h"
#include "NFA.h"

int
Hash( StatusSet sset )
{
    int     i, key;

    key = 0;
    for( i=0; i<sset.eps_closure.size; i++ )
      key += sset.eps_closure.Item[i];
    key %= MAX_HASH_SIZE;
    return key;
}

Hash_LinkNode   
Find( HashTable_For_Wrap *HT, StatusSet sset )
{
    int i, key;
    HashLinkList    header;
    Hash_LinkNode   p;

    key     =   Hash( sset );

    header  =   HT->slot[ key ];
    
    p = header;
    if( p==NULL )
        return NULL;
    while( p!=NULL )
    {
        /* 观察散列链中（所有冲突）的元素
         * 中与 sset 相等的元素
         */
        if(isEqual_StatusSet(p->wrap.statusSet, sset))
          return p;
        p = p->next;
    }
    return NULL;
}

bool    
HashTable_Insert( HashTable_For_Wrap *HT, Set_Status_Wrap Wrap)
{
    int     key;
    Hash_LinkNode   header;
    Hash_LinkNode   p;
    
    key     =   Hash( Wrap.statusSet );
    if( HT->slot[key] == NULL )
    {
        HT->slot[key] = malloc( sizeof(struct _hash_node) );
        if(!HT->slot[key]){
            Error("Memory exhausted!");
        }
        HT->slot[key]->wrap = Wrap; 
        return true;
    }
    header  =   HT->slot[key];
    for( p=header; p->next!=NULL; p=p->next )
        if( isEqual_StatusSet( (p->wrap.statusSet),Wrap.statusSet ) ) 
            return false;

    p->next = malloc( sizeof(struct _hash_node) );
    if( !p->next )
        Error("Memory exhausted!");
    p = p->next;
    p->wrap = Wrap;
    p->next = NULL;
    return true;

}
