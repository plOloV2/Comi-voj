#include "libs.h"
#include "algorithms.h"
#include "UI/TUI_func.h"
#include <math.h>

Route* run_choosen_algorithm(int32_t algorithm, alg_in_data* data, uint32_t* distances, size_t num_points){

    Route* result = NULL;

    switch(algorithm){
        case 1:
            result = nearest_neighbour_fast(distances, num_points, 0);
            break;
        
        case 2:
            result = nearest_neighbour_stack(distances, num_points, 0);
            break;
        
        case 3:
            result = repetitive_nearest_neighbour(distances, num_points, nearest_neighbour_fast);
            break;
        
        case 4:
            result = repetitive_nearest_neighbour(distances, num_points, nearest_neighbour_stack);
            break;
        
        case 5:
            result = brute_force(distances, num_points);
            break;
        
        case 6:
            result = rand_seq(distances, num_points, data->perms);
            break;

        case 7:
            result = branch_and_bound(distances, num_points, data->max_time, data->mode);
            break;

        case 8:
            result = tabu_search(distances, num_points, data->max_iter, data->sample_size, data->max_no_up, data->use_RNN, data->min_iter_stop, data->max_iter_stop, data->tabu_limit, data->use_aspiration);
            break;

        case 9:
            result = genetic(distances, num_points, data->max_time, (size_t)(((double)data->generation_size / 100.0) * (double)num_points), data->config, data->mutat_rate, data->cross_rate, data->target, &data->avg_last_gen);
            printf("Average last gen:%f\n", data->avg_last_gen);
            break;
        
    }

    return result;

}

static const char* algorithms_names[6] = {"NN (fast)", "NN (stack)", "RNN (fast)", "RNN (stack)", "BF", "Random"};

Route**** run_whole_calculation(uint32_t*** data_table){

    Route**** results = calloc(6, sizeof(Route***));
    if(!results){
        print_error("Route*** results alloc failed.\n");
        return NULL;
    }

    int32_t calc_fail = 0;

    for(int32_t i = 0; i < 6; i++){

        if(calc_fail)
            break;

        results[i] = calloc(7, sizeof(Route**));
        if(!results[i]){
            print_error("results[i] alloc failed.\n");
            calc_fail = 1;
            break;
        }

        for(int32_t j = 0; j < 7; j++){
            results[i][j] = calloc(100, sizeof(Route*));
            if(!results[i][j]){
                print_error("results[i][j] alloc failed.\n");
                calc_fail = 1;
                break;

            }
            
        }

    }

    if(!calc_fail){

        for(int32_t i = 0; i < 6; i++){

            if(calc_fail)
                break;

            double calc_time_whole;

            fprintf(stdout, "Running calculations for " ANSI_STYLE_ITALIC ANSI_COLOR_GREEN "%s" ANSI_RESET_ALL " algorithm:\n", algorithms_names[i]);

            calc_time_whole = omp_get_wtime();

            for(int32_t j = 0; j < 7; j++){

                if(calc_fail)
                    break;

                int32_t true_size = j + 8;
                int32_t progress = 0;

                fprintf(stdout, "\tCalculating for data size: " ANSI_STYLE_BOLD ANSI_COLOR_YELLOW "%d" ANSI_RESET_ALL "... 0%%", true_size);
                fflush(stdout);

                double calc_time_data = omp_get_wtime();

                #pragma omp parallel for schedule(dynamic)
                for(int32_t k = 0; k < 100; k++){

                    if(calc_fail)
                        continue;

                    switch(i){
                        case 0: results[i][j][k] = nearest_neighbour_fast(data_table[j][k], true_size, 0); break;
                        case 1: results[i][j][k] = nearest_neighbour_stack(data_table[j][k], true_size, 0); break;
                        case 2: results[i][j][k] = repetitive_nearest_neighbour(data_table[j][k], true_size, nearest_neighbour_fast); break;
                        case 3: results[i][j][k] = repetitive_nearest_neighbour(data_table[j][k], true_size, nearest_neighbour_stack); break;
                        case 4: results[i][j][k] = brute_force(data_table[j][k], true_size); break;
                        case 5: results[i][j][k] = rand_seq(data_table[j][k], true_size, 0); break;
                    }

                    if(!results[i][j][k]){
                        #pragma omp critical
                        {
                            print_error("\nAn algorithm failed calculations. Aborting all.\n");
                            calc_fail = 1;
                        }
                    }

                    #pragma omp critical
                    {
                        if(!calc_fail){
                            progress++;
                            fprintf(stdout, "\r\tCalculating for data size: " ANSI_STYLE_BOLD ANSI_COLOR_YELLOW "%d" ANSI_RESET_ALL "... %d%%", true_size, progress);
                            fflush(stdout);
                        }
                    }

                }

                calc_time_data = omp_get_wtime() - calc_time_data;
                fprintf(stdout, "\r\tCalculating for data size: " ANSI_STYLE_BOLD ANSI_COLOR_YELLOW "%d" ANSI_RESET_ALL "... done (took %.3lfs)\n", true_size, calc_time_data);

            }

            calc_time_whole = omp_get_wtime() - calc_time_whole;
            fprintf(stdout, ANSI_STYLE_ITALIC ANSI_COLOR_GREEN "%s" ANSI_RESET_ALL " total time: %.3lfs\n\n", algorithms_names[i], calc_time_whole);

        }

    }

    if(calc_fail){
        for(int32_t i = 0; i < 6; i++){

            if(results[i]){
                for(int32_t j = 0; j < 7; j++){

                    if(results[i][j]){
                        for(int32_t k = 0; k < 100; k++){

                            if(results[i][j][k]){
                                free(results[i][j][k]->city_order); 
                                free(results[i][j][k]);
                            }

                        }

                        free(results[i][j]);

                    }

                }

                free(results[i]);

            }

        }

        free(results);

        return NULL;

    }

    return results;

}



uint32_t* create_random_distances(size_t num_points, xoshiro256_state* xos_state);

void run_bb_experiment(double timeout_seconds){

    FILE* file = fopen("bb_results_comparison.csv", "w");
    if(!file){
        print_error("bb_results_comparison.csv file coundn't be open.\n");
        return;
    }

    fprintf(file, "N,Algorytm,RNN_Init,Avg_Time_s,Std_Dev_s,Timeout_Percent\n");
    
    uint64_t seed;
    create_rand_seed(&seed);
    xoshiro256_state RNG;
    xoshiro_init(&RNG, seed);

    const char* strategy_names[] = {"DFS", "BFS", "Best-FS"};

    for(int32_t n = 7; n <= 25; n++){
        fprintf(stdout, "\n" ANSI_STYLE_BOLD "Starting tests for N = %d" ANSI_RESET_ALL "\n", n);

        uint32_t** test_instances = malloc(100 * sizeof(uint32_t*));
        for(int32_t i = 0; i < 100; i++)
            test_instances[i] = create_random_distances(n, &RNG);
        
        for(int32_t strat = 0; strat < 3; strat++){
            for(int32_t rnn_init = 0; rnn_init < 2; rnn_init++){

                if(strat == 1 && (rnn_init == 0 || n > 17))
                    continue;
                
                uint8_t mode = (strat << 1) | rnn_init;
                double total_time = 0.0;
                int32_t timeouts = 0;
                
                double times[100] = {0}; 

                fprintf(stdout, "\tAlgorithm: " ANSI_COLOR_CYAN "%-7s" ANSI_RESET_ALL " | RNN: " ANSI_COLOR_MAGENTA "%-3s" ANSI_RESET_ALL " | Progress: " ANSI_COLOR_YELLOW "  0%%" ANSI_RESET_ALL, 
                        strategy_names[strat], rnn_init ? "Yes" : "No");
                fflush(stdout);

                for(int32_t i = 0; i < 100; i++){
                    Route* res = branch_and_bound(test_instances[i], n, timeout_seconds, mode);
                    
                    if(res){
                        times[i] = res->time;
                        total_time += res->time;
                        
                        if(res->time >= timeout_seconds - 0.001)
                            timeouts++;
                        
                        free(res->city_order);
                        free(res);

                    }else {
                        times[i] = 0.0;
                    }

                    fprintf(stdout, "\r\tAlgorithm: " ANSI_COLOR_CYAN "%-7s" ANSI_RESET_ALL " | RNN: " ANSI_COLOR_MAGENTA "%-3s" ANSI_RESET_ALL " | Progress: " ANSI_COLOR_YELLOW "%3d%%" ANSI_RESET_ALL, 
                            strategy_names[strat], rnn_init ? "Yes" : "No", i + 1);
                    fflush(stdout);

                }

                double avg_time = total_time / 100.0;
                
                double variance = 0.0;
                for(int32_t i = 0; i < 100; i++)
                    variance += (times[i] - avg_time) * (times[i] - avg_time);

                double std_dev = sqrt(variance / 100.0);
                
                fprintf(stdout, "\r\tAlgorithm: " ANSI_COLOR_CYAN "%-7s" ANSI_RESET_ALL " | RNN: " ANSI_COLOR_MAGENTA "%-3s" ANSI_RESET_ALL " | " ANSI_COLOR_GREEN "Done." ANSI_RESET_ALL " (Avg: " ANSI_COLOR_GREEN "%.4lfs" ANSI_RESET_ALL ", StdDev: " ANSI_COLOR_YELLOW "%.4lfs" ANSI_RESET_ALL ", Timeouts: " ANSI_COLOR_RED "%d%%" ANSI_RESET_ALL ")    \n", 
                        strategy_names[strat], rnn_init ? "Yes" : "No", avg_time, std_dev, timeouts);
                
                fprintf(file, "%d,%s,%d,%.9lf,%.9lf,%d\n", n, strategy_names[strat], rnn_init, avg_time, std_dev, timeouts);
                fflush(file);

            }

        }

        for(int32_t i = 0; i < 100; i++)
            free(test_instances[i]);

        free(test_instances);

    }

    fclose(file);
    fprintf(stdout, "\nCalculations done. Results saved in " ANSI_STYLE_BOLD "bb_results_comparison.csv" ANSI_RESET_ALL ".\n");
    
}

void get_file_path(char* path);
uint32_t* read_data_from_TSPLIB(char* file_path, size_t* num_points, uint8_t silence_mode);

void clear_string(char* input){

    size_t in_size = strlen(input);
    size_t len = in_size;

    while(len > 0 && (input[len-1] == '\n' || input[len-1] == '\r' || input[len-1] == ' '))
        input[--len] = '\0';

    if(len == 0)
        return;

    len = 0;
    while(len < in_size && (input[len] == '\t' || input[len] == ' '))
        len++;

    memmove(input, input + len, in_size - len + 1);

}

void find_base_path(char* file_path, char* base_path){

    size_t in_size = strlen(file_path);
    size_t len = in_size;

    while(len > 0 && file_path[len - 1] != '/')
        len--;

    if(len == 0){
        base_path[0] = '\0';
        return;
    }

    strncpy(base_path, file_path, len);
    base_path[len] = '\0';

}


void run_TB_experiment(){

    char results_file_path[256];
    get_file_path(results_file_path);

    FILE* f = fopen(results_file_path, "r");
    if(!f){
        print_error("Failed to open TSPLIB results file. Is the path correct?\n");
        return;
    }

    FILE* csv_file = fopen("ts.csv", "w");
    if(!csv_file){
        print_error("Failed to create tabu_results.csv file.\n");
        fclose(f);
        return;
    }
    
    fprintf(csv_file, "Instance,N,Optimum,Test_Type,Param_Value,Found_Distance,Rel_Error_%%,Time_s\n");

    char line[64];
    char TSP_file[32];
    char value[32];

    fgets(line, sizeof(line), f);
    clear_string(line);
    if(strcmp(line, "Best solution of ATSP") != 0){
        print_error("Headline missmatch. Is it the right file?\n");
        fclose(csv_file);
        fclose(f);
        return;
    }

    char base_path[256];
    find_base_path(results_file_path, base_path);
    size_t base_path_lenght = strlen(base_path);
    
    uint64_t seed;
    create_rand_seed(&seed);
    xoshiro256_state RNG;
    xoshiro_init(&RNG, seed);

    while(1){

        if(!fgets(line, sizeof(line), f))
            break; 

        if(line[0] == '\n' || line[0] == '\r')
            continue;

        clear_string(line);

        if(sscanf(line, "%31[^: \t\n]", TSP_file) != 1)
            continue;

        size_t TSP_file_lenght = strlen(TSP_file);
        char* val_ptr = line + TSP_file_lenght;
        while(*val_ptr == ':' || *val_ptr == ' ' || *val_ptr == '\t')
            val_ptr++;
        
        strncpy(value, val_ptr, sizeof(value));
        uint32_t best_solution = atoi(value);

        char ATSP_path[256];
        strncpy(ATSP_path, base_path, sizeof(ATSP_path));
        strncpy(ATSP_path + base_path_lenght, TSP_file, sizeof(ATSP_path) - base_path_lenght);
        strncpy(ATSP_path + base_path_lenght + TSP_file_lenght, ".atsp", sizeof(ATSP_path) - base_path_lenght - TSP_file_lenght);

        size_t num_points = 0;
        uint32_t* distances = read_data_from_TSPLIB(ATSP_path, &num_points, 0);
        if(!distances)
            continue;

        // Parametry bazowe wyznaczane relatywnie do rozmiaru instancji
        size_t iters = num_points * num_points * 0.75;
        size_t sample_size = (size_t)(iters * 0.50);
        size_t max_no_up = (size_t)(iters * 0.05);
        size_t min_tabu_base = (size_t)(iters * 0.05);
        size_t max_tabu_base = (size_t)(iters * 0.15);
        size_t tabu_limit_base = (size_t)(iters * 0.012);
        if(tabu_limit_base == 0) 
            tabu_limit_base = 5;

        Route* new_route = NULL;
        double rel_error = 0.0;

        for(int rnn = 1; rnn >= 0; rnn--){

            new_route = tabu_search(distances, num_points, iters, sample_size, max_no_up, rnn, min_tabu_base, max_tabu_base, tabu_limit_base, 1);

            if(new_route){

                rel_error = ((double)new_route->distance_u - (double)best_solution) / (double)best_solution * 100.0;

                fprintf(csv_file, "%s,%zu,%u,%s,%s,%zu,%.4lf,%.4lf\n",
                        TSP_file, num_points, best_solution, 
                        (rnn ? "3.0_Rozmiar_Base" : "3.5_Rozw_Poczatkowe"), 
                        (rnn ? "RNN" : "Random"), 
                        new_route->distance_u, rel_error, new_route->time);

                fflush(csv_file);

                free(new_route->city_order);
                free(new_route);
            }

        }

        size_t test_limits[] = {
            0,
            (size_t)(num_points * 0.1),
            (size_t)(num_points * 0.5),
            (size_t)(num_points * 1.0),
            (size_t)(num_points * 2.0),
            (size_t)(num_points * 5.0)
        };
        const char* limit_labels[] = {"Brak_0", "10%_N", "50%_N", "100%_N", "200%_N", "500%_N"};

        for(int l = 0; l < 6; l++){
            
            size_t current_limit = test_limits[l];
            if(l > 0 && current_limit == 0)
                current_limit = 1;

            new_route = tabu_search(distances, num_points, iters, sample_size, max_no_up, 1, min_tabu_base, max_tabu_base, current_limit, 1);

            if(new_route){

                rel_error = ((double)new_route->distance_u - (double)best_solution) / (double)best_solution * 100.0;

                fprintf(csv_file, "%s,%zu,%u,%s,%s,%zu,%.4lf,%.4lf\n",
                        TSP_file, num_points, best_solution, "4.0_Dlugosc_Listy", limit_labels[l],
                        new_route->distance_u, rel_error, new_route->time);

                fflush(csv_file);

                free(new_route->city_order);
                free(new_route);
            }

        }

        size_t cad_min[] = {
            2,
            (size_t)(num_points * 0.1),
            (size_t)(num_points * 0.3),
            (size_t)(num_points * 0.5),
            (size_t)(num_points * 0.8),
            (size_t)(num_points * 0.1),
            (size_t)(num_points * 0.2),
            (size_t)(num_points * 0.4),
            (size_t)(num_points * 0.6),
            (size_t)(num_points * 0.8)
        };
        size_t cad_max[] = {
            2,
            (size_t)(num_points * 0.1),
            (size_t)(num_points * 0.3),
            (size_t)(num_points * 0.5),
            (size_t)(num_points * 0.8),
            (size_t)(num_points * 0.3),
            (size_t)(num_points * 0.6),
            (size_t)(num_points * 0.8),
            (size_t)(num_points * 1.2),
            (size_t)(num_points * 1.5)
        };
        const char* cad_labels[] = {
            "Stala_BardzoKrotka", 
            "Stala_0.1N",
            "Stala_0.3N",  
            "Stala_0.5N", 
            "Stala_0.8N", 
            "Losowa_0.1N_0.3N", 
            "Losowa_0.2N_0.6N", 
            "Losowa_0.4N_0.8N", 
            "Losowa_0.6N_1.2N", 
            "Losowa_0.8N_1.5N"
        };
        
        for(int c = 0; c < 10; c++){
            size_t c_min = cad_min[c] < 1 ? 1 : cad_min[c];
            size_t c_max = cad_max[c];

            new_route = tabu_search(distances, num_points, iters, sample_size, max_no_up, 1, c_min, c_max, tabu_limit_base, 1);

            if(new_route){

                rel_error = ((double)new_route->distance_u - (double)best_solution) / (double)best_solution * 100.0;
                fprintf(csv_file, "%s,%zu,%u,%s,%s,%zu,%.4lf,%.4lf\n",
                        TSP_file, num_points, best_solution, "4.5_Wielkosc_Kadencji", cad_labels[c],
                        new_route->distance_u, rel_error, new_route->time);

                fflush(csv_file);

                free(new_route->city_order);
                free(new_route);
            }

        }

        new_route = tabu_search(distances, num_points, iters, sample_size, max_no_up, 1, min_tabu_base, max_tabu_base, tabu_limit_base, 0);
        if(new_route){
            rel_error = ((double)new_route->distance_u - (double)best_solution) / (double)best_solution * 100.0;
            fprintf(csv_file, "%s,%zu,%u,%s,%s,%zu,%.4lf,%.4lf\n",
                    TSP_file, num_points, best_solution, "5.0_Kryterium_Aspiracji", "Wylaczone",
                    new_route->distance_u, rel_error, new_route->time);

            fflush(csv_file);

            free(new_route->city_order);
            free(new_route);

        }

        free(distances);
    }

    fclose(csv_file);
    fclose(f);

}


void run_GA_experiment(){
    char results_file_path[256];
    get_file_path(results_file_path);

    FILE* f = fopen(results_file_path, "r");
    if(!f){
        print_error("Failed to open TSPLIB results file.\n");
        return;
    }

    FILE* csv_file = fopen("ga_results.csv", "w");
    if(!csv_file){
        print_error("Failed to create ga_results.csv.\n");
        fclose(f);
        return;
    }
    
    fprintf(csv_file, "Instance,N,Optimum,Target,Test_Type,Param_Value,Found_Distance,Rel_Error_%%,Time_s,Avg_Last_Gen,Avg_Rel_Error_%%\n");

    char line[64], TSP_file[32], value[32];
    fgets(line, sizeof(line), f); // Skip header

    char base_path[256];
    find_base_path(results_file_path, base_path);
    size_t base_path_lenght = strlen(base_path);

    while(fgets(line, sizeof(line), f)){

        if(line[0] == '\n' || line[0] == '\r')
            continue;
        clear_string(line);

        if(sscanf(line, "%31[^: \t\n]", TSP_file) != 1)
            continue;

        size_t TSP_file_lenght = strlen(TSP_file);
        char* val_ptr = line + TSP_file_lenght;

        while(*val_ptr == ':' || *val_ptr == ' ' || *val_ptr == '\t')
            val_ptr++;
        
        strncpy(value, val_ptr, sizeof(value));
        uint32_t best_solution = atoi(value);

        char ATSP_path[256];
        strncpy(ATSP_path, base_path, sizeof(ATSP_path));
        strncpy(ATSP_path + base_path_lenght, TSP_file, sizeof(ATSP_path) - base_path_lenght);
        strncpy(ATSP_path + base_path_lenght + TSP_file_lenght, ".atsp", sizeof(ATSP_path) - base_path_lenght - TSP_file_lenght);

        size_t num_points = 0;
        uint32_t* distances = read_data_from_TSPLIB(ATSP_path, &num_points, 0);

        if(!distances)
            continue;

        // Base GA Parameters
        double max_time = 900.0; // 15 mins max per 3.0 requirement
        size_t base_pop = num_points / 2; 
        double m_rate = 0.2;
        double c_rate = 0.8;
        // Base config: Tournament (1), PMX (0), Swap (1) -> 0b101 = 5
        uint8_t base_config = 0b101;

        Route* res = NULL;
        double rel_error = 0.0;
        double avg_rel_error = 0.0;
        double avg_last_gen = 0.0;

        uint64_t targeted_dist;
        if(num_points <= 25){
            targeted_dist = best_solution;
        }else if(num_points < 75){
            targeted_dist = (uint64_t)((double)best_solution * 1.5);
        }else{
            targeted_dist = (uint64_t)((double)best_solution * 2.0);
        }

        // Base run against Instance Size ---
        res = genetic(distances, num_points, max_time, base_pop, base_config, m_rate, c_rate, targeted_dist, &avg_last_gen);
        if(res){
            rel_error = ((double)res->distance_u - best_solution) / best_solution * 100.0;
            avg_rel_error = (avg_last_gen - best_solution) / best_solution * 100.0;
            fprintf(csv_file, "%s,%zu,%u,%lu,3.0_Zaleznosc_Czasu_Bledu,Base,%llu,%.4lf,%.4lf,%.4lf,%.4lf\n",
                TSP_file, num_points, best_solution, targeted_dist, (unsigned long long)res->distance_u, rel_error, res->time, avg_last_gen, avg_rel_error);
            free(res->city_order); free(res);
        }

        // Population Size Sweep ---
        double pop_sizes[] = {0.25, 0.5, 1.0, 2.0};
        for(int i=0; i<4; i++){
            res = genetic(distances, num_points, max_time, (size_t)(pop_sizes[i] * (double)num_points), base_config, m_rate, c_rate, targeted_dist, &avg_last_gen);
            if(res){
                rel_error = ((double)res->distance_u - best_solution) / best_solution * 100.0;
                avg_rel_error = (avg_last_gen - best_solution) / best_solution * 100.0;
                fprintf(csv_file, "%s,%zu,%u,%lu,3.5_Rozmiar_Populacji,%zu,%llu,%.4lf,%.4lf,%.4lf,%.4lf\n",
                        TSP_file, num_points, best_solution, targeted_dist, (size_t)(pop_sizes[i] * (double)num_points), (unsigned long long)res->distance_u, rel_error, res->time, avg_last_gen, avg_rel_error);
                free(res->city_order); free(res);
            }
        }

        // Mutation Method Sweep (Swap vs Invert) ---
        uint8_t mut_configs[] = {base_config & ~4, base_config | 4}; // 0=Invert, 4=Swap (bit 2)
        const char* mut_names[] = {"Invert", "Swap"};
        for(int i=0; i<2; i++){
            res = genetic(distances, num_points, max_time, base_pop, mut_configs[i], m_rate, c_rate, targeted_dist, &avg_last_gen);
            if(res){
                rel_error = ((double)res->distance_u - best_solution) / best_solution * 100.0;
                avg_rel_error = (avg_last_gen - best_solution) / best_solution * 100.0;
                fprintf(csv_file, "%s,%zu,%u,%lu,4.0_Metoda_Mutacji,%s,%llu,%.4lf,%.4lf,%.4lf,%.4lf\n",
                        TSP_file, num_points, best_solution, targeted_dist, mut_names[i], (unsigned long long)res->distance_u, rel_error, res->time, avg_last_gen, avg_rel_error);
                free(res->city_order); free(res);
            }
        }

        // Crossover Method Sweep (PMX vs OX) ---
        uint8_t cross_configs[] = {base_config & ~2, base_config | 2}; // 0=PMX, 2=OX (bit 1)
        const char* cross_names[] = {"PMX", "OX"};
        for(int i=0; i<2; i++){
            res = genetic(distances, num_points, max_time, base_pop, cross_configs[i], m_rate, c_rate, targeted_dist, &avg_last_gen);
            if(res){
                rel_error = ((double)res->distance_u - best_solution) / best_solution * 100.0;
                avg_rel_error = (avg_last_gen - best_solution) / best_solution * 100.0;
                fprintf(csv_file, "%s,%zu,%u,%lu,4.5_Metoda_Krzyzowania,%s,%llu,%.4lf,%.4lf,%.4lf,%.4lf\n",
                        TSP_file, num_points, best_solution, targeted_dist, cross_names[i], (unsigned long long)res->distance_u, rel_error, res->time, avg_last_gen, avg_rel_error);
                free(res->city_order); free(res);
            }
        }

        // Selection Method Sweep (Roulette vs Tournament) ---
        uint8_t sel_configs[] = {base_config & ~1, base_config | 1}; // 0=Roulette, 1=Tournament (bit 0)
        const char* sel_names[] = {"Roulette", "Tournament"};
        for(int i=0; i<2; i++){
            res = genetic(distances, num_points, max_time, base_pop, sel_configs[i], m_rate, c_rate, targeted_dist, &avg_last_gen);
            if(res){
                rel_error = ((double)res->distance_u - best_solution) / best_solution * 100.0;
                avg_rel_error = (avg_last_gen - best_solution) / best_solution * 100.0;
                fprintf(csv_file, "%s,%zu,%u,%lu,5.0_Metoda_Selekcji,%s,%llu,%.4lf,%.4lf,%.4lf,%.4lf\n",
                        TSP_file, num_points, best_solution, targeted_dist, sel_names[i], (unsigned long long)res->distance_u, rel_error, res->time, avg_last_gen, avg_rel_error);
                free(res->city_order); free(res);
            }
        }

        fflush(csv_file);
        free(distances);
    }

    fclose(csv_file);
    fclose(f);
    fprintf(stdout, "\nGA experiments completed. Results saved to " ANSI_STYLE_BOLD "ga_results.csv" ANSI_RESET_ALL "\n");

}
