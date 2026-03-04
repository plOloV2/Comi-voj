#ifndef DATA_PREPARE_H
#define DATA_PREPARE_H

#include "libs.h"
#include "data_structs.h"

Point* parse_file(char* file_path, size_t* num_points);

Point* create_random_points(size_t num_points, xoshiro256_state* xos_state);

Distance** calc_dist_table(Point* points, size_t num_points, int bool_float_distance);

#endif
