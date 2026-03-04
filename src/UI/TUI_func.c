#include "libs.h"
#include "UI/ansi_terminal.h"

void print_error(char* erro_msg){

    fprintf(stderr, ANSI_COLOR_RED ANSI_STYLE_BOLD "%s", erro_msg);

}

int startup_sentence(char* file_loc, size_t* num_points){

    fprintf(stdout, "Welcome to " ANSI_STYLE_ITALIC "Aleksander Dziagwa's" ANSI_RESET_ALL " project for "
            ANSI_STYLE_BOLD "P" ANSI_RESET_ALL "rojektowanie "
            ANSI_STYLE_BOLD "E" ANSI_RESET_ALL "fektywnych "
            ANSI_STYLE_BOLD "A" ANSI_RESET_ALL "lgorytmow.\n");

    fprintf(stdout, "\nYou can exit this program at any time by just presing " ANSI_STYLE_ITALIC "CTRL" ANSI_RESET_ALL " + " ANSI_STYLE_ITALIC "C.\n" ANSI_RESET_ALL);

    fprintf(stdout, "\nWhich set of algorithms would You like to run?\n"
            "(Select one of this options and press " ANSI_STYLE_ITALIC "ENTER" ANSI_RESET_ALL ")\n"
            "1. " ANSI_COLOR_BLUE "Naive algorithm\n" ANSI_RESET_ALL
            "2. " ANSI_COLOR_YELLOW "Brute-force algorithm\n" ANSI_RESET_ALL
            "4. " ANSI_COLOR_GREEN " - algorithm\n" ANSI_RESET_ALL
            "3. " ANSI_COLOR_MAGENTA " - algorithm\n" ANSI_RESET_ALL
            "5. " ANSI_COLOR_YELLOW ANSI_STYLE_BOLD "All of them!\n" ANSI_RESET_ALL
            "Your current choice: ");

}
