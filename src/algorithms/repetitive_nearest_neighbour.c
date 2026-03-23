#include "libs.h"
#include "UI/TUI_func.h"
#include "data_operations/route_struct.h"

Route* repetitive_nearest_neighbour(uint32_t* distances, size_t num_points, Route* (*NN_func)(uint32_t* distances, size_t num_points, int start_point_id)){

    if(num_points <= 2){
        print_error("Graph size is less or eqal to 2. You can find the best route by yourself.\n");
        return NULL;
    }

    Route* repitive = NULL;

    double time = omp_get_wtime();

    for(size_t i = 0; i < num_points; i++){

        Route* temp = NN_func(distances, num_points, i);

        if(!temp){
            print_error("NN algorithm returned NULL.\n");
            free(repitive->city_order);
            free(repitive);
            return NULL;
        }

        if(!repitive || temp->distance_u < repitive->distance_u){
            repitive = temp;
        }else {
            free(temp->city_order);
            free(temp);
        }

    }

    time = omp_get_wtime() - time;
    repitive->time = time;
    return repitive;

}
