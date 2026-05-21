#ifndef LIBS_H
#define LIBS_H

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <math.h>
#include <omp.h>
#include <string.h>
#include "xoshiro256/Xoshiro256.h"


typedef struct{

    double max_time;
    int perms;
    uint8_t mode;

    uint8_t use_RNN;
    size_t max_iter;
    size_t sample_size;
    size_t max_no_up;
    size_t min_iter_stop;
    size_t max_iter_stop;
    size_t tabu_limit;

} alg_in_data;

#endif
