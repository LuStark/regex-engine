#include "Status.h"
#include "EdgeArray.h"

#include <stdlib.h>
#include <stdio.h>
#include <assert.h>

typedef struct Status Status;

struct Status
{
    EdgeArray   *InEdges;
    EdgeArray   *OutEdges;
    bool        FinalStatus;
    int ID;
};

void init_status(Status **s)
{
    assert(*s);
    (*s) -> InEdges = create_edges_array(0);
    (*s) -> OutEdges = create_edges_array(0);
}

int size_of_status()
{
    return sizeof(struct Status);
}

Status*
create_Status ()
{
    Status *s;
    s = malloc(sizeof(struct Status));
    assert(s);
 
    s -> InEdges = create_edges_array(0);
    s -> OutEdges = create_edges_array(0);
    return s;
}


int  get_status_id(Status *s)
{
    assert(s);
    return s->ID;
}

EdgeArray* get_inEdges(Status *s)
{
    assert(s && s->InEdges);
    return s->InEdges;
}

EdgeArray* get_outEdges(Status *s)
{
    assert(s && s->OutEdges);
    return s->OutEdges;
}


void set_status_id (Status **s, int id)
{
    assert(*s);
    (*s) -> ID = id;
}

void set_as_final_status (Status **s)
{
    assert(*s);
    (*s) -> FinalStatus = true;
}

void cancel_final_status (Status **s)
{
    assert(*s);
    (*s) -> FinalStatus = false;
}

void append_inEdge (Status **s, Edge *e)
{
    assert((*s) && e);
    append_edge(&((*s)->InEdges), e);
}

void append_outEdge (Status **s, Edge *e)
{
    assert((*s) && e);
    append_edge(&((*s) -> OutEdges), e);
}

bool is_final_status (Status *s)
{
    assert(s);
    return s->FinalStatus;
}

