#include "libs.h"
#include "UI/TUI_func.h"
#include "data_operations/route_struct.h"

typedef union{
    uint64_t    a;
    uint32_t    b[2];
    uint16_t    c[4];
    u_int8_t    d[8];
} rand_convert;

Route* rand_seq(uint32_t* distances, size_t num_points, int perms){

    if(perms == 0)
        perms = 10 * num_points;
    
    if(num_points < 7){
        
        if(num_points == 6 && perms > 720)
            perms = 720;
        
        if(num_points == 5 && perms > 120)
            perms = 120;
        
        if(num_points == 4 && perms > 24)
            perms = 24;
        
        if(num_points == 3 && perms > 6)
            perms = 6;

        if(num_points < 3){
            print_error("Graph size is less or eqal to 2. You can find the best route by yourself.\n");
            return NULL;
        }

    }

    Route* result = malloc(sizeof(Route));
    if(!result){
        print_error("Route struct alloc failed in RNG algorithm, exiting...\n");
        return NULL;
    }
    
    result->city_order = malloc(num_points * sizeof(uint8_t));
    if(!result->city_order){
        print_error("Result->city_order alloc failed, exiting...\n");
        free(result);
        return NULL;
    }

    Route* var_array = malloc(perms * sizeof(Route));
    if(!var_array){
        print_error("Route* array alloc failed in RNG algorithm, exiting...\n");
        free(result->city_order);
        free(result);
        return NULL;
    }

    for(int i = 0; i < perms; i++){

        var_array[i].city_order = malloc("var_array[i].city_order malloc failed, exiting...\n");
        

    }

    result->distance_u = UINT64_MAX;
    result->time = omp_get_wtime();

    for(int i = 1; i < perms; i++){

    }

    return result;

}
