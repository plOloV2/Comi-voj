#include "libs.h"
#include "algorithms.h"
#include "UI/TUI_func.h"
#include "data_operations/data_prepare.h"

int main(){


    char file_loc[64];
    size_t num_points = 0;

    startup_sentence(file_loc, &num_points);

    // Point* points = parse_file(file_loc, &num_points);

    // Distance** distances_u = calc_dist_table(points, num_points, 0);

    // Route* naive = find_naive_route(distances_u, num_points);

    // fprintf(stdout, "res: %lu\n", naive->distance_u);

    return 0;

}
