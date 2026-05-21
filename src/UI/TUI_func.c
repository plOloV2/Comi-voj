#include "libs.h"
#include "UI/ansi_terminal.h"
#include "data_operations/route_struct.h"

void print_error(char* erro_msg){
    fprintf(stderr, ANSI_COLOR_RED ANSI_STYLE_BOLD "ERROR: %s" ANSI_RESET_ALL "\n", erro_msg);
}

int32_t user_input(int32_t lower_bound, int32_t higher_bound){

    int32_t input;

    while(scanf("%d", &input) != 1 || input > higher_bound || input < lower_bound){

        int32_t c;
        while((c = getchar()) != '\n' && c != EOF);

        fprintf(stdout, "You have chosen an incorrect value, try again (accepted value range: %d - %d): ", lower_bound, higher_bound);

    }

    return input;

}

void greeter(){

    fprintf(stdout, "\nWelcome to " ANSI_STYLE_ITALIC "Aleksander Dziagwa's" ANSI_RESET_ALL " first project for "
                ANSI_STYLE_BOLD "P" ANSI_RESET_ALL "rojektowanie "
                ANSI_STYLE_BOLD "E" ANSI_RESET_ALL "fektywnych "
                ANSI_STYLE_BOLD "A" ANSI_RESET_ALL "lgorytmow.\n");

    fprintf(stdout, "\nYou can " ANSI_STYLE_BOLD "exit" ANSI_RESET_ALL " this program at any time by just pressing "
            ANSI_STYLE_ITALIC ANSI_COLOR_YELLOW "CTRL" ANSI_RESET_ALL " + " ANSI_STYLE_ITALIC ANSI_COLOR_YELLOW "C.\n" ANSI_RESET_ALL);

}

int32_t start_choice(){

    fprintf(stdout, "\nWould you like to:"
                    "\n1. " ANSI_COLOR_BLUE     "Run calculations (P1)"             ANSI_RESET_ALL
                    "\n2. " ANSI_COLOR_GREEN    "Test implemented algorithms"       ANSI_RESET_ALL
                    "\n3. " ANSI_COLOR_MAGENTA  "Test implemented B&B (P2)"         ANSI_RESET_ALL
                    "\n4. " ANSI_COLOR_CYAN     "Test implemented TB (P3)"          ANSI_RESET_ALL);

    fprintf(stdout, "\nYour choice: ");

    return user_input(1, 4);

}

void display_full_ran_info(){

    fprintf(stdout, "\nStarting full calculations run.\n"
            ANSI_COLOR_YELLOW ANSI_STYLE_BOLD "ALL" ANSI_RESET_ALL " algorithms will be run " ANSI_COLOR_YELLOW ANSI_STYLE_ITALIC "100" ANSI_RESET_ALL " times for this data sizes: {"
            ANSI_STYLE_ITALIC "8, 9, 10, 11, 12, 13, 14" ANSI_RESET_ALL "}.\n");

}

int32_t display_test_menu(){

    fprintf(stdout, "\n"  ANSI_STYLE_BOLD "--- TEST MENU ---" ANSI_RESET_ALL "\n");
    fprintf(stdout, "1. " ANSI_COLOR_YELLOW     "Load data from a .txt file"            ANSI_RESET_ALL "\n");
    fprintf(stdout, "2. " ANSI_COLOR_BLUE       "Load data from a TSPLIB file"          ANSI_RESET_ALL "\n");
    fprintf(stdout, "3. " ANSI_COLOR_CYAN       "Generate random data"                  ANSI_RESET_ALL "\n");
    fprintf(stdout, "4. " ANSI_COLOR_GREEN      "Display last loaded/generated data"    ANSI_RESET_ALL "\n");
    fprintf(stdout, "5. " ANSI_COLOR_MAGENTA    "Run algorithm on current data"         ANSI_RESET_ALL "\n");
    fprintf(stdout, "0. " ANSI_COLOR_RED        "EXIT program"                          ANSI_RESET_ALL "\n");
    fprintf(stdout, "Your choice: ");

    return user_input(0, 5);

}

void get_file_path(char* path){

    char temp[256];

    fprintf(stdout, "\nPlease enter path to file: ");

    if(scanf("%255s", temp) != 1) 
        temp[0] = '\0';

    #ifdef _WIN32
        if(_fullpath(path, temp, 4096) == NULL){
    #else
        if(realpath(temp, path) == NULL){
    #endif
        print_error("Could not resolve path: ");
        fprintf(stdout, "%s\n   ", temp);
        return; 
    }

    fprintf(stdout, "\nSo you chose this file: " ANSI_COLOR_CYAN ANSI_STYLE_ITALIC "%s" ANSI_RESET_ALL " .\n", path);

}

int32_t get_rand_point_num(){

    fprintf(stdout, "\nPlease enter number of points to generate (6 to 100): ");

    return user_input(6, 100);

}

void disp_dist(uint32_t* dist, size_t num_points){

    if(!dist || num_points == 0){
        print_error("Distance table hasn't been initialized yet or number of points is set to 0\n");
        return;
    }

    fprintf(stdout, "\nNumber of points in table: " ANSI_STYLE_BOLD "%zu" ANSI_RESET_ALL "\n", num_points);
    fprintf(stdout, "Table with all distances, incorect ones are marked by " ANSI_STYLE_ITALIC ANSI_COLOR_YELLOW "UINT32_MAX" ANSI_RESET_ALL " (" ANSI_COLOR_RED "4294967295" ANSI_RESET_ALL "):\n\n");

    for(size_t i = 0; i < num_points; i++){

        for(size_t j = 0; j < num_points; j++){

            if(dist[i * num_points + j] == UINT32_MAX){
                fprintf(stdout, ANSI_COLOR_RED " %u " ANSI_RESET_ALL, dist[i * num_points + j]);
            }else {
                fprintf(stdout, " %u ", dist[i * num_points + j]);
            }
            
        }

        fprintf(stdout, "\n");

    }

}

void get_algorithm(int32_t* choice_algorithm, alg_in_data* data){

    fprintf(stdout, "\nWhich algorithm would you like to run?\n");
    fprintf(stdout, "1. " ANSI_COLOR_BLUE ANSI_STYLE_BOLD       "N" ANSI_RESET_ALL ANSI_COLOR_BLUE      "earest "       ANSI_STYLE_BOLD "N"
            ANSI_RESET_ALL ANSI_COLOR_BLUE  "eighbor (" ANSI_RESET_ALL ANSI_COLOR_YELLOW ANSI_STYLE_ITALIC  "fast" ANSI_RESET_ALL ANSI_COLOR_BLUE   ")\n"    ANSI_RESET_ALL);

    fprintf(stdout, "2. " ANSI_COLOR_BLUE ANSI_STYLE_BOLD       "N" ANSI_RESET_ALL ANSI_COLOR_BLUE      "earest "       ANSI_STYLE_BOLD "N"
            ANSI_RESET_ALL ANSI_COLOR_BLUE  "eighbor (" ANSI_RESET_ALL ANSI_COLOR_RED ANSI_STYLE_ITALIC     "stack" ANSI_RESET_ALL ANSI_COLOR_BLUE  ")\n"    ANSI_RESET_ALL);

    fprintf(stdout, "3. " ANSI_COLOR_CYAN ANSI_STYLE_BOLD       "R" ANSI_RESET_ALL ANSI_COLOR_CYAN      "epetitive "    ANSI_STYLE_BOLD "N"
            ANSI_RESET_ALL ANSI_COLOR_CYAN  "earest "           ANSI_STYLE_BOLD "N" ANSI_RESET_ALL ANSI_COLOR_CYAN "eighbor (" ANSI_RESET_ALL ANSI_COLOR_YELLOW ANSI_STYLE_ITALIC
            "fast" ANSI_RESET_ALL ANSI_COLOR_BLUE   ")\n" ANSI_RESET_ALL);

    fprintf(stdout, "4. " ANSI_COLOR_CYAN ANSI_STYLE_BOLD       "R" ANSI_RESET_ALL ANSI_COLOR_CYAN      "epetitive "    ANSI_STYLE_BOLD "N"
            ANSI_RESET_ALL ANSI_COLOR_CYAN  "earest "           ANSI_STYLE_BOLD "N" ANSI_RESET_ALL ANSI_COLOR_CYAN "eighbor (" ANSI_RESET_ALL ANSI_COLOR_RED ANSI_STYLE_ITALIC
            "stack" ANSI_RESET_ALL ANSI_COLOR_BLUE  ")\n" ANSI_RESET_ALL);

    fprintf(stdout, "5. " ANSI_COLOR_GREEN ANSI_STYLE_BOLD      "B" ANSI_RESET_ALL ANSI_COLOR_GREEN     "rute-"         ANSI_STYLE_BOLD "F"
            ANSI_RESET_ALL ANSI_COLOR_GREEN "orce (BF)\n"       ANSI_RESET_ALL);

    fprintf(stdout, "6. " ANSI_COLOR_MAGENTA ANSI_STYLE_BOLD    "R" ANSI_RESET_ALL ANSI_COLOR_MAGENTA   "andom algorithm\n" ANSI_RESET_ALL);

    fprintf(stdout, "7. " ANSI_COLOR_YELLOW ANSI_STYLE_BOLD     "B" ANSI_RESET_ALL ANSI_COLOR_YELLOW    "ranch "        ANSI_STYLE_BOLD "& B"
            ANSI_RESET_ALL ANSI_COLOR_YELLOW "ound (B&B)\n"       ANSI_RESET_ALL);

    fprintf(stdout, "8. " ANSI_COLOR_RED ANSI_STYLE_BOLD        "T" ANSI_RESET_ALL ANSI_COLOR_RED    "abu "        ANSI_STYLE_BOLD "S"
            ANSI_RESET_ALL ANSI_COLOR_RED    "earch (TS)\n"       ANSI_RESET_ALL);

    fprintf(stdout, "Your choice: ");

    *choice_algorithm = user_input(1, 8);

    if(*choice_algorithm == 6){

        fprintf(stdout, "\nYou have chosen the " ANSI_COLOR_MAGENTA "Random algorithm" ANSI_RESET_ALL ".\n");
        fprintf(stdout, "Please specify the number of permutations to generate (0 to 200, 0 -> 10N): ");
        data->perms = user_input(0, 200);

    }else if(*choice_algorithm == 7){
        
        fprintf(stdout, "\nYou have chosen " ANSI_COLOR_YELLOW "Branch & Bound" ANSI_RESET_ALL ".\n");
        
        fprintf(stdout, "Choose search strategy (0: DFS, 1: BFS, 2: Best-FS): ");
        int32_t strat = user_input(0, 2);
        
        fprintf(stdout, "Use RNN to find initial Upper Bound? (1: Yes, 0: No): ");
        int32_t rnn = user_input(0, 1);
        
        data->mode = (strat << 1) | rnn;

        fprintf(stdout, "Enter timeout in seconds (e.g. 120): ");
        while(scanf("%lf", &data->max_time) != 1 || data->max_time <= 0.0){
            int32_t c;
            while((c = getchar()) != '\n' && c != EOF);
            fprintf(stdout, "Incorrect timeout value. Try again: ");
        }

    }else if(*choice_algorithm == 8){

        fprintf(stdout, "\nYou have chosen " ANSI_COLOR_RED "Tabu Search" ANSI_RESET_ALL ".");
        fprintf(stdout, "\nShould the algorytm start with random route (0) or with one from RNN (1): ");
        data->use_RNN = user_input(0, 1);

        fprintf(stdout, "\nFor how many iterations should the algorithm run: ");
        data->max_iter = user_input(1, INT32_MAX);

        fprintf(stdout, "\nHow big should be the sample in each cycle: ");
        data->sample_size = user_input(1, 1000);

        fprintf(stdout, "\nAfter how many cycles without imprevment should the algorithm randomize route: ");
        data->max_no_up = user_input(1, INT32_MAX);

        fprintf(stdout, "\nMinimum cycles for move to stay tabu: ");
        data->min_iter_stop = user_input(1, 1000);

        fprintf(stdout, "\nMaximum cycles for move to stay tabu: ");
        data->max_iter_stop = user_input(1, 1000);

    }
    
    
}

void display_Route(Route* data, size_t num_points){

    if(!data || !data->city_order || data->distance_u == 0 || data->distance_u == UINT64_MAX || data->time < 0.0){
        print_error("Route* provided is broken.\n");
        return;
    }

    fprintf(stdout, "Algorithm found route of length " ANSI_COLOR_GREEN ANSI_STYLE_BOLD "%llu" ANSI_RESET_ALL " in " ANSI_COLOR_BLUE ANSI_STYLE_BOLD "%.6lfs" ANSI_RESET_ALL ".\n", (unsigned long long)data->distance_u, data->time);
    fprintf(stdout, "Route found:\n");

    for(size_t i = 0; i < num_points; i++)
        fprintf(stdout, "%hu -> ", data->city_order[i]);

    fprintf(stdout, "%hu\n", data->city_order[0]);

}

int32_t check_data_created(uint32_t* dist, size_t num_points){

    if(!dist || num_points <= 2){

        print_error("You have to create some data for algorithms first. Go back and do that.\n");

        return 0;
    }

    return 1;

}
