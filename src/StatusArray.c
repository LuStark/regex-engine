#include "StatusArray.h"
#include "Status.h"

#include <stdlib.h>
#include <stdio.h>

struct Status_Array
{
    size_t  length;
    size_t  capability;
    Status  **array;
};


Status_Array*
create_status_array(size_t len)
{
    int     i;
    Status_Array    *array;
    Status  *status;
    
    array = malloc(sizeof(struct Status_Array));
    assert(array);
    
    if (len == 0)
    {
        array -> length = 0;
        array -> capability = 0;
        array -> array = NULL;
    }
    else
    {
        array -> length = len;
        array -> capability = 2 * len;

        array -> array = malloc(sizeof(Status*) * array -> capability);
        assert(array -> array);

        for (i=0; i<len; i++)
        {
            //TODO: create new status object for each position or set NULL?
            array -> array[i] = NULL;
            //status = array->array[i];
            //init_status(&status);
        }
    }
    return array;
}

size_t
get_statuses_length(Status_Array *array)
{
    assert(array);
    return array->length;
}

Status*
get_status(Status_Array *array, int i)
{
    assert(array && array->array);
    assert(i < array->length);
    
    return array->array[i];
}

void
put_status(Status_Array **array, int i, Status *status)
{
    assert((*array) && (*array) -> array);
    assert(i < (*array) -> length);

    (*array) -> array[i] = status;
}

static void
resize_status_array(Status_Array **array)
{
    assert(*array);
    (*array) -> capability *= 2;
    (*array) -> array = realloc((*array) -> array, 
                   size_of_status() * (*array) -> capability);
}


void
append_status(Status_Array **array, Status *status)
{
    assert(*array);
    if ((*array) -> length == (*array) -> capability)
    {
        resize_status_array(array);
    }
    (*array) -> array[(*array) -> length] = status;   
}


