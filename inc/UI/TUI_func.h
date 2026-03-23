#ifndef TUI_FUNC_H
#define TUI_FUNC_H

#include "libs.h"
#include "UI/ansi_terminal.h"
#include "data_operations/route_struct.h"

void print_error(char* erro_msg);

void greeter();

int start_choice();

int display_test_menu();

void get_file_path(char* path);

int get_rand_point_num();

void disp_dist(uint32_t* dist, size_t num_points);

void get_algorithm(int* choice_algorithm, int* num_permutations);

void display_Route(Route* data, size_t num_points);

int check_data_created(uint32_t* dist, size_t num_points);

#endif
