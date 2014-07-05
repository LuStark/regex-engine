#ifndef _SET_STATUS_WRAP_H
#define _SET_STATUS_WRAP_H

#include    "status_set.h"

typedef struct  _set_status {
    StatusSet   statusSet;
    Status      *status;
}Set_Status_Wrap;

#endif
