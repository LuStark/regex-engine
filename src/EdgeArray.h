#ifndef EDGE_ARRAY
#define EDGE_ARRAY

#include <stddef.h>


typedef struct EdgeArray EdgeArray;
typedef struct Edge Edge;

extern EdgeArray*
create_edges_array(size_t len);

extern int
get_edge_array_length(EdgeArray *array);

extern Edge*
get_edge(EdgeArray *array, int i);

extern void
put_edge(EdgeArray *array, int i, Edge *edge);

extern void
append_edge(EdgeArray **array, Edge *edge);

#endif
