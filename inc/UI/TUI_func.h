#ifndef TUI_FUNC_H
#define TUI_FUNC_H

#include "libs.h"
#include "UI/ansi_terminal.h"
#include "data_operations/route_struct.h"

void print_error(char* erro_msg);

void greeter();

int start_choice();

void display_full_ran_info();

int display_test_menu();

void get_file_path(char* path);

int get_rand_point_num();

void disp_dist(uint32_t* dist, size_t num_points);

void get_algorithm(int32_t* choice_algorithm, alg_in_data* data);

void display_Route(Route* data, size_t num_points);

int check_data_created(uint32_t* dist, size_t num_points);

int32_t user_input(int32_t lower_bound, int32_t higher_bound);

#endif
