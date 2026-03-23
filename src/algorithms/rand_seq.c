#include "libs.h"
#include "UI/TUI_func.h"
#include "data_operations/route_struct.h"

typedef union{
    uint64_t    a;
    uint16_t    d[4];
} rand_convert;

static inline void swap_numbers(uint8_t* a, uint8_t* b){
    uint8_t temp = *a;
    *a = *b;
    *b = temp;
}

// Fisher–Yates shuffle
static void create_rand_sequence(uint8_t* table, size_t num_points, xoshiro256_state* xos_state){

    rand_convert new_pull;
    new_pull.a = xoshiro_next(xos_state);

    for(size_t i = 0; i < num_points; i++)
        table[i] = i;

    size_t byte_idx = 4;

    for(size_t i = num_points - 1; i > 0; i--){
        
        if(byte_idx >= 4){
            new_pull.a = xoshiro_next(xos_state);
            byte_idx = 0;
        }

        size_t j = new_pull.d[byte_idx++] % (i + 1);

        swap_numbers(&table[i], &table[j]);

    }
    
}

Route* rand_seq(uint32_t* distances, size_t num_points, int perms){

    if(perms == 0)
        perms = 10 * num_points;

    if(num_points < 3){
        print_error("Graph size is less or eqal to 2. You can find the best route by yourself.\n");
        return NULL;
    }

    Route* result = malloc(sizeof(Route));
    if(!result){
        print_error("Route struct result alloc failed in RNG algorithm, exiting...\n");
        return NULL;
    }
    
    result->city_order = malloc(num_points * sizeof(uint8_t));
    if(!result->city_order){
        print_error("Result->city_order alloc failed, exiting...\n");
        free(result);
        return NULL;
    }

    Route* rand_route = malloc(sizeof(Route));
    if(!rand_route){
        print_error("Route struct rand_route alloc failed in RNG algorithm, exiting...\n");
        free(result->city_order);
        free(result);
        return NULL;
    }

    rand_route->city_order = malloc(num_points * sizeof(uint8_t));
    if(!rand_route->city_order){
        print_error("rand_route->city_order alloc failed, exiting...\n");
        free(result->city_order);
        free(result);
        free(rand_route);
        return NULL;
    }

    uint64_t xos_seed;
    create_rand_seed(&xos_seed);
    xoshiro256_state xos_state;
    xoshiro_init(&xos_state, xos_seed);

    result->distance_u = UINT64_MAX;
    result->time = omp_get_wtime();

    for(int i = 0; i < perms; i++){
        rand_route->distance_u = 0;
        create_rand_sequence(rand_route->city_order, num_points, &xos_state);

        for(size_t j = 1; j < num_points; j++)
            rand_route->distance_u += distances[rand_route->city_order[j - 1] * num_points + rand_route->city_order[j]];
        
        rand_route->distance_u += distances[rand_route->city_order[num_points - 1] * num_points + rand_route->city_order[0]];

        if(rand_route->distance_u < result->distance_u){

            result->distance_u = rand_route->distance_u;
            memcpy(result->city_order, rand_route->city_order, num_points * sizeof(uint8_t));

        }

    }

    result->time = omp_get_wtime() - result->time;

    free(rand_route->city_order);
    free(rand_route);    

    return result;

}
