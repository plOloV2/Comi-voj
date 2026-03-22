#include "libs.h"
#include "UI/TUI_func.h"
#include "data_operations/stack_struct.h"
#include "data_operations/route_struct.h"

Path_stack* init_stack(size_t num_points){

    Path_stack* Stack = malloc(sizeof(Path_stack));
    if(!Stack){
        print_error("Failed to alloc Path_stack struct.\n");
        return NULL;
    }

    Stack->stack_size = num_points * 10;
    Stack->stack_top = 0;

    Stack->stack = calloc(Stack->stack_size, sizeof(Path_state));
    if(!Stack->stack){
        print_error("Failed to alloc Stack->stack array.\n");
        free(Stack);
        return NULL;
    }

    for(size_t i = 0; i < Stack->stack_size; i++){

        Stack->stack[i].city_order = malloc(num_points * sizeof(uint8_t));
        if(!Stack->stack[i].city_order){
            print_error("Failed to alloc Stack->stack[i].city_order array.\n");
            for(size_t j = 0; j < i; j++){
                free(Stack->stack[j].city_order);
                free(Stack->stack[j].visited);
            }
            free(Stack->stack);
            free(Stack);
            return NULL;
        }

        Stack->stack[i].visited = calloc(num_points, sizeof(uint8_t));
        if(!Stack->stack[i].visited){
            print_error("Failed to alloc Stack->stack[i].visited array.\n");
            free(Stack->stack[i].city_order);
            for(size_t j = 0; j < i; j++){
                free(Stack->stack[j].city_order);
                free(Stack->stack[j].visited);
            }
            free(Stack->stack);
            free(Stack);
            return NULL;
        }

    }

    return Stack;
    
}

void free_stack(Path_stack* Stack){

    for(size_t i = 0; i < Stack->stack_size; i++){

        free(Stack->stack[i].city_order);
        free(Stack->stack[i].visited);

    }

    free(Stack->stack);
    free(Stack);

}
