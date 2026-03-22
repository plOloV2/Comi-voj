#include "libs.h"
#include "UI/ansi_terminal.h"

void print_error(char* erro_msg){
    fprintf(stderr, ANSI_COLOR_RED ANSI_STYLE_BOLD "ERROR: %s" ANSI_RESET_ALL "\n", erro_msg);
}

static int user_input(int lower_bound, int higher_bound){
    int input;
    while(scanf("%d", &input) != 1 || input > higher_bound || input < lower_bound){
        int c;
        while((c = getchar()) != '\n' && c != EOF);
        fprintf(stdout, "You have chosen an incorrect value, try again (accepted value range: %d - %d): ", lower_bound, higher_bound);
    }
    return input;
}

// Funkcja wyświetlająca powitanie i główne menu (zastępuje stare startup_sentence)
int display_main_menu(){

    static int first_run = 1;

    if (first_run) {
        fprintf(stdout, "\nWelcome to " ANSI_STYLE_ITALIC "Aleksander Dziagwa's" ANSI_RESET_ALL " first project for "
                ANSI_STYLE_BOLD "P" ANSI_RESET_ALL "rojektowanie "
                ANSI_STYLE_BOLD "E" ANSI_RESET_ALL "fektywnych "
                ANSI_STYLE_BOLD "A" ANSI_RESET_ALL "lgorytmow.\n");
        first_run = 0;
    }

    fprintf(stdout, "\n" ANSI_STYLE_BOLD "--- MAIN MENU ---" ANSI_RESET_ALL "\n");
    fprintf(stdout, "1. " ANSI_COLOR_YELLOW "Load data from a file" ANSI_RESET_ALL "\n");
    fprintf(stdout, "2. " ANSI_COLOR_CYAN "Generate random data" ANSI_RESET_ALL "\n");
    fprintf(stdout, "3. " ANSI_COLOR_GREEN "Display last loaded/generated data" ANSI_RESET_ALL "\n");
    fprintf(stdout, "4. " ANSI_COLOR_MAGENTA "Run algorithm on current data" ANSI_RESET_ALL "\n");
    fprintf(stdout, "0. " ANSI_COLOR_RED "Exit program" ANSI_RESET_ALL "\n");
    fprintf(stdout, "Your choice: ");

    return user_input(0, 4);
}

// Funkcja wywoływana, gdy w menu głównym wybrano opcję 4
void run_algorithm_menu(int* choice_algorithm, int* num_permutations) {
    fprintf(stdout, "\nWhich algorithm would you like to run?\n");
    fprintf(stdout, "1. " ANSI_COLOR_BLUE ANSI_STYLE_BOLD "N" ANSI_RESET_ALL ANSI_COLOR_BLUE "earest " ANSI_STYLE_BOLD "N" ANSI_RESET_ALL ANSI_COLOR_BLUE "eighbor (NN)\n" ANSI_RESET_ALL);
    fprintf(stdout, "2. " ANSI_COLOR_CYAN ANSI_STYLE_BOLD "R" ANSI_RESET_ALL ANSI_COLOR_CYAN "epetitive " ANSI_STYLE_BOLD "N" ANSI_RESET_ALL ANSI_COLOR_CYAN "earest " ANSI_STYLE_BOLD "N" ANSI_RESET_ALL ANSI_COLOR_CYAN "eighbor (RNN)\n" ANSI_RESET_ALL);
    fprintf(stdout, "3. " ANSI_COLOR_GREEN ANSI_STYLE_BOLD "B" ANSI_RESET_ALL ANSI_COLOR_GREEN "rute-" ANSI_STYLE_BOLD "F" ANSI_RESET_ALL ANSI_COLOR_GREEN "orce (BF)\n" ANSI_RESET_ALL);
    fprintf(stdout, "4. " ANSI_COLOR_MAGENTA ANSI_STYLE_BOLD "R" ANSI_RESET_ALL ANSI_COLOR_MAGENTA "andom algorithm\n" ANSI_RESET_ALL);
    fprintf(stdout, "Your choice: ");

    *choice_algorithm = user_input(1, 4);

    // Zgodnie z wymaganiami, jeśli wybrano algorytm losowy (4), pytamy o permutacje
    if (*choice_algorithm == 4) {
        fprintf(stdout, "\nYou have chosen the " ANSI_COLOR_MAGENTA "Random algorithm" ANSI_RESET_ALL ".\n");
        fprintf(stdout, "Please specify the number of permutations to generate: ");
        *num_permutations = user_input(1, 1000); // Dostosuj górny limit według uznania
    } else {
        *num_permutations = 0; // Wartość ignorowana dla innych algorytmów
    }
}
