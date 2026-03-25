#include "libs.h"
#include "UI/TUI_func.h"
#include "data_operations/route_struct.h"

static const char* algorithms_names[6] = {"NN (fast)", "NN (stack)", "RNN (fast)", "RNN (stack)", "Random", "BF"};

int save_results_to_csv(Route**** results){
    
    if(!results){
        print_error("Cannot save: results array is NULL.\n");
        return 0;
    }

    char filename[256];

    for(int size_idx = 0; size_idx < 7; size_idx++){
        
        int true_size = size_idx + 8;
        
        snprintf(filename, sizeof(filename), "results_size_%d.csv", true_size);
        
        FILE* f = fopen(filename, "w");
        if(!f){
            print_error("Failed to open file for writing: ");
            fprintf(stderr, "%s\n", filename);
            return 0; 
        }

        fprintf(f, "Instance");
        for(int algo_idx = 0; algo_idx < 6; algo_idx++)
            fprintf(f, ",%s Time (s),%s Distance", algorithms_names[algo_idx], algorithms_names[algo_idx]);
        
        fprintf(f, "\n");

        for(int instance_idx = 0; instance_idx < 100; instance_idx++){
            
            fprintf(f, "%d", instance_idx + 1); 

            for(int algo_idx = 0; algo_idx < 6; algo_idx++){
                
                if(results[algo_idx] && results[algo_idx][size_idx] && results[algo_idx][size_idx][instance_idx]){
                    
                    double time = results[algo_idx][size_idx][instance_idx]->time;
                    uint64_t dist = results[algo_idx][size_idx][instance_idx]->distance_u;
                    
                    fprintf(f, ",%.9f,%lu", time, dist); 
                    
                }else {
                    fprintf(f, ",,");
                }

            }

            fprintf(f, "\n");

        }

        fclose(f);
        fprintf(stdout, "Saved " ANSI_STYLE_BOLD ANSI_COLOR_GREEN "%s" ANSI_RESET_ALL " successfully.\n", filename);
    }

    return 1;

}
