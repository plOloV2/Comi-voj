#include "libs.h"
#include "UI/TUI_func.h"

Route* repetitive_nearest_neighbour(uint32_t* distances, size_t num_points, Route* (*NN_func)(uint32_t* distances, size_t num_points, int start_point_id));
Route* nearest_neighbour_fast(uint32_t* distances, size_t num_points, int start_point_id);

typedef union{
    uint64_t b64;
    uint16_t b16[4];
    uint8_t  b8[8];
} type_conv;

typedef struct tabu tabu;

struct tabu{
    size_t      cycles_blocked;
    uint16_t*   order;
    tabu*       next;
};

static void add_tabu(tabu* list, uint16_t* state, xoshiro256_state* xos){



}

static void decrease_cycles_tabu(tabu* list){

    tabu* next = list;

    while(next){

        if(next->cycles_blocked-- == 1){

            free(next->order);
            list->next = next->next;

        }

        list = next;
        next = list->next;
        
    }

}

static void free_tabu(tabu* list){

    if(!list)
        return;

    

}

static inline void swap_numbers(uint16_t* a, uint16_t* b){
    uint16_t temp = *a;
    *a = *b;
    *b = temp;
}

// Fisher–Yates shuffle
static void create_rand_sequence(uint16_t* table, size_t num_points, xoshiro256_state* xos_state){

    type_conv new_pull;
    new_pull.b64 = xoshiro_next(xos_state);

    for(size_t i = 0; i < num_points; i++)
        table[i] = i;

    size_t byte_idx = 4;

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


Route* tabu_search(uint32_t* distances, size_t num_points, double max_time, xoshiro256_state* xos_state, size_t max_no_up, uint8_t use_RNN){

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

    Route* start = NULL;
    if(use_RNN){

        start = repetitive_nearest_neighbour(distances, num_points, nearest_neighbour_fast);

        if(!start){
            print_error("RNN failed in TS algorithm.");
            free(best_route->city_order);
            free(best_route);
            return NULL;
        }

    }else {

        start = malloc(sizeof(Route));

        if(!start){
            print_error("Route* start alloc failed in TS algorithm.");
            free(best_route->city_order);
            free(best_route);
            return NULL;
        }

        start->city_order = malloc(num_points * sizeof(uint16_t));
        if(!start->city_order){
            print_error("start->city_order malloc failed in TS algorithm.");
            free(best_route->city_order);
            free(best_route);
            free(start);
            return NULL;
        }

        create_rand_sequence(start->city_order, num_points, xos_state);
        start->distance_u = calculate_road_dist(distances, num_points, start->city_order);

    }
    

    best_route->distance_u = start->distance_u;
    memcpy(best_route->city_order, start->city_order, (num_points * sizeof(uint16_t)));
    size_t since_last_up = 0;
    tabu* tabu_list = NULL;
    best_route->time = omp_get_wtime();
    

    while(1){

        if((omp_get_wtime() - best_route->time) >= max_time)
            break;

        if(since_last_up++ > max_no_up){

        } else{

        }

    }


    best_route->time = omp_get_wtime() - best_route->time;
    return best_route;

}
