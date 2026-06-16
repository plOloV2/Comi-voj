#include "libs.h"
#include "UI/TUI_func.h"

typedef enum config{
    SELECTION = 1 << 0,
    CROSSOVER = 1 << 1,
    MUTATION  = 1 << 2
}config;

typedef union type_conv{
    uint64_t b64;
    uint16_t b16[4];
} type_conv;

typedef struct {
    double total_fit;
    double* cum_fitness;
} RouletteContext;


static void free_route(Route* route);
static void free_gen(Route** gen, size_t gen_size);
static Route** alloc_gen(size_t gen_size, size_t num_points);
static void mut_swap(uint16_t* child, size_t num_points, xoshiro256_state* xos);
static void mut_inver(uint16_t* child, size_t num_points, xoshiro256_state* xos);
static uint64_t calculate_road_dist(uint32_t* distances, size_t num_points, uint16_t* list);
static Route* selec_roulette(Route** gen, size_t gen_size, xoshiro256_state* xos, void* ctx);
static Route* selec_tournament(Route** gen, size_t gen_size, xoshiro256_state* xos, void* ctx);
static void init_first_gen(size_t gen_size, uint32_t* distances, size_t num_points, xoshiro256_state* xos, Route** gen);
static void crossover_OX(const uint16_t* p1, const uint16_t* p2, uint16_t* child, size_t num_points, xoshiro256_state* xos);
static void crossover_PMX(const uint16_t* p1, const uint16_t* p2, uint16_t* child, size_t num_points, xoshiro256_state* xos);

// Main genetic algorithm function
Route* genetic(uint32_t* distances, const size_t num_points, const double max_sec, const size_t generation_size, const uint8_t config, const double mutat_rate, const double cross_rate, const uint64_t target, double* avg_last_gen){

    // startup checks and alloc
    if(num_points <= 3){
        print_error("Graph size is less or eqal to 3. You can find the best route by yourself.\n");
        return NULL;
    }

    Route* best_route = malloc(sizeof(Route));
    if(!best_route){
        print_error("Best route alloc failed in generic algorithm.\n");
        return NULL;
    }

    best_route->city_order = malloc(num_points * sizeof(uint16_t));
    if(!best_route->city_order){
        print_error("Best_route->city_order alloc failed in generic algorithm.\n");
        free(best_route);
        return NULL;
    }

    Route** curr_gen = alloc_gen(generation_size, num_points);
    if(!curr_gen){
        free_route(best_route);
        return NULL;
    }

    Route** next_gen = alloc_gen(generation_size, num_points);
    if(!next_gen){
        free_gen(curr_gen, generation_size);
        free_route(best_route);
        return NULL;
    }

    uint64_t xos_seed;
    create_rand_seed(&xos_seed);
    xoshiro256_state xos_state;
    xoshiro_init(&xos_state, xos_seed);

    RouletteContext* sel_context = NULL;

    // functions selection based on passed config
    Route*   (*selec_func)(Route**, size_t, xoshiro256_state*, void*);
    void     (*cross_func)(const uint16_t*, const uint16_t*, uint16_t*, size_t, xoshiro256_state*);
    void     (*mutat_func)(uint16_t*, size_t, xoshiro256_state*);

    if(config & SELECTION){
        selec_func = &selec_tournament;
    }else{
        selec_func = selec_roulette;

        sel_context = malloc(sizeof(RouletteContext));
        if(!sel_context){
            print_error("sel_context alloc failed.\n");
            free_gen(curr_gen, generation_size);
            free_gen(next_gen, generation_size);
            free_route(best_route);
            return NULL;
        }

        sel_context->cum_fitness = malloc(generation_size * sizeof(double));
        if(!sel_context->cum_fitness){
            print_error("sel_context->cum_fitness alloc failed.\n");
            free_gen(curr_gen, generation_size);
            free_gen(next_gen, generation_size);
            free_route(best_route);
            free(sel_context);
            return NULL;
        }

    }

    if(config & CROSSOVER){
        cross_func = &crossover_OX;
    }else{
        cross_func = &crossover_PMX;
    }

    if(config & MUTATION){
        mutat_func = &mut_swap;
    }else{
        mutat_func = &mut_inver;
    }

    // generates generation 0
    init_first_gen(generation_size, distances, num_points, &xos_state, curr_gen);

    best_route->distance_u = UINT64_MAX;
    best_route->time = omp_get_wtime();

    // algorithm runs until it finds better solution than passed target or till time runs out
    while((omp_get_wtime() - best_route->time) < max_sec && best_route->distance_u > target){

        size_t best_id = 0;
        for(size_t i = 1; i < generation_size; i++)
            if(curr_gen[i]->distance_u < curr_gen[best_id]->distance_u)
                best_id = i;

        if(curr_gen[best_id]->distance_u < best_route->distance_u){
            memcpy(best_route->city_order, curr_gen[best_id]->city_order, num_points * sizeof(uint16_t));
            best_route->distance_u = curr_gen[best_id]->distance_u;
        }

        // precalculation for roulett selection (drops from n^2 to nlogn)
        if(sel_context != NULL){

            sel_context->total_fit = 0.0;
            for(size_t i = 0; i < generation_size; i++){

                double dist = (double)curr_gen[i]->distance_u;
                sel_context->total_fit += 1.0 / dist;
                sel_context->cum_fitness[i] = sel_context->total_fit;

            }

        }

        // best from previouse gen gets transfered to next
        next_gen[0]->distance_u = curr_gen[best_id]->distance_u;
        memcpy(next_gen[0]->city_order, curr_gen[best_id]->city_order, num_points * sizeof(uint16_t));

        // loop to fill next generation with childs from current generation
        for(size_t i = 1; i < generation_size; i++){
        
            Route* parent1 = selec_func(curr_gen, generation_size, &xos_state, (void*)sel_context);
            Route* parent2 = selec_func(curr_gen, generation_size, &xos_state, (void*)sel_context);

            double cross_chance = (double)xoshiro_next(&xos_state) / (double)UINT64_MAX;
            if(cross_chance < cross_rate){
                cross_func(parent1->city_order, parent2->city_order, next_gen[i]->city_order, num_points, &xos_state);
            } else if(xoshiro_next(&xos_state) % 2 == 0){
                memcpy(next_gen[i]->city_order, parent1->city_order, num_points * sizeof(uint16_t));
            } else{
                memcpy(next_gen[i]->city_order, parent2->city_order, num_points * sizeof(uint16_t));
            }

            double mutat_chance = (double)xoshiro_next(&xos_state) / (double)UINT64_MAX;
            if(mutat_chance < mutat_rate){
                mutat_func(next_gen[i]->city_order, num_points, &xos_state);
            }

            next_gen[i]->distance_u = calculate_road_dist(distances, num_points, next_gen[i]->city_order);

        }

        // next gen becomes current gen
        void* temp = next_gen;
        next_gen = curr_gen;
        curr_gen = temp;

    }

    best_route->time = omp_get_wtime() - best_route->time;

    *avg_last_gen = 0.0;

    for(size_t i = 0; i < generation_size; i++)
        *avg_last_gen += (double)curr_gen[i]->distance_u / (double)generation_size;

    // memory cleanup
    free_gen(curr_gen, generation_size);
    free_gen(next_gen, generation_size);

    if(sel_context != NULL){
        free(sel_context->cum_fitness);
        free(sel_context);
    }

    return best_route;

}


// helper function to swap two uint16_t numbers
static inline void swap_numbers(uint16_t* a, uint16_t* b){
    uint16_t temp = *a;
    *a = *b;
    *b = temp;
}

// Fisher–Yates shuffle helper function
static void create_rand_sequence(uint16_t* table, size_t num_points, xoshiro256_state* xos_state){

    size_t byte_idx = 0;
    type_conv new_pull;
    new_pull.b64 = xoshiro_next(xos_state);

    for(size_t i = num_points - 1; i > 0; i--){
        
        if(byte_idx >= 4){
            new_pull.b64 = xoshiro_next(xos_state);
            byte_idx = 0;
        }

        size_t j = new_pull.b16[byte_idx++] % (i + 1);

        swap_numbers(&table[i], &table[j]);

    }
    
}

// helper function to calculate whole route distance
static uint64_t calculate_road_dist(uint32_t* distances, size_t num_points, uint16_t* list){

    uint64_t result = 0;

    for(size_t i = 1; i < num_points; i++)
        result += distances[list[i - 1] * num_points + list[i]];

    result += distances[list[num_points - 1] * num_points + list[0]];

    return result;

}

// helper function to alloc genetation buffer
static Route** alloc_gen(size_t gen_size, size_t num_points){

    Route** generation = calloc(gen_size, sizeof(Route*));
    if(!generation){
        print_error("Route** generation alloc failed in generic algorithm.\n");
        return NULL;
    }


    for(size_t i = 0; i < gen_size; i++){

        generation[i] = malloc(sizeof(Route));
        if(!generation[i]){
            print_error("generation[] alloc failed in generic algorithm.\n");

            for(size_t j = 0; j < i; j++)
                free_route(generation[j]);

            free(generation);
            return NULL;
        }

        generation[i]->city_order = malloc(num_points * sizeof(uint16_t));
        if(!generation[i]->city_order){
            print_error("generation[]->city_order alloc failed in generic algorithm.\n");

            free(generation[i]);
            for(size_t j = 0; j < i; j++)
                free_route(generation[j]);

            free(generation);
            return NULL;
        }

    }

    return generation;

}

// helper function to create random first gen using Fisher–Yates shuffle algorithm
static void init_first_gen(size_t gen_size, uint32_t* distances, size_t num_points, xoshiro256_state* xos, Route** gen){
 
    for(size_t i = 0; i < gen_size; i++){

        for(size_t n = 0; n < num_points; n++)
            gen[i]->city_order[n] = n;

        create_rand_sequence(gen[i]->city_order, num_points, xos);

        gen[i]->distance_u = calculate_road_dist(distances, num_points, gen[i]->city_order);

    }

}

// helper function to free alocated Route struct
static void free_route(Route* route){

    free(route->city_order);
    free(route);

}

// helper function to free alocated generation
static void free_gen(Route** gen, size_t gen_size){

    for(size_t i = 0; i < gen_size; i++)
        free_route(gen[i]);

    free(gen);

}

// Order Crossover 
static void crossover_OX(const uint16_t* p1, const uint16_t* p2, uint16_t* child, size_t num_points, xoshiro256_state* xos){

    // buffer for cities copied from first parent
    uint8_t* visited = calloc(num_points, sizeof(uint8_t));
    if(!visited){
        print_error("visited alloc failed in crossover_OX. Copying parent 1...\n");
        memcpy(child, p1, num_points * sizeof(uint16_t));
        return;
    }

    // random start and end index
    type_conv rand_val;
    rand_val.b64 = xoshiro_next(xos);

    size_t pt1 = rand_val.b16[0] % num_points;
    size_t pt2 = rand_val.b16[1] % num_points;
    
    size_t start = (pt1 < pt2) ? pt1 : pt2;
    size_t end = (pt1 > pt2) ? pt1 : pt2;

    // coping cities from firs parent and marking them
    memcpy(&child[start], &p1[start], (end - start + 1) * sizeof(uint16_t));
    for(size_t i = start; i <= end; i++)
        visited[p1[i]] = 1;

    // filling child up from other parent
    size_t p2_idx    = (end + 1) % num_points;
    size_t child_idx = (end + 1) % num_points;

    for(size_t i = 0; i < num_points; i++){
        uint16_t city = p2[p2_idx];

        if(!visited[city]){
            child[child_idx] = city;
            child_idx = (child_idx + 1) % num_points;
        }

        p2_idx = (p2_idx + 1) % num_points;

    }

    free(visited);

}

// Partially Mapped Crossover
static void crossover_PMX(const uint16_t* p1, const uint16_t* p2, uint16_t* child, size_t num_points, xoshiro256_state* xos){

    // buffer for cities positions in parent 2  
    uint16_t* pos_in_p2 = malloc(num_points * sizeof(uint16_t));
    if(!pos_in_p2){
        print_error("pos_in_p2 alloc failed in crossover_PMX. Copying parent 1...\n");
        memcpy(child, p1, num_points * sizeof(uint16_t));
        return;
    }

    // random start and end index
    type_conv rand_val;
    rand_val.b64 = xoshiro_next(xos);

    size_t pt1 = rand_val.b16[0] % num_points;
    size_t pt2 = rand_val.b16[1] % num_points;
    
    size_t start = (pt1 < pt2) ? pt1 : pt2;
    size_t end = (pt1 > pt2) ? pt1 : pt2;

    // setting child to uninitialized state
    memset(child, 0xff, num_points * sizeof(uint16_t));

    // lookup table for city position in parent 2
    for(size_t i = 0; i < num_points; i++)
        pos_in_p2[p2[i]] = i;

    // copy midle part from parent 1 to child
    memcpy(&child[start], &p1[start], (end - start + 1) * sizeof(uint16_t));

    // placing cities from middle part of parent 2
    for(size_t i = start; i <= end; i++){
        uint16_t city = p2[i];
        
        // is this city already included?
        int is_in_segment = 0;
        for(size_t j = start; j <= end; j++){
            if(p1[j] == city){
                is_in_segment = 1;
                break;
            }
        }

        // looking for place to put this city
        if(!is_in_segment){
            size_t curr_idx = i;
            while(curr_idx >= start && curr_idx <= end){
                uint16_t city_in_p1 = p1[curr_idx];
                curr_idx = pos_in_p2[city_in_p1];
            }

            child[curr_idx] = city;

        }

    }

    // filling remaining empty places with towns from parent 2
    for(size_t i = 0; i < num_points; i++){

        if(child[i] == UINT16_MAX)
            child[i] = p2[i];
        
    }

    free(pos_in_p2);

}

// Mutation by swaping two random cities
static void mut_swap(uint16_t* child, size_t num_points, xoshiro256_state* xos){
    type_conv rand_val;
    rand_val.b64 = xoshiro_next(xos);

    size_t pt1 = rand_val.b16[0] % num_points;
    size_t pt2 = rand_val.b16[1] % num_points;
    
    // ensure we are swapping two distinct indices to actually cause a mutation
    if(pt1 == pt2)
        pt2 = (pt2 + 1) % num_points;

    swap_numbers(&child[pt1], &child[pt2]);

}

// Mutation by inverting city order inside random 
static void mut_inver(uint16_t* child, size_t num_points, xoshiro256_state* xos){
    type_conv rand_val;
    rand_val.b64 = xoshiro_next(xos);

    size_t pt1 = rand_val.b16[0] % num_points;
    size_t pt2 = rand_val.b16[1] % num_points;

    size_t start = (pt1 < pt2) ? pt1 : pt2;
    size_t end = (pt1 > pt2) ? pt1 : pt2;

    // Reverse the sequence between start and end indices
    while(start < end){
        swap_numbers(&child[start], &child[end]);
        start++;
        end--;
    }

}

// Tournament selection (bo4)
static Route* selec_tournament(Route** gen, size_t gen_size, xoshiro256_state* xos,  [[maybe_unused]] void* ctx){
    const size_t k = 4;
    type_conv rand_val;
    rand_val.b64 = xoshiro_next(xos);

    size_t best_idx = rand_val.b16[0] % gen_size;

    // Compare with (k-1) other random competitors
    for(size_t i = 1; i < k; i++){
        size_t rand_idx = rand_val.b16[i] % gen_size;
        
        if(gen[rand_idx]->distance_u < gen[best_idx]->distance_u)
            best_idx = rand_idx;
        
    }

    return gen[best_idx];

}

// Roulette selection
static Route* selec_roulette(Route** gen, size_t gen_size, xoshiro256_state* xos, void* context){

    const RouletteContext* ctx = (const RouletteContext*)context;

    double r = ((double)xoshiro_next(xos) / (double)UINT64_MAX) * ctx->total_fit;

    // Binary search for the first cumulative element >= r
    size_t low = 0, high = gen_size - 1;
    while(low < high){

        size_t mid = low + (high - low) / 2;
        if(ctx->cum_fitness[mid] < r)
            low = mid + 1;
        else
            high = mid;

    }

    return gen[low];

}
