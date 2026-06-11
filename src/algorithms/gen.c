#include "libs.h"
#include "UI/TUI_func.h"


typedef union{
    uint64_t b64;
    uint16_t b16[4];
} type_conv;

// helper function to swap two uint16_t numbers
static inline void swap_numbers(uint16_t* a, uint16_t* b){
    uint16_t temp = *a;
    *a = *b;
    *b = temp;
}

// Fisher–Yates shuffle helper function
static void create_rand_sequence(uint16_t* table, size_t num_points, xoshiro256_state* xos_state){

    size_t byte_idx = 0;
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

// helper function to calculate whole route distance
static uint64_t calculate_road_dist(uint32_t* distances, size_t num_points, uint16_t* list){

    uint64_t result = 0;

    for(size_t i = 1; i < num_points; i++)
        result += distances[list[i - 1] * num_points + list[i]];

    result += distances[list[num_points - 1] * num_points + list[0]];

    return result;

}

// Main genetic algorithm function
Route* genetic(uint32_t* distances, size_t num_points, double max_sec, size_t generation_size){

    if(num_points <= 3){
        print_error("Graph size is less or eqal to 3. You can find the best route by yourself.\n");
        return NULL;
    }

    Route* best_route = malloc(sizeof(Route));
    if(!best_route){
        print_error("Best route alloc failed in generic algorithm.\n");
        return NULL;
    }

    best_route->city_order = malloc(num_points * sizeof(uint16_t));
    if(!best_route->city_order){
        print_error("Best_route->city_order alloc failed in generic algorithm.\n");
        free(best_route);
        return NULL;
    }

    Route** generation = calloc(generation_size, sizeof(Route*));
    if(!generation){
        print_error("Route** generation alloc failed in generic algorithm.\n");
        free(best_route->city_order);
        free(best_route);
        return NULL;
    }


    uint64_t xos_seed;
    create_rand_seed(&xos_seed);
    xoshiro256_state xos_state;
    xoshiro_init(&xos_state, xos_seed);

    best_route->distance_u = 0;


    for(size_t i = 0; i < generation_size; i++){

        generation[i] = malloc(sizeof(Route));
        if(!generation[i]){
            print_error("generation[] alloc failed in generic algorithm.\n");

            for(size_t j = 0; j < i; j++){
                free(generation[j]->city_order);
                free(generation[j]);
            }

            free(best_route->city_order);
            free(best_route);
            return NULL;
        }

        generation[i]->city_order = malloc(num_points * sizeof(uint16_t));
        if(!generation[i]->city_order){
            print_error("generation[]->city_order alloc failed in generic algorithm.\n");

            free(generation[i]);
            for(size_t j = 0; j < i; j++){
                free(generation[j]->city_order);
                free(generation[j]);
            }

            free(best_route->city_order);
            free(best_route);
            return NULL;
        }

        for(size_t n = 0; n < num_points; n++)
            generation[i]->city_order[n] = n;

        create_rand_sequence(generation[i]->city_order, num_points, &xos_state);

        generation[i]->distance_u = calculate_road_dist(distances, num_points, generation[i]->city_order);

    }

    best_route->time = omp_get_wtime();

    while((omp_get_wtime() - best_route->time) < max_sec){

        

    }

    best_route->time = omp_get_wtime() - best_route->time;

    for(size_t i = 0; i < generation_size; i++){
        free(generation[i]->city_order);
        free(generation[i]);
    }

    free(generation);

    return best_route;

}
