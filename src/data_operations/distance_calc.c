#include "libs.h"
#include "data_operations/data_structs.h"

uint32_t distance(Point A, Point B){

    uint32_t dx = (uint32_t)(A.x - B.x);
    uint32_t dy = (uint32_t)(A.y - B.y);

    return dx*dx + dy*dy;

}

Distance** calc_dist_table(Point* points, size_t num_points, int bool_float_distance){

    Distance** result = malloc(num_points * sizeof(Distance*));
    if(!result)
        return NULL;

    for(size_t i = 0; i < num_points; i++){

        result[i] = calloc(num_points, sizeof(Distance));
        if(!result[i]){
            for(size_t j = 0; j < i; j++)
                free(result[j]);

            free(result);
            return NULL;

        }

        if(bool_float_distance){

            for(size_t j = 0; j < num_points; j++)
                result[i][j].dist_f = sqrtf(distance(points[i], points[j]));

        }else{

            for(size_t j = 0; j < num_points; j++)
                result[i][j].dist_u = distance(points[i], points[j]);
        
        }


    }

    return result;

}
