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

    Path_stack* Stack = init_stack(num_points);
    if(!Stack){
        print_error("Stack alloc failed in stack NN algorithm.\n");
        return NULL;
    }

    Stack->stack[0].city_order[0] = (uint8_t)start_point_id;
    Stack->stack[0].visited[start_point_id] = 1;
    Stack->stack[0].current_city = start_point_id;
    naive->time = omp_get_wtime();

    while(Stack->stack_top >= 0){

        Stack->stack[Stack->stack_top];

    }

}
