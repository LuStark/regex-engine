#include "LLex.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

linkNode
Init_EntityTree( RegexEntity ent )
{
    linkNode root;
    
    root = malloc( sizeof(struct _node) );

    if( !root ){
        printf("内存分配失败!\n");
        exit(1);
    }
    root->regex_entity = ent;
    root->key = malloc( sizeof(char) * strlen(ent.name) );
    if( !root->key ){
        printf("内存分配失败!\n");
        exit(1);
    }
    strcpy( root->key, ent.name );

    return root;
}

bool 
Insert_EntTree( linkNode root, RegexEntity ent )
{
    linkNode    p, q, newnode;
    int     cmpValue;

    q = p = root;
    while( q )
    {
        p = q;
        cmpValue = strcmp( ent.name, p->key );
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
    newnode -> key = malloc( sizeof(char) * strlen(ent.name) );
    if( !newnode->key )
    {
        printf("内存分配失败!\n");
        exit(1);
    }
    newnode->regex_entity = ent;

    if( cmpValue == -1 )
        p -> lChild = newnode;
    else
        p -> rChild = newnode;

    return true;
}

linkNode
FindEntNode( linkNode root, char *name )
{
    linkNode    p,q;
    int     cmpValue;

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

xchar_t
*getRegexFromNode( const linkNode p )
{
    if(!p){
        return NULL;
    }
    return p->regex_entity.regex;
}

