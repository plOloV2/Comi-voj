#ifndef DATA_PREPARE_H
#define DATA_PREPARE_H

#include "libs.h"
#include "data_structs.h"

Point* parse_file(char* file_path, int* num_points);

uint32_t** calc_dist_table(Point* points, int num_points);

Route* find_naive_route(uint32_t** distances, int num_points);

#endif
