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

uint32_t*** create_random_distances_for_calc(){

    uint32_t*** dist_table = calloc(7, sizeof(uint32_t**));
    if(!dist_table){
        print_error("uint32_t*** dist_table alloc failed.\n");
        return NULL;
    }

    int alloc_fail = 0;
    int lr = omp_get_max_threads();

    uint64_t seeds[lr];
    xoshiro256_state xos_states[lr];

    for(int k = 0; k < lr; k++){
        create_rand_seed(&seeds[k]);
        xoshiro_init(&xos_states[k], seeds[k]);
    }

    for(int i = 0; i < 7; i++){

        if(alloc_fail)
            break;

        dist_table[i] = calloc(100, sizeof(uint32_t*));
        if(!dist_table[i]){
            print_error("dist_table[i] alloc failed.\n");
            alloc_fail = 1;
            break;
        }

        #pragma omp parallel for num_threads(lr)
        for(int j = 0; j < 100; j++){

            dist_table[i][j] = create_random_distances(i + 8, &xos_states[omp_get_thread_num()]);
            if(!dist_table[i][j]){
                #pragma omp critical
                {
                    print_error("creation of dist_table[i][j] failed.\n");
                    alloc_fail = 1;
                }

            }

        }

    }

    if(alloc_fail){

        for(int i = 0; i < 7; i++){

            if(dist_table[i])
                for(int j = 0; j < 100; j++)
                    free(dist_table[i][j]);

            free(dist_table[i]);
                
        }

        free(dist_table);
        return NULL;

    }

    return dist_table;

}
