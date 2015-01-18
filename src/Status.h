#ifndef STATUS_INCLUEDED
#define STATUS_INCLUEDED

#include <wchar.h>
#include <stdbool.h>

typedef struct EdgeArray    EdgeArray;
typedef struct Status       Status;
typedef struct Edge         Edge;

extern Status*      create_status ();
extern void         init_status(Status **s);

extern EdgeArray*   get_inEdges (Status*);
extern EdgeArray*   get_outEdges (Status*);

extern int  get_status_id (Status*);

extern void set_status_id (Status**, int);
extern void set_as_final_status (Status**);

extern void cancel_final_status (Status**);

extern void append_inEdge (Status**, Edge*);
extern void append_outEdge (Status**, Edge*);

extern bool is_final_status (Status*);
extern int  size_of_status();

#endif
