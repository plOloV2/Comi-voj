#include "libs.h"
#include "algorithms.h"
#include "UI/TUI_func.h"
#include "data_operations/data_prepare.h"

Route* run_choosen_algorithm(int algorithm, int perms, uint32_t* distances, size_t num_points);

int main(){

    greeter();

    uint64_t seed;
    create_rand_seed(&seed);

    xoshiro256_state RNG;
    xoshiro_init(&RNG, seed); 

    if(start_choice() == 1){

        print_error("test");

    }else{

        int run = 1;

        uint32_t* dist = NULL;
        size_t num_points = 0;
        Route* result = NULL;

        while(run){

            int conf = display_test_menu();

            switch(conf){
                case 0:
                    run = 0;
                    break;

                case 1:
                    free(dist);
                    char path[256];
                    get_file_path(path);
                    dist = parse_file(path, &num_points);
                    break;
                
                case 2:
                    free(dist);
                    num_points = get_rand_point_num();
                    dist = create_random_distances(num_points, &RNG);
                    break;
                
                case 3:
                    disp_dist(dist, num_points);
                    break;
                
                case 4:
                    int good_to_go = check_data_created(dist, num_points);
                    if(!good_to_go)
                        break;

                    int algorithm = 0, perms = 0;
                    get_algorithm(&algorithm, &perms);

                    result = run_choosen_algorithm(algorithm, perms, dist, num_points);
                    display_Route(result, num_points);

                    if(result)
                        free(result->city_order);
                        
                    free(result);
                    result = NULL;

                    break;
                
            }

        }
        
        free(result);

    }

    return 0;

}
