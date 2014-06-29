#include "vector.h"

int main()
{
    vector_int  v3,v1,v2;
    int a[]={1,2,3,4,5};

    initByArray(&v1,a,sizeof(a)/sizeof(int));
    v2  =  copyVector( v1 );

    printVector( v2 );
}
