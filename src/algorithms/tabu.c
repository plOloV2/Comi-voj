#include "libs.h"
#include "UI/TUI_func.h"

// RNN algorithm from previouse stage of project
Route* repetitive_nearest_neighbour(uint32_t* distances, size_t num_points, Route* (*NN_func)(uint32_t* distances, size_t num_points, int start_point_id));
Route* nearest_neighbour_fast(uint32_t* distances, size_t num_points, int current_route_point_id);

typedef union{
    uint64_t b64;
    uint16_t b16[4];
} type_conv;

typedef struct {
    uint16_t town;
    uint16_t pos;
} tabu_move;

// helper function to alloc tabu_matrix of Size = size^2
// returns NULL on fail
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

// main Tabu Search function
Route* tabu_search(uint32_t* distances, size_t num_points, size_t max_iter, size_t sample_size, size_t max_no_up, uint8_t use_RNN, size_t min_iter_stop, size_t max_iter_stop, size_t tabu_limit, uint8_t use_aspiration){

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

    uint64_t xos_seed;
    create_rand_seed(&xos_seed);
    xoshiro256_state xos_state;
    xoshiro_init(&xos_state, xos_seed);

    

    // creates a queue to save tabu moves (use just to limit max number of moves that are tabu)
    tabu_move* tabu_queue = NULL;
    if(tabu_limit > 0) {
        tabu_queue = malloc(tabu_limit * sizeof(tabu_move));
        if(!tabu_queue){
            print_error("Tabu queue alloc failed.");
            free(best_route->city_order);
            free(best_route);
            return NULL;
        }
    }
    
    // creates a matrix to save tabu moves (allows O(1) checking if move is tabu)
    size_t** tabu_matrix = alloc_tabu_matrix(num_points);
    if(!tabu_matrix){
        print_error("Tabu matrix alloc failed.");
        free(best_route->city_order);
        free(best_route);
        free(tabu_queue);
        return NULL;
    }
    size_t tabu_head = 0;
    size_t tabu_count = 0;

    size_t since_last_up = 0;
    size_t current_iter = 1;
    size_t iter_diff = max_iter_stop - min_iter_stop;

    // start of time measurement
    best_route->time = omp_get_wtime();

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
        create_rand_sequence(current_route->city_order, num_points, &xos_state);
        current_route->distance_u = calculate_road_dist(distances, num_points, current_route->city_order);

    }

    best_route->distance_u = current_route->distance_u;
    memcpy(best_route->city_order, current_route->city_order, (num_points * sizeof(uint16_t)));

    
    while(current_iter++ <= max_iter){

        if((current_iter % 16) == 1 && (omp_get_wtime() - best_route->time) >= 900.0){
            print_error("Algorithm exceeded 15minutes. Stoping now.");
            break;
        }

        if(since_last_up++ > max_no_up){    // algorithm stuck

            // creates random route to "unstuck" algorithm
            create_rand_sequence(current_route->city_order, num_points, &xos_state);
            current_route->distance_u = calculate_road_dist(distances, num_points, current_route->city_order);

            // resets tabu matrix
            for(size_t i = 0; i < num_points; i++)
                memset(tabu_matrix[i], 0, num_points * sizeof(size_t));
                
            since_last_up = 0;

            // resets number of moves in tabu queue
            tabu_count = 0;
            tabu_head = 0;

        } else{                             // normal algorithm cycle

            uint64_t best_sample = UINT64_MAX;
            uint16_t best_move_1;
            uint16_t best_move_2;

            uint16_t best_town_1; 
            uint16_t best_town_2;

            type_conv new_pull;

            // creates random moves and chooses best one that's not in tabu list
            for(size_t i = 0; i < sample_size; i++){
                
                if(i % 2 == 0)
                    new_pull.b64 = xoshiro_next(&xos_state);

                // randomly chooses one town to go to random position
                uint16_t old_pos = new_pull.b16[(i % 2) * 2] % num_points;
                uint16_t new_pos = new_pull.b16[(i % 2) * 2 + 1] % num_points;

                if(old_pos == new_pos)
                    continue;

                if(old_pos > new_pos)
                    swap_numbers(&old_pos, &new_pos);
                
                uint16_t town_1 = current_route->city_order[old_pos];
                uint16_t town_2 = current_route->city_order[new_pos];

                uint16_t prev_1 = current_route->city_order[old_pos == 0 ? num_points - 1 : old_pos - 1];
                uint16_t next_1 = current_route->city_order[old_pos == num_points - 1 ? 0 : old_pos + 1];

                uint16_t prev_2 = current_route->city_order[new_pos == 0 ? num_points - 1 : new_pos - 1];
                uint16_t next_2 = current_route->city_order[new_pos == num_points - 1 ? 0 : new_pos + 1];

                int64_t delta = 0;

                // delta of whole route calculations for this move
                if(new_pos == old_pos + 1 || (old_pos == 0 && new_pos == num_points - 1)){
                    // town_1 and town_2 are neighbours
                    
                    uint16_t t_left = (old_pos == 0 && new_pos == num_points - 1) ? town_2 : town_1;
                    uint16_t t_right = (old_pos == 0 && new_pos == num_points - 1) ? town_1 : town_2;
                    uint16_t p_left = (old_pos == 0 && new_pos == num_points - 1) ? prev_2 : prev_1;
                    uint16_t n_right = (old_pos == 0 && new_pos == num_points - 1) ? next_1 : next_2;

                    delta -= distances[p_left * num_points + t_left];
                    delta -= distances[t_left * num_points + t_right];
                    delta -= distances[t_right * num_points + n_right];

                    delta += distances[p_left * num_points + t_right];
                    delta += distances[t_right * num_points + t_left];
                    delta += distances[t_left * num_points + n_right];

                } else{
                    // town_1 and town_2 are not neighbours
                    
                    delta -= distances[prev_1 * num_points + town_1];
                    delta -= distances[town_1 * num_points + next_1];
                    delta -= distances[prev_2 * num_points + town_2];
                    delta -= distances[town_2 * num_points + next_2];

                    delta += distances[prev_1 * num_points + town_2];
                    delta += distances[town_2 * num_points + next_1];
                    delta += distances[prev_2 * num_points + town_1];
                    delta += distances[town_1 * num_points + next_2];

                }

                uint64_t new_dist = current_route->distance_u + delta;
                
                // checking if current move is best in this sample
                if(new_dist < best_sample){
            
                    uint8_t is_tabu = (tabu_matrix[town_1][new_pos] >= current_iter || tabu_matrix[town_2][old_pos] >= current_iter);
                    uint8_t aspiration = (use_aspiration && new_dist < best_route->distance_u);
                    
                    // if move is not in tabu and is better then previuse best we remember it
                    if(!is_tabu || aspiration){

                        best_sample = new_dist;

                        best_move_1 = old_pos;
                        best_move_2 = new_pos;

                        best_town_1 = town_1;
                        best_town_2 = town_2;

                    }

                }

            }

            // if we found valid move we set it as our new base 
            if(best_sample != UINT64_MAX){

                current_route->distance_u = best_sample;
                swap_numbers(&current_route->city_order[best_move_1], &current_route->city_order[best_move_2]);

                // moves to add to tabu
                tabu_move moves_to_add[2] = {
                    {best_town_1, best_move_1},
                    {best_town_2, best_move_2}
                };

                for(int m = 0; m < 2; m++){

                    if(tabu_limit != 0){

                        // if we reach a limit we delete old moves from tabu
                        if(tabu_count == tabu_limit){
                            uint16_t old_town = tabu_queue[tabu_head].town;
                            uint16_t old_pos = tabu_queue[tabu_head].pos;
                            
                            tabu_matrix[old_town][old_pos] = 0; 
                        }else {
                            tabu_count++;
                        }

                        // saving new move in queue
                        tabu_queue[tabu_head].town = moves_to_add[m].town;
                        tabu_queue[tabu_head].pos = moves_to_add[m].pos;
                        

                        // setting new queue head
                        tabu_head = (tabu_head + 1) % tabu_limit;

                    }

                    // saving new move in matrix
                    tabu_matrix[moves_to_add[m].town][moves_to_add[m].pos] = current_iter +
                        ((iter_diff == 0) ? min_iter_stop : (size_t)((xoshiro_next(&xos_state) % iter_diff) + min_iter_stop));

                }

            }

        }

        // if our base is better than best route we found, we update best_route
        if(current_route->distance_u < best_route->distance_u){

            best_route->distance_u = current_route->distance_u;
            memcpy(best_route->city_order, current_route->city_order, (num_points * sizeof(uint16_t)));

            since_last_up = 0;

        }

    }

    // ends time measurement
    best_route->time = omp_get_wtime() - best_route->time;

    // memory cleanup 
    for(size_t i = 0; i < num_points; i++)
        free(tabu_matrix[i]);
    free(tabu_matrix);

    free(tabu_queue);

    free(current_route->city_order);
    free(current_route);

    return best_route;

}
