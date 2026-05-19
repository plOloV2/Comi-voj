#include "libs.h"
#include "UI/TUI_func.h"

Route* repetitive_nearest_neighbour(uint32_t* distances, size_t num_points, Route* (*NN_func)(uint32_t* distances, size_t num_points, int current_route_point_id));
Route* nearest_neighbour_fast(uint32_t* distances, size_t num_points, int current_route_point_id);

typedef union{
    uint64_t b64;
    uint16_t b16[4];
    uint8_t  b8[8];
} type_conv;

static size_t** alloc_tabu_matrix(size_t size){

    size_t** matrix = malloc(size * sizeof(size_t*));
    if(!matrix){
        print_error("Tabu matrix** alloc failed.\n");
        return NULL;
    }

    for(size_t i = 0; i < size; i++){

        matrix[i] = calloc(size, sizeof(size_t));
        if(!matrix[i]){
            print_error("Tabu matrix* alloc failed.\n");
            for(size_t j = 0; j < i; j++)
                free(matrix[i]);

            free(matrix);
            return NULL;

        }

    }

    return matrix;

}

static inline void swap_numbers(uint16_t* a, uint16_t* b){
    uint16_t temp = *a;
    *a = *b;
    *b = temp;
}

// Fisher–Yates shuffle
static void create_rand_sequence(uint16_t* table, size_t num_points, xoshiro256_state* xos_state){

    size_t byte_idx = 4;
    type_conv new_pull;
    new_pull.b64 = xoshiro_next(xos_state);

    for(size_t i = num_points - 1; i > 0; i--){
        
        if(byte_idx >= 4){
            new_pull.b64 = xoshiro_next(xos_state);
            byte_idx = 0;
        }

        size_t j = new_pull.b16[byte_idx++] % (i + 1);

        swap_numbers(&table[i], &table[j]);

    }
    
}

static uint64_t calculate_road_dist(uint32_t* distances, size_t num_points, uint16_t* list){

    uint64_t result = 0;

    for(size_t i = 1; i < num_points; i++)
        result += distances[list[i - 1] * num_points + list[i]];

    result += distances[list[num_points - 1] * num_points + list[0]];

    return result;

}


Route* tabu_search( uint32_t* distances, size_t num_points, size_t max_iter, xoshiro256_state* xos_state, size_t sample_size,
                    size_t max_no_up, uint8_t use_RNN, size_t min_iter_stop, size_t max_iter_stop){

    if(num_points <= 3){
        print_error("Graph size is less or eqal to 3. You can find the best route by yourself.\n");
        return NULL;
    }

    Route* best_route = malloc(sizeof(Route));
    if(!best_route){
        print_error("Best route alloc failed in TS algorithm.\n");
        return NULL;
    }

    best_route->city_order = malloc(num_points * sizeof(uint16_t));
    if(!best_route->city_order){
        print_error("Best_route->city_order alloc failed in TS algorithm.\n");
        free(best_route);
        return NULL;
    }

    Route* current_route = NULL;
    if(use_RNN){

        current_route = repetitive_nearest_neighbour(distances, num_points, nearest_neighbour_fast);

        if(!current_route){
            print_error("RNN failed in TS algorithm.");
            free(best_route->city_order);
            free(best_route);
            return NULL;
        }

    }else {

        current_route = malloc(sizeof(Route));

        if(!current_route){
            print_error("Route* current_route alloc failed in TS algorithm.");
            free(best_route->city_order);
            free(best_route);
            return NULL;
        }

        current_route->city_order = malloc(num_points * sizeof(uint16_t));
        if(!current_route->city_order){
            print_error("current_route->city_order malloc failed in TS algorithm.");
            free(best_route->city_order);
            free(best_route);
            free(current_route);
            return NULL;
        }


        for(size_t i = 0; i < num_points; i++)
            current_route->city_order[i] = i;
        create_rand_sequence(current_route->city_order, num_points, xos_state);
        current_route->distance_u = calculate_road_dist(distances, num_points, current_route->city_order);

    }
    

    size_t** tabu_matrix = alloc_tabu_matrix(num_points);
    if(!tabu_matrix){
        print_error("Tabu matrix alloc failed.");
        free(best_route->city_order);
        free(best_route);
        free(current_route->city_order);
        free(current_route);
        return NULL;
    }

    best_route->distance_u = current_route->distance_u;
    memcpy(best_route->city_order, current_route->city_order, (num_points * sizeof(uint16_t)));
    size_t since_last_up = 0;
    size_t current_iter = 1;
    best_route->time = omp_get_wtime();
    

    while(current_iter++ <= max_iter){

        if(since_last_up++ > max_no_up){

            create_rand_sequence(current_route->city_order, num_points, xos_state);
            calculate_road_dist(distances, num_points, current_route->city_order);

            for(size_t i = 0; i < num_points; i++)
                memset(tabu_matrix[i], 0, num_points * sizeof(size_t));
                
            since_last_up = 0;

        } else{

            for(size_t i = 0; i < sample_size; i++){

                

            }

        }

        if(current_route->distance_u < best_route->distance_u){

            best_route->distance_u = current_route->distance_u;
            memcpy(best_route->city_order, current_route->city_order, (num_points * sizeof(uint16_t)));

            since_last_up = 0;

        }

    }


    best_route->time = omp_get_wtime() - best_route->time;

    for(size_t i = 0; i < num_points; i++)
        free(tabu_matrix[i]);
    free(tabu_matrix);

    free(current_route->city_order);
    free(current_route);

    return best_route;

}
