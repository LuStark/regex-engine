#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "Array.h"
#include "ArrayRep.h"

#define T Array_T


T Array_new (int length, int size)
{
    T array;

    /* 事实上这是一个指针值 */
    array = malloc (sizeof(*array));
    if (length > 0)
        ArrayRep_init(array, length, size, malloc(length*size));
    else
        ArrayRep_init(array, length, size, NULL);
    return array;
}

void ArrayRep_init (T array, int length, int size, void *ary)
{
    assert(array);
    assert(ary && length>0 || length==0 && ary==NULL);
    assert(size > 0);
    array->length = length;
    array->size   = size;

    if (length > 0)
        array->array = ary;
    else
        array->array = NULL;

}

void Array_free(T *array)
{
    assert(array && *array);
    free ((*array)->array);
    (*array)->array = NULL;
    free (*array);
    (*array)->length = 0;
    (*array)=NULL;
}

void setEmptyPt(T array)
{
    array->array=NULL;
}

void *Array_get(T array, int i)
{
    assert(array);
    assert(i >= 0 && i < array->length);
    return array->array + i*array->size;
}

void *Array_put(T array, int i, void *elem)
{
    assert(array);
    assert(i >= 0 && i < array->length);
    assert(elem);
    memcpy(array->array + i*array->size, elem, array->size);
    return elem;
}

int Array_length(T array)
{
    assert(array);
    return array->length;
}

int Array_size(T array)
{
    assert(array);
    return array->size;
}

void Array_resize(T array, int length)
{
    assert(array);
    assert(length >= 0);
    if (length==0)
        free(array->array);
    else if (array->length == 0)
        array->array = malloc (length*array->size);
    else
        array->array = realloc(array->array, length*array->size);
    array->length = length;
//    array->array[array->size*array->length] = '\0';
}

T Array_copy(T array, int length)
{
    T copy;

    assert(array);
    assert(length >= 0);
    copy = Array_new(length, array->size);
    if (copy->length >= array->length && array->length > 0)
        memcpy (copy->array, array->array, array->length*array->size);
    else if (array->length > copy->length && copy->length > 0)
        memcpy (copy->array, array->array,copy->length*array->size);
    return copy;
}

void Array_copy_from_range (T toArray, int start1, T fromArray, int start2, int copylength)
{
    assert (toArray);
    assert (fromArray);
    
    assert (copylength>0);

    if (start2+copylength > fromArray->length)
        copylength = fromArray->length - start2;

    if (start1+copylength > toArray->length)
        copylength = toArray->length - start1;

    memcpy (toArray->array + start1 * toArray->size, 
            fromArray->array + start2 * fromArray->size, 
            copylength*fromArray->size
            );

}


void Array_append(T array, void *elem)
{
    assert(array);

    if (array->length == 0)
    {
        array->array = malloc (1*array->size);
        array->length = 1;
        Array_put (array, 0, elem);
    }
    else
    {
        Array_resize (array, array->length+1);
        Array_put (array, array->length-1, elem);
    }
}

/*
void Array_free (T array)
{
    assert (array);
    if (array->array==NULL)
        return;
    free(array->array);
    array->array=NULL;
}
*/

int sizeOfArray()
{
    return sizeof(struct Array_T);
}


