#ifndef DATA_STRUCTS_H
#define DATA_STRUCTS_H

#include "libs.h"

typedef struct{
    int16_t x;
    int16_t y;
}Point;

typedef struct{
    double  time;
    union{
        uint64_t    distance_u;
        double      distance_d;
    };
    uint8_t*    points_list;
    uint8_t*    visited;
}Route;

typedef union{
    uint32_t    dist_u;
    float       dist_f; 
}Distance;

#endif
