#include "libs.h"
#include "algorithms.h"
#include "UI/TUI_func.h"

Route* run_choosen_algorithm(int algorithm, int perms, uint32_t* distances, size_t num_points){

    Route* result = NULL;

    switch(algorithm){
        case 1:
            result = nearest_neighbour_fast(distances, num_points, 0);
            break;
        
        case 2:
            result = nearest_neighbour_stack(distances, num_points, 0);
            break;
        
        case 3:
            result = repetitive_nearest_neighbour(distances, num_points, nearest_neighbour_fast);
            break;
        
        case 4:
            result = repetitive_nearest_neighbour(distances, num_points, nearest_neighbour_stack);
            break;
        
        case 5:
            result = brute_force(distances, num_points);
            break;
        
        case 6:
            result = rand_seq(distances, num_points, perms);
            break;
        
    }

    return result;

}
