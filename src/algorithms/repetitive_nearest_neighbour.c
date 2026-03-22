#include "libs.h"
#include "UI/TUI_func.h"
#include "data_operations/data_structs.h"

Route* nearest_neighbour(uint32_t* distances, size_t num_points, int start_point_id);

Route* repetitive_nearest_neighbour(uint32_t* distances, size_t num_points){

    if(num_points <= 2){
        print_error("Graph size is less or eqal to 2. You can find the best by yourself.\n");
        return NULL;
    }

    Route* repitive = malloc(sizeof(Route));
    if(!repitive)
        return NULL;

    repitive->distance_u = UINT64_MAX;

    repitive->time = omp_get_wtime();

    for(size_t i = 0; i < num_points; i++){

        uint64_t now_dist = nearest_neighbour(distances, num_points, i)->distance_u;

        if(now_dist < repitive->distance_u)
            repitive->distance_u = now_dist;

    }

    repitive->time = omp_get_wtime() - repitive->time;
    return repitive;

}
