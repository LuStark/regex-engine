#include "LLex.h"
#include <stdio.h>
#include <stdlib.h>

linkNode
*Init_EntityTree( MatchEntity ent )
{
    linkNode root;
    
    root = malloc( sizeof(struct _node) );

    if( !root ){
        printf("内存分配失败!\n");
        exit(1);
    }
    root->match_entity = ent;
    root->key = malloc( sizeof(char) * ent.entityName );
    if( !root->key ){
        printf("内存分配失败!\n");
        exit(1);
    }
    strcpy( root->key, ent.entityName );

    return root;
}

bool 
Insert_EntTree( linkNode root, MatchEntity ent )
{
    linkNode    p, q, newnode;
    int     cmpValue;

    q = p = root;
    while( q )
    {
        p = q;
        cmpValue = strcmp( ent.entityName, p->key );
        if( cmpValue == -1 )
            q = p -> lChild;
        else if( cmpValue == 1 )
            q = p -> rChild;
        else
            return false; 
    }
    newnode = malloc( sizeof(struct _node) );
    if( !newnode ){
        printf("内存分配失败!\n");
        exit(1);
    }
    newnode -> key = malloc( sizeof(char) * ent.entityName );
    if( !newnode->key )
    {
        printf("内存分配失败!\n");
        exit(1);
    }
    newnode.match_entity = ent;

    if( cmpValue == -1 )
        p -> lChild = newnode;
    else
        p -> rChild = newnode;

    return true;
}

linkNode
*Find( linkNode root, char *name )
{
    linkNode    p,q;

    q = p = root;

    while( q )
    {
        p = q;
        cmpValue = strcmp( name, p->key );
        if( cmpValue == -1 )
            q = p->lChild;
        else if( cmpValue == 1 )
            q = p->rChild;
        else
            return p;
    }
    return NULL;

}
