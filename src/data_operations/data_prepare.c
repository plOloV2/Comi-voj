#include "libs.h"
#include "data_operations/data_structs.h"

Point* parse_file(char* file_path, size_t* num_points){

    FILE* f = fopen(file_path, "r");
    if(!f)
        return NULL;

    if(fscanf(f, "%zu", num_points) != 1){
        fclose(f);
        return NULL;
    }

    if(*num_points <= 0){
        *num_points = 0;
        fclose(f);
        return NULL;
    }

    Point* points = malloc(*num_points * sizeof(Point));
    if(!points){
        fclose(f);
        return NULL;
    }

    for(size_t i = 0; i < *num_points; i++){
        if(fscanf(f, "%hd %hd", &points[i].x, &points[i].y) != 2){
            free(points);
            fclose(f);
            *num_points = 0;
            return NULL;
        }
    }

    fclose(f);

    return points;

}

Point* create_random_points(size_t num_points, xoshiro256_state* xos_state){

    Point* points = malloc(num_points * sizeof(Point));
    if(!points)
        return NULL;

    int num_runs = num_points / 2;

    for(int i = 0; i < num_runs; i++){

        uint64_t rand_val = xoshiro_next(xos_state);

        points[(i * 2) + 0].x = (int16_t)(rand_val & 0xffff);
        points[(i * 2) + 0].y = (int16_t)((rand_val >> 16) & 0xffff);
        
        points[(i * 2) + 1].x = (int16_t)((rand_val >> 32) & 0xffff);
        points[(i * 2) + 1].y = (int16_t)((rand_val >> 48) & 0xffff);

    }

    if(num_points % 2 == 1){
        
        uint64_t rand_val = xoshiro_next(xos_state);

        points[num_points - 1].x = (int16_t)(rand_val & 0xffff);
        points[num_points - 1].y = (int16_t)((rand_val >> 16) & 0xffff);

    }

    return points;

}



