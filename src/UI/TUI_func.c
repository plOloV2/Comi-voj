#include "libs.h"
#include "UI/ansi_terminal.h"

void print_error(char* erro_msg){

    fprintf(stderr, ANSI_COLOR_RED ANSI_STYLE_BOLD "%s", erro_msg);

}

static int user_input(int lower_bound, int higher_bound){

    int input;

    while(scanf("%d", &input) != 1 || input > higher_bound || input < lower_bound){
        
        int c;
        while((c = getchar()) != '\n' && c != EOF);

        fprintf(stdout, "\nYou have chosen an incorrect value, try again (accepted value range: %d - %d): ", lower_bound, higher_bound);

    }

    return input;

}

uint32_t startup_sentence(char* file_loc, size_t* num_points){

    fprintf(stdout, "\n\nWelcome to " ANSI_STYLE_ITALIC "Aleksander Dziagwa's" ANSI_RESET_ALL " project for "
            ANSI_STYLE_BOLD "P" ANSI_RESET_ALL "rojektowanie "
            ANSI_STYLE_BOLD "E" ANSI_RESET_ALL "fektywnych "
            ANSI_STYLE_BOLD "A" ANSI_RESET_ALL "lgorytmow.\n");

    fprintf(stdout, "\nYou can " ANSI_STYLE_BOLD "exit" ANSI_RESET_ALL " this program at any time by just presing "
            ANSI_STYLE_ITALIC ANSI_COLOR_YELLOW "CTRL" ANSI_RESET_ALL " + " ANSI_STYLE_ITALIC ANSI_COLOR_YELLOW "C.\n" ANSI_RESET_ALL);

    fprintf(stdout, "\nWhich set of algorithms would You like to run?\n"
            "(Select one of this options and press " ANSI_STYLE_ITALIC "ENTER" ANSI_RESET_ALL ")\n"
            "1. " ANSI_COLOR_BLUE       "Naive algorithm\n"         ANSI_RESET_ALL
            "2. " ANSI_COLOR_CYAN       "Brute-force algorithm\n"   ANSI_RESET_ALL
            "3. " ANSI_COLOR_GREEN      " - algorithm\n"            ANSI_RESET_ALL
            "4. " ANSI_COLOR_MAGENTA    " - algorithm\n"            ANSI_RESET_ALL
            "5. " ANSI_COLOR_YELLOW     "All of them!\n"            ANSI_RESET_ALL
            "Your current choice: ");

    int choice_algorithm = user_input(1, 5);

    fprintf(stdout, "\nSo You have choosen: ");
    switch(choice_algorithm){
        case 1:
            fprintf(stdout, ANSI_COLOR_BLUE     "Naive algorithm"       ANSI_RESET_ALL);
            break;
        
        case 2:
            fprintf(stdout, ANSI_COLOR_CYAN     "Brute-force algorithm" ANSI_RESET_ALL);
            break;
        
        case 3:
            fprintf(stdout, ANSI_COLOR_GREEN    " - algorithm"          ANSI_RESET_ALL);
            break;
        
        case 4:
            fprintf(stdout, ANSI_COLOR_MAGENTA  " - algorithm"          ANSI_RESET_ALL);
            break;
        
        case 5:
            fprintf(stdout, ANSI_COLOR_YELLOW   "All of them!"          ANSI_RESET_ALL);
            break;
    }

    fprintf(stdout, ".\n\nWould You like to run:"
            "\n1. " ANSI_COLOR_YELLOW "test on provided data"     ANSI_RESET_ALL
            "\n2. " ANSI_COLOR_CYAN   "simualtion on random data" ANSI_RESET_ALL
            "\nYour current choice: ");

    int choice_data = user_input(1, 2);
    int num_runs = 1;

    fprintf(stdout, "\nSo You have choosen: ");
    
    if(choice_data == 1){

        fprintf(stdout, ANSI_COLOR_YELLOW "test on provided data."     ANSI_RESET_ALL
                "\nNow please type in path to the file cointaning data to run algorithm(s): ");

        if(scanf("%255s", file_loc) != 1) 
            file_loc[0] = '\0';

        fprintf(stdout, "\nSo you choose this file: " ANSI_COLOR_CYAN ANSI_STYLE_ITALIC "%s" ANSI_RESET_ALL " .\n", file_loc);
        

    }else{

        fprintf(stdout, ANSI_COLOR_CYAN   "simualtion on random data." ANSI_RESET_ALL
                "\nNow please type in how big should the data set be (" ANSI_STYLE_ITALIC "my suggestion is to choose from 12 to 20" ANSI_RESET_ALL "): ");

        *num_points = (size_t)user_input(3, 50);

        fprintf(stdout, "\nAnd how many times should the algoritm(s) be run?\nNumber of runs (" ANSI_STYLE_ITALIC "suggested range 1 - 200" ANSI_RESET_ALL "): ");

        num_runs = user_input(1, 1000);

        fprintf(stdout, "\nSo You have choosen the data set to have " ANSI_COLOR_GREEN "%d" ANSI_RESET_ALL " points "
                "and the algorithm(s) to run " ANSI_COLOR_MAGENTA "%d" ANSI_RESET_ALL " times.\n", (int)*num_points, num_runs);

    }

    return ((uint32_t)num_runs << 16) | ((uint32_t)choice_data << 8) | (uint32_t)choice_algorithm;
    
}
