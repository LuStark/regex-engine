#ifndef STATUS_ARRAY
#define STATUS_ARRAY

#include <stddef.h>
//#include "Status.h"

typedef struct Status Status;
typedef struct Status_Array Status_Array;


extern Status_Array*
create_status_array(size_t len);

extern size_t 
get_statuses_length(Status_Array *array);

extern Status*
get_status(Status_Array *array, int i);

extern void
put_status(Status_Array **array, int i, Status *status);

extern void
append_status(Status_Array **array, Status *status);

#endif
