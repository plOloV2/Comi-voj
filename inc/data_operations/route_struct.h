#ifndef ROUTE_STRUCT_H
#define ROUTE_STRUCT_H

#include "libs.h"

typedef struct{
    double      time;
    uint64_t    distance_u;
    uint16_t*   city_order;
}Route;

#endif
