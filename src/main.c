#include "libs.h"
#include "algorithms.h"
#include "UI/TUI_func.h"
#include "data_operations/data_prepare.h"

int main(){


    char file_loc[256];
    size_t num_points = 0;

    int conf = startup_sentence(file_loc, &num_points);

    int choosen_alg =  conf & 0xff;
    int data_source = (conf >> 8) & 0xff;
    int num_runs    =  conf >> 16;

    // Point* points = parse_file(file_loc, &num_points);

    // Distance** distances_u = calc_dist_table(points, num_points, 0);

    // Route* naive = find_naive_route(distances_u, num_points);

    // fprintf(stdout, "res: %lu\n", naive->distance_u);

    return 0;

}
