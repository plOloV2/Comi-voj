#ifndef ROUTE_STRUCTS_H
#define ROUTE_STRUCTS_H

#include "libs.h"

typedef struct{
    double      time;
    uint64_t    distance_u;
    uint8_t*    city_order;
}Route;

#endif
