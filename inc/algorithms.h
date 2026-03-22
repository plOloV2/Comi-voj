#ifndef ALGORITHMS_H
#define ALGORITHMS_H

#include "libs.h"
#include "data_operations/data_structs.h"

Route* nearest_neighbour(uint32_t* distances, size_t num_points, int start_point_id);

Route* repetitive_nearest_neighbour(uint32_t* distances, size_t num_points);

#endif
