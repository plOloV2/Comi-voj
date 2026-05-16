#ifndef DATA_PREPARE_H
#define DATA_PREPARE_H

#include "libs.h"

uint32_t* parse_file(char* file_path, size_t* num_points);

uint32_t* create_random_distances(size_t num_points, xoshiro256_state* xos_state);

uint32_t*** create_random_distances_for_calc();

uint32_t* read_data_from_TSPLIB(char* file_path, size_t* num_points, uint8_t silence_mode);

#endif
