#include "libs.h"
#include "data_prepare.h"
#include "data_structs.h"

#define VERBOUSE

int main(int argc, char** argv){

    if(argc != 2){
        fprintf(stdout, "Provide me with one argument\n");
        return 1;
    }

    int num_points = 0;
    #ifdef VERBOUSE
        fprintf(stdout, "Starting data preparation... ");
        double time = omp_get_wtime();
    #endif

    Point* points = parse_file(argv[1], &num_points);

    uint32_t** distances = calc_dist_table(points, num_points);

    Route* naive = find_naive_route(distances, num_points);

    #ifdef VERBOUSE
        time = omp_get_wtime() - time;
        fprintf(stdout, "done, took %lfms\n", time * 1000);

        for(int i = 0; i < num_points; i++){
            fprintf(stdout, "\nPoint %d - x:%hd, y:%hd\n\tDistances:", i, points[i].x, points[i].y);
            for(int j = 0; j < num_points; j++)
                fprintf(stdout, " %d", distances[i][j]);
        }
    #endif

    return 0;

}
