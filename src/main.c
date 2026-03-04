#include "libs.h"
#include "data_prepare.h"
#include "data_structs.h"

int main(int argc, char** argv){

    if(argc != 2){
        fprintf(stdout, "Provide me with one argument\n");
        return 1;
    }

    int num_points = 0;

    Point* points = parse_file(argv[1], &num_points);

    uint32_t** distances = calc_dist_table(points, num_points);

    Route* naive = find_naive_route(distances, num_points);

    return 0;

}
