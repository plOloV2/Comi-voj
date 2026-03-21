#ifndef DATA_PREPARE_H
#define DATA_PREPARE_H

#include "libs.h"
#include "data_structs.h"

uint32_t* parse_file(char* file_path, size_t* num_points);

uint32_t* create_random_distances(size_t num_points, xoshiro256_state* xos_state);

#endif
