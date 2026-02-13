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

uint32_t distance(Point A, Point B){

    uint32_t dx = (uint32_t)(A.x - B.x);
    uint32_t dy = (uint32_t)(A.y - B.y);

    return dx*dx + dy*dy;

}

uint32_t** calc_dist_table(Point* points, int num_points){

    uint32_t** result = calloc(num_points, sizeof(uint32_t*));
    if(!result)
        return NULL;

    for(int i = 0; i < num_points; i++){

        result[i] = calloc(num_points, sizeof(uint32_t));
        if(!result[i]){
            for(int j = 0; j < i; j++)
                free(result[j]);

            free(result);
            return NULL;

        }

        for(int j = 0; j < num_points; j++)
            result[i][j] = distance(points[i], points[j]);

    }

    return result;

}

Route* find_naive_route(uint32_t** distances, int num_points){

    Route naive;

    for(int i = 0; i < num_points; i++){

        uint32_t min = UINT32_MAX;

    }

    return &naive;

}
