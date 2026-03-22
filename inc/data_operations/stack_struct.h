#ifndef STACK_STRUCT_H
#define STACK_STRUCT_H

#include "libs.h"

typedef struct{
    uint64_t    distance_u;
    uint8_t*    city_order;
    uint8_t*    visited;
    size_t      depth;
    int         current_city;
} Path_state;

typedef struct{
    Path_state* stack;
    size_t      stack_size;
    int         stack_top;
} Path_stack;

Path_stack* init_stack(size_t num_points);

void free_stack(Path_stack* Stack);

#endif
