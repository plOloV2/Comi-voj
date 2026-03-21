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

    uint32_t** distances = NULL;

    if(data_source == 1){

        

    }else{


    }

    // if(float_calc == 1){

        // Distance** distances_u = calc_dist_table(points, num_points, float_calc);

    // }else{

        // Distance** distances_f = calc_dist_table(points, num_points, float_calc);

    // }

    // Route* naive = find_naive_route(distances_u, num_points);

    // fprintf(stdout, "res: %lu\n", naive->distance_u);

    return 0;

}
