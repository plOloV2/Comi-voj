#ifndef DATA_STRUCTS_H
#define DATA_STRUCTS_H

#include "libs.h"

typedef struct{
    int16_t x;
    int16_t y;
}Point;

typedef struct{
    int64_t distance;
    uint8_t* points_list;
    uint8_t* visited;
}Route;

#endif
