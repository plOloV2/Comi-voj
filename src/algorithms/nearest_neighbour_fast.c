#include "libs.h"
#include "UI/TUI_func.h"
#include "data_operations/route_struct.h"

Route* nearest_neighbour_fast(uint32_t* distances, size_t num_points, int start_point_id){

    Route* naive = malloc(sizeof(Route));
    if(!naive){
        print_error("Route struct alloc failed in fast NN algorithm, exiting...\n");
        return NULL;
    }

    naive->city_order = malloc(num_points * sizeof(uint16_t));
    if(!naive->city_order){
        print_error("city_order table alloc failed in fast NN algorithm, exiting...\n");
        free(naive);
        return NULL;
    }
    
    uint16_t* visited = calloc(num_points, sizeof(uint16_t));
    if(!visited){
        print_error("visited table alloc failed in fast NN algorithm, exiting...\n");
        free(naive->city_order);
        free(naive);
        return NULL;
    }

    visited[start_point_id] = 1;
    int now_position = start_point_id;
    int target_id = -1;
    naive->distance_u = 0;
    naive->city_order[0] = start_point_id;

    naive->time = omp_get_wtime();

    for(size_t i = 1; i < num_points; i++){

        uint32_t min = UINT32_MAX;
        target_id = -1;

        for(size_t j = 0; j < num_points; j++){

            if(visited[j])
                continue;

            if(distances[now_position * num_points + j] < min){
                min = distances[now_position * num_points + j];
                target_id = j;
            }

        }

        if(target_id == -1){
            print_error("Fast NN algorithm finished too early. Maybe it's a distonnected graph or corrupted data?\n");
            free(visited);
            free(naive);
            return NULL;
        }

        visited[target_id] = 1;
        naive->city_order[i] = now_position = target_id;
        naive->distance_u += min;

    }

    naive->distance_u += distances[target_id * num_points + start_point_id];

    naive->time = omp_get_wtime() - naive->time;
    free(visited);
    return naive;

}
