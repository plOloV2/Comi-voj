#include "libs.h"
#include "algorithms.h"
#include "UI/TUI_func.h"
#include "data_operations/data_prepare.h"

Route* run_choosen_algorithm(int32_t algorithm, alg_in_data* data, uint32_t* distances, size_t num_points);
Route**** run_whole_calculation(uint32_t*** data_table);
int32_t save_results_to_csv(Route**** results);
void run_bb_experiment(double timeout_seconds);
void run_TB_experiment();
void run_GA_experiment();


int32_t main(){

    greeter();

    int32_t decision = start_choice();

    if(decision == 1){

        display_full_ran_info();

        fprintf(stdout, "\nCreating random data... ");

        double cz = omp_get_wtime();
        uint32_t*** data_table = create_random_distances_for_calc();
        cz = (omp_get_wtime() - cz) * 1000.0;

        fprintf(stdout, "done (took %.3lfms)\n\n", cz);
        cz = omp_get_wtime();

        Route**** calc_results = run_whole_calculation(data_table);

        if(save_results_to_csv(calc_results)){
            fprintf(stdout, "All was saved safely.\n");
        }

        for(int32_t i = 0; i < 6; i++){

            if(calc_results[i]){

                for(int32_t j = 0; j < 7; j++){

                    if(calc_results[i][j]){

                        for(int32_t k = 0; k < 100; k++){

                            if(calc_results[i][j][k]){
                                free(calc_results[i][j][k]->city_order);
                                free(calc_results[i][j][k]);
                            }

                        }

                        free(calc_results[i][j]);
                    }

                }

                free(calc_results[i]);
            }

        }

        free(calc_results);

        for(int32_t i = 0; i < 7; i++){
            if(data_table[i]){
                for(int32_t j = 0; j < 100; j++)
                    free(data_table[i][j]);
                
                free(data_table[i]);

            }

        }
        
        free(data_table);

    }else if(decision == 2){

        int32_t run = 1;

        uint32_t* dist = NULL;
        size_t num_points = 0;
        Route* result = NULL;

        uint64_t seed;
        create_rand_seed(&seed);

        xoshiro256_state RNG;
        xoshiro_init(&RNG, seed);

        alg_in_data data;

        while(run){

            int32_t conf = display_test_menu();

            char path[256];


            switch(conf){
                case 0:
                    run = 0;
                    break;

                case 1:
                    free(dist);
                    get_file_path(path);
                    dist = parse_file(path, &num_points);
                    break;

                case 2:
                    free(dist);
                    get_file_path(path);
                    dist = read_data_from_TSPLIB(path, &num_points, 0);
                    break;
                
                case 3:
                    free(dist);
                    num_points = get_rand_point_num();
                    dist = create_random_distances(num_points, &RNG);
                    break;
                
                case 4:
                    disp_dist(dist, num_points);
                    break;
                
                case 5:
                    int32_t good_to_go = check_data_created(dist, num_points);
                    if(!good_to_go)
                        break;

                    int32_t algorithm = 0;
                    
                    get_algorithm(&algorithm, &data);

                    result = run_choosen_algorithm(algorithm, &data, dist, num_points);
                    display_Route(result, num_points);

                    if(result)
                        free(result->city_order);
                        
                    free(result);
                    result = NULL;

                    break;

                default:
                    print_error("Something unexpected happend. How did we get here?");
                    break;
                
            }

        }
        
        free(result);

    }else if(decision == 3){

        run_bb_experiment(240.0);

    } else if(decision == 4){

        run_TB_experiment();

    } else if(decision == 5){ 

        run_GA_experiment();

    }

    return 0;

}
