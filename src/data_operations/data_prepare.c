#include "libs.h"
#include "UI/TUI_func.h"
#include "data_operations/route_struct.h"

#define MAX_ROUTE 0x0fff

static inline uint32_t* aloc_mem(size_t size){

    return malloc(size * size * sizeof(uint32_t));
    
}

uint32_t* parse_file(char* file_path, size_t* num_points){

    FILE* f = fopen(file_path, "r");
    if(!f){
        print_error("Failed to open file. Is the path correct?\n");
        return NULL;
    }

    if(fscanf(f, "%zu", num_points) != 1){
        print_error("Failed to read number of points from file.\n");
        fclose(f);
        return NULL;
    }

    if(*num_points == 0){
        *num_points = 0;
        print_error("Number of points read from file is 0.\n");
        fclose(f);
        return NULL;
    }

    uint32_t* distances = aloc_mem(*num_points);
    if(!distances){
        print_error("distances table mem alloc failed.\n");
        *num_points = 0;
        fclose(f);
        return NULL;
    }

    for(size_t i = 0; i < *num_points; i++){
        for(size_t j = 0; j < *num_points; j++){

            if(fscanf(f, "%u", &distances[i * *num_points + j]) != 1){

                print_error("Failed to read distance from file.\n");
                free(distances);
                fclose(f);
                *num_points = 0;
                return NULL;

            }

            distances[i * *num_points + j] = (i == j) ? UINT32_MAX : (distances[i * *num_points + j] % MAX_ROUTE);
            
        }
        
    }

    fclose(f);

    return distances;

}

uint32_t* create_random_distances(size_t num_points, xoshiro256_state* xos_state){

    uint32_t* distances = aloc_mem(num_points);
    if(!distances){
        print_error("distances table mem alloc failed.\n");
        return NULL;
    }

    for(size_t i = 0; i < num_points; i++){
        for(size_t j = 0; j < num_points; j+=2){

            uint64_t rand_val = xoshiro_next(xos_state);

            distances[i * num_points + j] = (i == j) ? UINT32_MAX : (((rand_val & 0xffffffff) % MAX_ROUTE) + 1);

            if((j + 1) < num_points)
                distances[i * num_points + j + 1] = (i == (j + 1)) ? UINT32_MAX : ((((rand_val >> 32) & 0xffffffff) % MAX_ROUTE) + 1);

        }

    }

    return distances;

}
