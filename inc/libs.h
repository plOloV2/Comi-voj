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
    uint8_t use_aspiration;
    size_t max_iter;
    size_t sample_size;
    size_t max_no_up;
    size_t min_iter_stop;
    size_t max_iter_stop;
    size_t tabu_limit;

    size_t generation_size;
    uint8_t config;
    double mutat_rate;
    double cross_rate;
    uint64_t target;
    double avg_last_gen;

} alg_in_data;

#endif
