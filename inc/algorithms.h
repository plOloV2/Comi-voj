#ifndef ALGORITHMS_H
#define ALGORITHMS_H

#include "libs.h"
#include "data_operations/route_struct.h"

Route* nearest_neighbour_fast(uint32_t* distances, size_t num_points, int start_point_id);

Route* nearest_neighbour_stack(uint32_t* distances, size_t num_points, int start_point_id);

Route* repetitive_nearest_neighbour(uint32_t* distances, size_t num_points, Route* (*NN_func)(uint32_t* distances, size_t num_points, int start_point_id));

Route* brute_force(uint32_t* distances, size_t num_points);

Route* rand_seq(uint32_t* distances, size_t num_points, int perms);

Route* branch_and_bound(uint32_t* distances, size_t num_points, double timeout_seconds, uint8_t mode);

Route* tabu_search(uint32_t* distances, size_t num_points, size_t max_iter, size_t sample_size, size_t max_no_up, uint8_t use_RNN, size_t min_iter_stop, size_t max_iter_stop, size_t tabu_limit);

#endif
