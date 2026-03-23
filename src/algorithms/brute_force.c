#include "libs.h"
#include "UI/TUI_func.h"
#include "data_operations/route_struct.h"

static inline void swap_numbers(uint8_t* a, uint8_t* b){
    uint8_t temp = *a;
    *a = *b;
    *b = temp;
}

Route* brute_force(uint32_t* distances, size_t num_points){

    if(num_points < 3){
        print_error("Graph size is less or eqal to 2. You can find the best route by yourself.\n");
        return NULL;
    }

    Route* result = malloc(sizeof(Route));
    if(!result){
        print_error("Route struct result alloc failed in BF algorithm, exiting...\n");
        return NULL;
    }
    
    result->city_order = malloc(num_points * sizeof(uint8_t));
    if(!result->city_order){
        print_error("Result->city_order alloc failed in BF algorithm, exiting...\n");
        free(result);
        return NULL;
    }

    uint8_t* temp_permutation = malloc(num_points * sizeof(uint8_t));
    if(!temp_permutation){
        print_error("Temp_permutation alloc failed in BF algorithm, exiting...\n");
        free(result->city_order);
        free(result);
        return NULL;
    }

    for(size_t i = 0; i < num_points; i++)
        temp_permutation[i] = i;

    result->distance_u = UINT64_MAX;
    result->time = omp_get_wtime();
   
    while(1){

        uint64_t temp_dist = 0;
        for(size_t i = 1; i < num_points; i++)
            temp_dist += distances[temp_permutation[i - 1] * num_points + temp_permutation[i]];
        
        temp_dist += distances[temp_permutation[num_points - 1] * num_points + temp_permutation[0]];

        if(temp_dist < result->distance_u){

            result->distance_u = temp_dist;
            memcpy(result->city_order, temp_permutation, num_points * sizeof(uint8_t));

        }

        uint32_t k = UINT32_MAX;
        uint32_t l = UINT32_MAX;

        for(size_t i = num_points - 2; i >= 1; i--){
            if(temp_permutation[i] < temp_permutation[i + 1]){
                k = i;
                break;
            }

        }

        if(k == UINT32_MAX)
            break;

        for(size_t i = num_points - 1; i > k; i--){
            if(temp_permutation[k] < temp_permutation[i]){
                l = i;
                break;
            }

        }

        swap_numbers(&temp_permutation[l], &temp_permutation[k]);

        size_t left = k + 1;
        size_t right = num_points - 1;
        while(left < right){
            swap_numbers(&temp_permutation[left], &temp_permutation[right]);
            left++;
            right--;
        }

    }

    result->time = omp_get_wtime() - result->time;

    free(temp_permutation);

    return result;

}
