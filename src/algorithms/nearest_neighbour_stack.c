#include "libs.h"
#include "UI/TUI_func.h"
#include "data_operations/stack_struct.h"
#include "data_operations/route_struct.h"

Route* nearest_neighbour_stack(uint32_t* distances, size_t num_points, int start_point_id){

    Route* naive = malloc(sizeof(Route));
    if(!naive){
        print_error("Route struct alloc failed in stack NN algorithm, exiting...\n");
        return NULL;
    }

    naive->city_order = malloc(num_points * sizeof(uint8_t));
    if(!naive->city_order){
        print_error("Route->city_order alloc failed in stack NN.\n");
        free(naive);
        return NULL;
    }

    naive->distance_u = UINT64_MAX;

    Path_stack* Stack = init_stack(num_points);
    if(!Stack){
        print_error("Stack alloc failed in stack NN algorithm.\n");
        free(naive->city_order);
        free(naive);
        return NULL;
    }

    Stack->stack[0].city_order[0] = (uint8_t)start_point_id;
    Stack->stack[0].visited[start_point_id] = 1;
    Stack->stack[0].current_city = start_point_id;
    Stack->stack[0].depth = 1;
    naive->time = omp_get_wtime();

    while(Stack->stack_top >= 0){

        Path_state current = Stack->stack[Stack->stack_top];

        Stack->stack[Stack->stack_top].city_order = NULL;
        Stack->stack[Stack->stack_top].visited = NULL;

        Stack->stack_top--;

        if(current.depth == num_points){

            uint64_t total_dist = current.distance_u + distances[current.current_city * num_points + start_point_id];

            if(total_dist < naive->distance_u){
                naive->distance_u = total_dist;
                memcpy(naive->city_order, current.city_order, num_points * sizeof(uint8_t));
            }

            free(current.city_order);
            free(current.visited);

            continue;

        }

        uint32_t min = UINT32_MAX;
        for(size_t i = 0; i < num_points; i++){

            if(current.visited[i])
                continue;

            uint32_t dist = distances[current.current_city * num_points + i];

            if(dist < min)
                min = dist;

        }

        if(min == UINT32_MAX){
            print_error("No valid route was found (stack NN). Maybe it's a disconnected graph or the data is corupted?\n");
            free(current.city_order);
            free(current.visited);
            free_stack(Stack);
            free(naive->city_order);
            free(naive);
            return NULL;
        }

        for(size_t i = 0; i < num_points; i++){
            if(current.visited[i] || distances[current.current_city * num_points + i] != min)
                continue;
            
            if(Stack->stack_top + 1 >= Stack->stack_size){

                Stack->stack_size *= 2;
                Path_state* temp = realloc(Stack->stack, Stack->stack_size * sizeof(Path_state));

                if(!temp){
                    print_error("Stack realloc() failed in stack NN.");
                    Stack->stack_size /= 2;
                    free(current.city_order);
                    free(current.visited);
                    free_stack(Stack);
                    free(naive->city_order);
                    free(naive);
                    return NULL;
                }

                Stack->stack = temp;

                memset(&Stack->stack[Stack->stack_size / 2], 0, (Stack->stack_size / 2) * sizeof(Path_state));

            }

            Stack->stack_top++;

            Stack->stack[Stack->stack_top].visited = malloc(num_points * sizeof(uint8_t));
            if(!Stack->stack[Stack->stack_top].visited){
                print_error("Failed to alloc Stack->stack[Stack->stack_top].visited array in stack NN.\n");
                Stack->stack_top--;
                free(current.city_order);
                free(current.visited);
                free_stack(Stack);
                free(naive->city_order);
                free(naive);
                return NULL;
            }

            Stack->stack[Stack->stack_top].city_order = malloc(num_points * sizeof(uint8_t));
            if(!Stack->stack[Stack->stack_top].city_order){
                print_error("Failed to alloc Stack->stack[Stack->stack_top].city_order array in stack NN.\n");
                free(current.city_order);
                free(current.visited);
                free(Stack->stack[Stack->stack_top--].visited);
                free_stack(Stack);
                free(naive->city_order);
                free(naive);
                return NULL;
            }

            memcpy(Stack->stack[Stack->stack_top].visited, current.visited, num_points);
            memcpy(Stack->stack[Stack->stack_top].city_order, current.city_order, num_points);

            Stack->stack[Stack->stack_top].visited[i] = 1;
            Stack->stack[Stack->stack_top].city_order[current.depth] = i;
            Stack->stack[Stack->stack_top].distance_u = current.distance_u + min;
            Stack->stack[Stack->stack_top].current_city = i;
            Stack->stack[Stack->stack_top].depth = current.depth + 1;

        }

        free(current.city_order);
        free(current.visited);

    }

    naive->time = omp_get_wtime() - naive->time;

    free_stack(Stack);
    return naive;

}
