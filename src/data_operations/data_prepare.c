#include "libs.h"
#include "data_operations/data_structs.h"

static inline uint32_t* aloc_mem(size_t size){

    return malloc(size * size * sizeof(uint32_t));
    
}

uint32_t* parse_file(char* file_path, size_t* num_points){

    FILE* f = fopen(file_path, "r");
    if(!f)
        return NULL;

    if(fscanf(f, "%zu", num_points) != 1){
        fclose(f);
        return NULL;
    }

    if(*num_points == 0){
        *num_points = 0;
        fclose(f);
        return NULL;
    }

    uint32_t* distances = aloc_mem(*num_points);
    if(!distances){

        *num_points = 0;
        fclose(f);
        return NULL;

    }

    for(size_t i = 0; i < *num_points; i++){
        for(size_t j = 0; j < *num_points; j++){

            if(fscanf(f, "%u", &distances[i * *num_points + j]) != 1){

                free(distances);
                fclose(f);
                *num_points = 0;
                return NULL;

            }

            if(i == j)
                distances[i * *num_points + j] = 0;

        }
        
    }

    fclose(f);

    return distances;

}

uint32_t* create_random_points(size_t num_points, xoshiro256_state* xos_state){

    uint32_t* distances = aloc_mem(num_points);
    if(!distances)
        return NULL;

    for(size_t i = 0; i < num_points; i++){
        for(size_t j = 0; j < num_points; j+=2){

            uint64_t rand_val = xoshiro_next(xos_state);

            distances[i * num_points + j] = (i == j) ? 0 : (rand_val & 0xffffffff);

            if((j + 1) < num_points){
                distances[i * num_points + j + 1] = (i == (j + 1)) ? 0 : ((rand_val >> 32) & 0xffffffff);
            }

        }

    }

    return distances;

}
