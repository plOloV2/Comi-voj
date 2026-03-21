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

    fprintf(stdout, "\n\nWelcome to " ANSI_STYLE_ITALIC "Aleksander Dziagwa's" ANSI_RESET_ALL " first project for "
            ANSI_STYLE_BOLD "P" ANSI_RESET_ALL "rojektowanie "
            ANSI_STYLE_BOLD "E" ANSI_RESET_ALL "fektywnych "
            ANSI_STYLE_BOLD "A" ANSI_RESET_ALL "lgorytmow.\n");

    fprintf(stdout, "\nYou can " ANSI_STYLE_BOLD "exit" ANSI_RESET_ALL " this program at any time by just pressing "
            ANSI_STYLE_ITALIC ANSI_COLOR_YELLOW "CTRL" ANSI_RESET_ALL " + " ANSI_STYLE_ITALIC ANSI_COLOR_YELLOW "C.\n" ANSI_RESET_ALL);

    fprintf(stdout, "\nWould you like to:\n"
            "1. " ANSI_COLOR_YELLOW "Test a specific algorithm" ANSI_RESET_ALL "\n"
            "2. " ANSI_COLOR_MAGENTA "Run the whole program (All algorithms on random data)" ANSI_RESET_ALL "\n"
            "Your current choice: ");

    int choice_mode = user_input(1, 2);
    
    int choice_algorithm = 5;
    int choice_data = 2;
    int num_runs = 1;

    if (choice_mode == 1) {
        fprintf(stdout, "\nWhich algorithm would you like to test?\n"
                "(Select one of these options and press " ANSI_STYLE_ITALIC "ENTER" ANSI_RESET_ALL ")\n"
                "1. " ANSI_COLOR_BLUE ANSI_STYLE_BOLD "N" ANSI_RESET_ALL ANSI_COLOR_BLUE "earest " ANSI_STYLE_BOLD "N" ANSI_RESET_ALL ANSI_COLOR_BLUE "eighbor algorithm\n" ANSI_RESET_ALL
                "2. " ANSI_COLOR_CYAN ANSI_STYLE_BOLD "R" ANSI_RESET_ALL ANSI_COLOR_CYAN "epetitive " ANSI_STYLE_BOLD "N" ANSI_RESET_ALL ANSI_COLOR_CYAN "earest " ANSI_STYLE_BOLD "N" ANSI_RESET_ALL ANSI_COLOR_CYAN "eighbor algorithm\n" ANSI_RESET_ALL
                "3. " ANSI_COLOR_GREEN ANSI_STYLE_BOLD "B" ANSI_RESET_ALL ANSI_COLOR_GREEN "rute-" ANSI_STYLE_BOLD "F" ANSI_RESET_ALL ANSI_COLOR_GREEN "orce algorithm\n" ANSI_RESET_ALL
                "4. " ANSI_COLOR_MAGENTA ANSI_STYLE_BOLD "R" ANSI_RESET_ALL ANSI_COLOR_MAGENTA "andom algorithm\n" ANSI_RESET_ALL
                "Your current choice: ");

        choice_algorithm = user_input(1, 4);

        fprintf(stdout, "\nSo you have chosen: ");
        switch(choice_algorithm){
            case 1:
                fprintf(stdout, ANSI_COLOR_BLUE ANSI_STYLE_BOLD "N" ANSI_RESET_ALL ANSI_COLOR_BLUE "earest " ANSI_STYLE_BOLD "N" ANSI_RESET_ALL ANSI_COLOR_BLUE "eighbor algorithm" ANSI_RESET_ALL);
                break;
            case 2:
                fprintf(stdout, ANSI_COLOR_CYAN ANSI_STYLE_BOLD "R" ANSI_RESET_ALL ANSI_COLOR_CYAN "epetitive " ANSI_STYLE_BOLD "N" ANSI_RESET_ALL ANSI_COLOR_CYAN "earest " ANSI_STYLE_BOLD "N" ANSI_RESET_ALL ANSI_COLOR_CYAN "eighbor algorithm" ANSI_RESET_ALL);
                break;
            case 3:
                fprintf(stdout, ANSI_COLOR_GREEN ANSI_STYLE_BOLD "B" ANSI_RESET_ALL ANSI_COLOR_GREEN "rute-" ANSI_STYLE_BOLD "F" ANSI_RESET_ALL ANSI_COLOR_GREEN "orce algorithm" ANSI_RESET_ALL);
                break;
            case 4:
                fprintf(stdout, ANSI_COLOR_MAGENTA ANSI_STYLE_BOLD "R" ANSI_RESET_ALL ANSI_COLOR_MAGENTA "andom algorithm" ANSI_RESET_ALL);
                break;
        }

        fprintf(stdout, ".\n\nWould you like to use:\n"
                "1. " ANSI_COLOR_YELLOW "Distance table loaded from a file" ANSI_RESET_ALL "\n"
                "2. " ANSI_COLOR_CYAN   "Randomly generated distance table" ANSI_RESET_ALL "\n"
                "Your current choice: ");

        choice_data = user_input(1, 2);
    } 
    else {
        fprintf(stdout, "\nSo you have chosen to run the " ANSI_COLOR_MAGENTA "whole program" ANSI_RESET_ALL ".\n");
    }

    if (choice_data == 1) {
        fprintf(stdout, "\nNow please type in the path to the file containing data to run the algorithm(s): ");

        if(scanf("%255s", file_loc) != 1) 
            file_loc[0] = '\0';

        fprintf(stdout, "\nSo you chose this file: " ANSI_COLOR_CYAN ANSI_STYLE_ITALIC "%s" ANSI_RESET_ALL " .\n", file_loc);
        
    } else {
        fprintf(stdout, "\nNow please type in how big the data set should be (" ANSI_STYLE_ITALIC "my suggestion is to choose from 12 to 20" ANSI_RESET_ALL "): ");

        *num_points = (size_t)user_input(3, 50);

        fprintf(stdout, "\nAnd how many times should the algorithm(s) be run?\nNumber of runs (" ANSI_STYLE_ITALIC "suggested range 1 - 200" ANSI_RESET_ALL "): ");

        num_runs = user_input(1, 1000);

        fprintf(stdout, "\nSo you have chosen the data set to have " ANSI_COLOR_GREEN "%d" ANSI_RESET_ALL " points "
                "and the algorithm(s) to run " ANSI_COLOR_MAGENTA "%d" ANSI_RESET_ALL " times.\n", (int)*num_points, num_runs);
    }

    return ((uint32_t)num_runs << 16) | ((uint32_t)choice_data << 8) | (uint32_t)choice_algorithm;
}
