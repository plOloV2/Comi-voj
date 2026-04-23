#include "libs.h"
#include "algorithms.h"
#include "UI/TUI_func.h"
#include <math.h>

Route* run_choosen_algorithm(int algorithm, int perms_or_mode, double timeout, uint32_t* distances, size_t num_points){

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
            result = rand_seq(distances, num_points, perms_or_mode);
            break;

        case 7:
            result = branch_and_bound(distances, num_points, timeout, (uint8_t)perms_or_mode);
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

    int calc_fail = 0;

    for(int i = 0; i < 6; i++){

        if(calc_fail)
            break;

        results[i] = calloc(7, sizeof(Route**));
        if(!results[i]){
            print_error("results[i] alloc failed.\n");
            calc_fail = 1;
            break;
        }

        for(int j = 0; j < 7; j++){
            results[i][j] = calloc(100, sizeof(Route*));
            if(!results[i][j]){
                print_error("results[i][j] alloc failed.\n");
                calc_fail = 1;
                break;

            }
            
        }

    }

    if(!calc_fail){

        for(int i = 0; i < 6; i++){

            if(calc_fail)
                break;

            double calc_time_whole;

            fprintf(stdout, "Running calculations for " ANSI_STYLE_ITALIC ANSI_COLOR_GREEN "%s" ANSI_RESET_ALL " algorithm:\n", algorithms_names[i]);

            calc_time_whole = omp_get_wtime();

            for(int j = 0; j < 7; j++){

                if(calc_fail)
                    break;

                int true_size = j + 8;
                int progress = 0;

                fprintf(stdout, "\tCalculating for data size: " ANSI_STYLE_BOLD ANSI_COLOR_YELLOW "%d" ANSI_RESET_ALL "... 0%%", true_size);
                fflush(stdout);

                double calc_time_data = omp_get_wtime();

                #pragma omp parallel for schedule(dynamic)
                for(int k = 0; k < 100; k++){

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
        for(int i = 0; i < 6; i++){

            if(results[i]){
                for(int j = 0; j < 7; j++){

                    if(results[i][j]){
                        for(int k = 0; k < 100; k++){

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

void run_bb_experiment(double timeout_seconds) {
    FILE* file = fopen("bb_results_comparison.csv", "w");
    if(!file){
        print_error("Nie mozna otworzyc bb_results_comparison.csv do zapisu.\n");
        return;
    }

    fprintf(file, "N,Algorytm,RNN_Init,Avg_Time_s,Std_Dev_s,Timeout_Percent\n");
    
    // Inicjalizacja generatora RNG
    uint64_t seed;
    create_rand_seed(&seed);
    xoshiro256_state RNG;
    xoshiro_init(&RNG, seed);

    const char* strategy_names[] = {"DFS", "BFS", "Best-FS"};

    for(int n = 7; n <= 25; n++){
        fprintf(stdout, "\n" ANSI_STYLE_BOLD "Rozpoczynanie testow dla N = %d" ANSI_RESET_ALL "\n", n);

        uint32_t** test_instances = malloc(100 * sizeof(uint32_t*));
        for(int i = 0; i < 100; i++)
            test_instances[i] = create_random_distances(n, &RNG);
        
        for(int strat = 0; strat < 3; strat++){
            for(int rnn_init = 0; rnn_init < 2; rnn_init++){

                if(strat == 1 && (rnn_init == 0 || n > 17))
                    continue;
                
                uint8_t mode = (strat << 1) | rnn_init;
                double total_time = 0.0;
                int timeouts = 0;
                
                // TABLICA NA CZASY DO ODCHYLENIA STANDARDOWEGO
                double times[100] = {0}; 

                fprintf(stdout, "\tAlgorytm: " ANSI_COLOR_CYAN "%-7s" ANSI_RESET_ALL " | RNN: " ANSI_COLOR_MAGENTA "%-3s" ANSI_RESET_ALL " | Postep: " ANSI_COLOR_YELLOW "  0%%" ANSI_RESET_ALL, 
                        strategy_names[strat], rnn_init ? "TAK" : "NIE");
                fflush(stdout);

                // Testujemy 100 tych samych instancji
                for(int i = 0; i < 100; i++){
                    Route* res = branch_and_bound(test_instances[i], n, timeout_seconds, mode);
                    
                    if(res){
                        times[i] = res->time; // Zapisujemy czas pojedynczej instancji
                        total_time += res->time;
                        
                        if(res->time >= timeout_seconds - 0.001)
                            timeouts++;
                        
                        free(res->city_order);
                        free(res);

                    }else {
                        times[i] = 0.0; // W razie błędu alokacji
                    }

                    // Aktualizacja paska postępu w tej samej linijce (\r)
                    fprintf(stdout, "\r\tAlgorytm: " ANSI_COLOR_CYAN "%-7s" ANSI_RESET_ALL " | RNN: " ANSI_COLOR_MAGENTA "%-3s" ANSI_RESET_ALL " | Postep: " ANSI_COLOR_YELLOW "%3d%%" ANSI_RESET_ALL, 
                            strategy_names[strat], rnn_init ? "TAK" : "NIE", i + 1);
                    fflush(stdout);
                }

                double avg_time = total_time / 100.0;
                
                // Wyliczenie wariancji i odchylenia standardowego
                double variance = 0.0;
                for(int i = 0; i < 100; i++)
                    variance += (times[i] - avg_time) * (times[i] - avg_time);

                double std_dev = sqrt(variance / 100.0);
                
                // Po zakończeniu 100 iteracji, nadpisujemy linię ładnym podsumowaniem wyników z Odchyleniem
                fprintf(stdout, "\r\tAlgorytm: " ANSI_COLOR_CYAN "%-7s" ANSI_RESET_ALL " | RNN: " ANSI_COLOR_MAGENTA "%-3s" ANSI_RESET_ALL " | " ANSI_COLOR_GREEN "Gotowe." ANSI_RESET_ALL " (Avg: " ANSI_COLOR_GREEN "%.4lfs" ANSI_RESET_ALL ", StdDev: " ANSI_COLOR_YELLOW "%.4lfs" ANSI_RESET_ALL ", Timeouts: " ANSI_COLOR_RED "%d%%" ANSI_RESET_ALL ")    \n", 
                        strategy_names[strat], rnn_init ? "TAK" : "NIE", avg_time, std_dev, timeouts);
                
                // Zapisujemy wszystkie parametry do pliku
                fprintf(file, "%d,%s,%d,%.9lf,%.9lf,%d\n", n, strategy_names[strat], rnn_init, avg_time, std_dev, timeouts);
                fflush(file);

            }

        }

        for(int i = 0; i < 100; i++)
            free(test_instances[i]);

        free(test_instances);

    }

    fclose(file);
    fprintf(stdout, "\nEksperyment zakonczony. Wyniki zapisano w " ANSI_STYLE_BOLD "bb_results_comparison.csv" ANSI_RESET_ALL ".\n");
    
}
