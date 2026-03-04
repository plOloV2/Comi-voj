#include "libs.h"
#include "data_structs.h"

Point* parse_file(char* file_path, int* num_points){

    FILE* f = fopen(file_path, "r");
    if(!f)
        return NULL;

    if(fscanf(f, "%d", num_points) != 1){
        fclose(f);
        return NULL;
    }

    if(*num_points <= 0){
        *num_points = 0;
        fclose(f);
        return NULL;
    }

    Point* points = calloc(*num_points, sizeof(Point));
    if(!points){
        fclose(f);
        return NULL;
    }

    for(int i = 0; i < *num_points; i++){
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

Route* find_naive_route(uint32_t** distances, int num_points){

    Route naive;

    for(int i = 0; i < num_points; i++){

        uint32_t min = UINT32_MAX;

    }

    return &naive;

}
