#include "libs.h"
#include "algorithms.h"
#include "UI/TUI_func.h"

Route* run_choosen_algorithm(int algorithm, int perms, uint32_t* distances, size_t num_points){

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
            result = rand_seq(distances, num_points, perms);
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
