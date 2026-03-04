#include "libs.h"
#include "data_structs.h"

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
