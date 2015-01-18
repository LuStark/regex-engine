#include "EdgeArray.h"
#include "Edge.h"
#include <stdlib.h>

struct EdgeArray
{
    size_t  length;
    size_t  capability;
    Edge    **array;
};

EdgeArray*
create_edges_array(size_t len)
{
    int     i;
    EdgeArray   *array;
    Edge    *edge;
    
    array = malloc(sizeof(struct EdgeArray));
    assert(array);
    
    if (len == 0)
    {
        array -> array = NULL;
        array -> capability = array -> length = 0;  
    }
    else
    {
        array -> length = len;
        array -> capability = 2 * len;
    
        array -> array = malloc(sizeof(Edge*) * array->capability);
        assert(array -> array);
 
        for (i=0; i<len; i++)
        {
            array -> array[i] = NULL;
        }
    }
    return array;
}

int     get_edge_array_length(EdgeArray *array)
{
    return array -> length;
}

Edge*   get_edge(EdgeArray *array, int i)
{
    assert(array && array -> array);
    assert(i < array -> length);

    return array -> array[i];
}

void    put_edge(EdgeArray *array, int i, Edge *edge)
{
    assert(array && array -> array);
    assert(i < array->length);

    array -> array[i] = edge;
}

static void
resize_edge_array(EdgeArray **array)
{
    assert(*array);
    (*array) -> capability *= 2;
    (*array) -> array = realloc((*array) -> array, (*array) -> capability);
}

void    append_edge(EdgeArray **array, Edge *edge)
{
    assert(*array);
    assert(edge);
    if ((*array) -> length == (*array) -> capability)
    {
        resize_edge_array(array);
    }
    (*array) -> array[(*array) -> length] = edge;
}
