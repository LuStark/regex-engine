#include "Set.h"

static void Append(Set *s,int a)
{
    if( s->num < MAXNUM_STATES )
        s->a[ s->num++ ] = a;
}

static int Size(Set *s)
{
    return s->num;
}

static int Visit(Set *s,int i)
{
    return s->a[i]; 
}

void initSet(Set *s)
{
    s->num  = 0;
    s->add  = Append;
    s->size = Size;
    s->visit = Visit;
}

