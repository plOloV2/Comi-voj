#include "libs.h"
#include "UI/TUI_func.h"
#include "data_operations/route_struct.h"
#include "data_operations/stack_struct.h"

Route* repetitive_nearest_neighbour(uint32_t* distances, size_t num_points, Route* (*NN_func)(uint32_t* distances, size_t num_points, int start_point_id));
Route* nearest_neighbour_fast(uint32_t* distances, size_t num_points, int start_point_id);


static uint32_t* precompute_min_outgoing(uint32_t* distances, size_t num_points){

    uint32_t* min_out = malloc(num_points * sizeof(uint32_t));
    if(!min_out){
        print_error("Failed to allocate min_outgoing array.\n");
        return NULL;
    }

    for(size_t i = 0; i < num_points; i++){

        uint32_t min_val = UINT32_MAX;

        for(size_t j = 0; j < num_points; j++){
            if(i != j && distances[i * num_points + j] != (uint32_t)-1){

                if(distances[i * num_points + j] < min_val)
                    min_val = distances[i * num_points + j];
                
            }

        }

        min_out[i] = min_val;

    }

    return min_out;

}

static uint64_t calculate_lower_bound(uint32_t* min_outgoing, size_t num_points, uint8_t* visited, int current_city){
    uint64_t lower_bound = 0;

    if(min_outgoing[current_city] != UINT32_MAX)
        lower_bound += min_outgoing[current_city];

    for(size_t i = 0; i < num_points; i++)
        if(!visited[i] && i != (size_t)current_city)
            lower_bound += min_outgoing[i];

    return lower_bound;

}

static void bb_dfs(uint32_t* distances, size_t num_points, [[maybe_unused]] double timeout_seconds, Route* best_route, uint32_t* min_out){

    Path_stack* stack = init_stack(num_points); 
    if(!stack){
        print_error("Failed to init stack in B&B DFS.\n");
        return;
    }

    // Inicjalizacja stanu początkowego (zaczynamy od miasta 0)
    stack->stack[0].visited[0] = 1;
    stack->stack[0].city_order[0] = 0;
    stack->stack[0].current_city = 0;
    stack->stack[0].depth = 1;
    stack->stack[0].distance_u = 0;

    while(stack->stack_top >= 0){

        #ifdef NDEBUG
        if(omp_get_wtime() - best_route->time > timeout_seconds){
            print_error("B&B DFS exceeded timeout. Ending execution early.\n");
            break;
        }
        #endif

        Path_state current = stack->stack[stack->stack_top];
        stack->stack[stack->stack_top].city_order = NULL;
        stack->stack[stack->stack_top].visited = NULL;
        stack->stack_top--;

        // Osiągniecie stanu końcowego 
        if(current.depth == num_points){

            uint32_t return_cost = distances[current.current_city * num_points + 0];

            if(return_cost != UINT32_MAX){

                uint64_t total_cost = current.distance_u + return_cost;
                
                if(total_cost < best_route->distance_u){
                    best_route->distance_u = total_cost;
                    memcpy(best_route->city_order, current.city_order, num_points * sizeof(uint8_t));
                }

            }

        }else {

            // Rozgałęzianie (generowanie dzieci)
            for(size_t i = 0; i < num_points; i++){
                if(!current.visited[i] && distances[current.current_city * num_points + i] != UINT32_MAX){
                    
                    uint64_t next_cost = current.distance_u + distances[current.current_city * num_points + i];

                    // Symulujemy odwiedzenie 'i', aby funkcja calculate_lower_bound nie wliczała jego wyjścia dwukrotnie
                    current.visited[i] = 1; 
                    uint64_t lb = calculate_lower_bound(min_out, num_points, current.visited, i);
                    current.visited[i] = 0;

                    // Odcinanie gałęzi
                    if(next_cost + lb < best_route->distance_u){
                        
                        // Bezpieczne rozszerzanie stosu
                        if((size_t)stack->stack_top + 1 >= stack->stack_size){

                            stack->stack_size *= 2;

                            Path_state* temp = realloc(stack->stack, stack->stack_size * sizeof(Path_state));
                            if(!temp){

                                print_error("Stack realloc failed in B&B DFS.\n");
                                free(current.visited);
                                free(current.city_order);
                                break;

                            }

                            stack->stack = temp;

                        }

                        // PUSH na stos
                        stack->stack_top++;
                        stack->stack[stack->stack_top].visited = malloc(num_points * sizeof(uint8_t));
                        stack->stack[stack->stack_top].city_order = malloc(num_points * sizeof(uint8_t));

                        memcpy(stack->stack[stack->stack_top].visited, current.visited, num_points);
                        memcpy(stack->stack[stack->stack_top].city_order, current.city_order, num_points);

                        stack->stack[stack->stack_top].visited[i] = 1;
                        stack->stack[stack->stack_top].city_order[current.depth] = i;
                        stack->stack[stack->stack_top].current_city = i;
                        stack->stack[stack->stack_top].depth = current.depth + 1;
                        stack->stack[stack->stack_top].distance_u = next_cost;

                    }

                }

            }

        }

        // Zwalniamy stan rodzica po wygenerowaniu z niego wszystkich możliwych gałęzi
        free(current.visited);
        free(current.city_order);

    }
    
    free_stack(stack);

}

static void bb_bfs(uint32_t* distances, size_t num_points, [[maybe_unused]] double timeout_seconds, Route* best_route, uint32_t* min_out){

    size_t queue_capacity = num_points * num_points;
    Path_state* queue = malloc(queue_capacity * sizeof(Path_state));
    if(!queue){
        print_error("Failed to init queue in B&B BFS.\n");
        return;
    }

    size_t head = 0; // Skąd pobieramy (POP)
    size_t tail = 0; // Gdzie wstawiamy (PUSH)

    // Inicjalizacja stanu początkowego
    queue[tail].visited = calloc(num_points, sizeof(uint8_t));
    if(!queue[tail].visited){
        print_error("Queue[tail].visited alloc failed in B&B BFS.\n");
        free(queue);
        return;
    }

    queue[tail].city_order = calloc(num_points, sizeof(uint8_t));
    if(!queue[tail].city_order){
        print_error("Queue[tail].city_order alloc failed in B&B BFS.\n");
        free(queue[tail].visited);
        free(queue);
        return;
    }

    queue[tail].visited[0] = 1;
    queue[tail].city_order[0] = 0;
    queue[tail].current_city = 0;
    queue[tail].depth = 1;
    queue[tail].distance_u = 0;
    tail++;

    while(head < tail){

        #ifdef NDEBUG
        if(omp_get_wtime() - best_route->time > timeout_seconds){
            print_error("B&B BFS exceeded timeout. Ending execution early.\n");
            break;
        }
        #endif

        // Pobranie z kolejki
        Path_state current = queue[head];
        queue[head].city_order = NULL;
        queue[head].visited = NULL;
        head++;

        if(current.depth == num_points){

            uint32_t return_cost = distances[current.current_city * num_points + 0];

            if(return_cost != UINT32_MAX){
                uint64_t total_cost = current.distance_u + return_cost;
                
                if(total_cost < best_route->distance_u){
                    best_route->distance_u = total_cost;
                    memcpy(best_route->city_order, current.city_order, num_points * sizeof(uint8_t));
                }

            }

        }else {

            // Rozgałęzianie
            for(size_t i = 0; i < num_points; i++){
                if(!current.visited[i] && distances[current.current_city * num_points + i] != UINT32_MAX){
                    
                    uint64_t next_cost = current.distance_u + distances[current.current_city * num_points + i];

                    current.visited[i] = 1; 
                    uint64_t lb = calculate_lower_bound(min_out, num_points, current.visited, i);
                    current.visited[i] = 0;

                    // Odcinanie
                    if(next_cost + lb < best_route->distance_u){
                        
                        // Pamiecią zarządzanie
                        if(tail >= queue_capacity){
                            
                            // Jeśli wskaźnik head przesunął się poza połowę pojemności,
                            // robimy przesunięcie pamięci w lewo zamiast realloc
                            if(head > queue_capacity / 2){

                                size_t active_elements = tail - head;
                                memmove(queue, &queue[head], active_elements * sizeof(Path_state));
                                memset(&queue[active_elements], 0, (queue_capacity - active_elements) * sizeof(Path_state));
                                
                                head = 0;
                                tail = active_elements;
                            }else {

                                // Prawdziwy brak miejsca, rozszerzamy tablicę
                                queue_capacity *= 2;
                                Path_state* temp = realloc(queue, queue_capacity * sizeof(Path_state));
                                if(!temp){
                                    print_error("Queue realloc failed in B&B BFS.\n");
                                    free(current.visited);
                                    free(current.city_order);
                                    goto CLEANUP_BFS;
                                }

                                queue = temp;
                                
                                // Wyzerowanie nowej pamięci
                                memset(&queue[queue_capacity / 2], 0, (queue_capacity / 2) * sizeof(Path_state));

                            }

                        }

                        // WSTAWIANIE do kolejki
                        queue[tail].visited = malloc(num_points * sizeof(uint8_t));
                        if(!queue[tail].visited){
                            print_error("Queue[tail].visited alloc has failed in B&B BFS.\n");
                            goto CLEANUP_BFS;
                        }
                        queue[tail].city_order = malloc(num_points * sizeof(uint8_t));
                        if(!queue[tail].city_order){
                            print_error("Queue[tail].city_order alloc has failed in B&B BFS.\n");
                            free(queue[tail].visited);
                            goto CLEANUP_BFS;
                        }

                        memcpy(queue[tail].visited, current.visited, num_points);
                        memcpy(queue[tail].city_order, current.city_order, num_points);

                        queue[tail].visited[i] = 1;
                        queue[tail].city_order[current.depth] = i;
                        queue[tail].current_city = i;
                        queue[tail].depth = current.depth + 1;
                        queue[tail].distance_u = next_cost;
                        
                        tail++;
                        
                    }

                }

            }

        }

        // Zwalniamy stan rodzica
        free(current.visited);
        free(current.city_order);
    }

CLEANUP_BFS:
    // Czyszczenie wszystkiego, co zostało w kolejce
    for(size_t i = head; i < tail; i++){
        free(queue[i].visited);
        free(queue[i].city_order);
    }

    free(queue);

}

void bb_best(uint32_t* original_distances, size_t num_points, [[maybe_unused]] double timeout_seconds, Route* best_route);

/*
mode    -> last bit: use RNN for  upper bound or not
        -> xxxxx00x: DFS
        -> xxxxx01x: Breadth-first-search
        -> xxxxx10x: Best-first-search
*/

Route* branch_and_bound(uint32_t* distances, size_t num_points, double timeout_seconds, uint8_t mode){

    if(num_points <= 3){
        print_error("Graph size is less or eqal to 3. You can find the best route by yourself.\n");
        return NULL;
    }

    Route* best_route = malloc(sizeof(Route));
    if(!best_route){
        print_error("Best route alloc failed in B&B algorithm.\n");
        return NULL;
    }

    best_route->city_order = malloc(num_points * sizeof(uint8_t));
    if(!best_route->city_order){
        print_error("Best_route->city_order alloc failed in B&B algorithm.\n");
        free(best_route);
        return NULL;
    }
    
    best_route->time = omp_get_wtime();

    if((mode & 0x01)){

        Route* initial_solution = repetitive_nearest_neighbour(distances, num_points, nearest_neighbour_fast);
        
        if(initial_solution){
            best_route->distance_u = initial_solution->distance_u;
            memcpy(best_route->city_order, initial_solution->city_order, num_points * sizeof(uint8_t));
            
            free(initial_solution->city_order);
            free(initial_solution);
        }else {
            print_error("RRN algorithm has failed to find starting distance. Setting it to UINT64_MAX...\n");
            best_route->distance_u = UINT64_MAX; 
        }

    }else {

        best_route->distance_u = UINT64_MAX;

    }

    uint32_t* min_out = precompute_min_outgoing(distances, num_points);
    if(!min_out){
        print_error("precompute_min_outgoing failed in B&B. Exiting...\n");
        free(best_route->city_order);
        free(best_route);
        return NULL;
    }

    switch(mode >> 1){
        case 0:
            bb_dfs(distances, num_points, timeout_seconds, best_route, min_out);
            break;
        case 1:
            bb_bfs(distances, num_points, timeout_seconds, best_route, min_out);
            break;
        case 2:
            bb_best(distances, num_points, timeout_seconds, best_route);
            break;
        
        default:
            print_error("Incorect mode detected in B&B. Exiting...\n");
            free(best_route->city_order);
            free(best_route);
            free(min_out);
            return NULL;
    }

    best_route->time = omp_get_wtime() - best_route->time;
    free(min_out);
    return best_route;

}
