#ifndef _VECTOR_H_
#define _VECTOR_H_
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#define     ALLOCATED_INTs  4
#define     MAXVECTORLEN          250
//#define     vector_int#a    4; 

typedef struct  {
    int     *Item;
    int     size;        /* 实际int 数 */
    int     allocedInts; /* 已经分配的int数*/
} vector_int;

void    
initialize( vector_int *v );

void
initByArray( vector_int *v,int [],int );


void 
freeVector(   vector_int *v );

vector_int 
copyVector( vector_int );

void    
append( vector_int *, int );

void
printVector( vector_int );

vector_int  
UnionVector( vector_int,  vector_int );


#endif
