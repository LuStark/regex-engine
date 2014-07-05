#include "vector.h"


void
initialize( vector_int *v ){
    v->allocedInts = v->size = 0;
}

void
initByArray(vector_int *v,int a[],int size)
{
    int i;
    initialize( v );
    
    v->Item = malloc(sizeof(int)*size);
    if( v->Item==NULL )
    {
        printf("内存吃撑了.\n");
        while(1){}
    }
    v->size = size;
    v->allocedInts = size;
    for( i=0; i<size; i++)
      v->Item[i] = a[i];
}

void    freeVector( vector_int *v )
{
    free( v->Item );
}

vector_int
copyVector( vector_int  v )
{
    int     i;
    vector_int  newVector;
    
    if( v.size > 0 )
    {
        newVector.size = v.size ;
        newVector.Item=malloc(sizeof(int)*v.size);
        for( i = 0; i < newVector.size; i++ )
            newVector.Item[i] = v.Item[i];
    }
    newVector.allocedInts = v.allocedInts;

    return newVector;
}

void
append( vector_int  *v, int item )
{
    void    *p;

    if( v->size == v->allocedInts )
    {
        if( v->size == 0 )
        {
            v->Item = malloc( sizeof(int) * ALLOCATED_INTs );
            v->allocedInts += ALLOCATED_INTs;
        }
        else
        {
            v->allocedInts += ALLOCATED_INTs;
            p   = realloc( v->Item, sizeof(int) * v->allocedInts );
            if( p== NULL )
            {
                printf("新空间的分配失败。\n");
                while(getchar()){}
            }
            //if( p!= v->Item )
                //printf("新地址为:%x\n", p );
            v->Item = p ;
        }
    }
    v->Item[ v->size++ ] = item ;
}

void
printVector( vector_int v )
{
    int i;
    for( i = 0; i < v.size; i++ )
        printf("%d\n",v.Item[i]);
}

vector_int
UnionVector(vector_int v1, vector_int v2)
{
    int     i, bitSet[ MAXVECTORLEN ]; 
    vector_int  union_vector;

    initialize( &union_vector );

    for( i = 0; i < MAXVECTORLEN; i++ ) bitSet[ i ] = 0;
    for( i = 0; i < v1.size; i++ )
      bitSet[ v1.Item[i] ] = 1;

    for( i = 0; i < v2.size; i++ )
      bitSet[ v2.Item[i] ] = 1;
      
    for( i = 0; i < MAXVECTORLEN; i++ )
      if( bitSet[ i ] == 1 )
        append( &union_vector, i );     /* 涉及了动态内存分配 */ 

    return union_vector;
}
