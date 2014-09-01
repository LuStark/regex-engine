#ifndef _TYPEDEF_H_
#define _TYPEDEF_H_

#include <wchar.h>

typedef wchar_t xchar_t;
typedef short   Index;

typedef unsigned long long int bitVector;
//typedef xchar_t *_xString ;

// 表示字符的范围。
typedef struct {
    xchar_t from,to;  
}Range;



/*  正确返回1 */
#define RangeEqual(r1,r2)\
    (r1.from==r2.from)&&(r1.to==r2.to)

#endif
