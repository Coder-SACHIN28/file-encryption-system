#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "ui.h"

#ifdef _WIN32
#include <windows.h>
#define SLEEP(ms) Sleep(ms)
#else
#include <unistd.h>
#define SLEEP(ms) usleep((ms)*1000)
#endif

void clear_screen() {
#ifdef _WIN32
    system("cls");
#else
    system("clear");
#endif
}

void print_cyberpunk_banner() {
    printf("\n");
    printf(COLOR_GREEN COLOR_BOLD);
    printf("    +===========================================================+\n");
    printf("    |                                                           |\n");
    printf("    |     #####  ####  #####     #####  ####  #####            |\n");
    printf("    |    ##  ## ##    ##         ## ## ##    ##                |\n");
    printf("    |    ##### #####  #####      ####  ##### #####             |\n");
    printf("    |    ##  ## ##       ##       ##       ## ##                |\n");
    printf("    |    ##  ## ####  #####       ####  ##### #####            |\n");
    printf("    |                                                           |\n");
    printf("    |          + VIGENERE DUAL-LAYER ENCRYPTION SYSTEM         |\n");
    printf("    |                                                           |\n");
    printf("    |               " COLOR_CYAN ">> MILITARY-GRADE SECURITY" COLOR_GREEN "                |\n");
    printf("    |               " COLOR_YELLOW "!! UNAUTHORIZED ACCESS PROHIBITED" COLOR_GREEN "      |\n");
    printf("    |                                                           |\n");
    printf("    +===========================================================+\n");
    printf(COLOR_RESET);
    printf("\n");
    printf(COLOR_DIM "    [SYSTEM v4.0] [BUILD 2025.11.18] [STATUS: OPERATIONAL]" COLOR_RESET "\n");
    printf("\n");
}

void draw_menu_box() {
    printf(COLOR_GREEN);
    printf("    +===========================================================+\n");
    printf("    |                    " COLOR_BOLD "** MAIN MENU **" COLOR_RESET COLOR_GREEN "                        |\n");
    printf("    +===========================================================+\n");
    printf("    |                                                           |\n");
    printf("    |  " COLOR_CYAN "  [1]" COLOR_RESET " >> Encrypt File (AES-256 + Vigenere)            " COLOR_GREEN "|\n");
    printf("    |  " COLOR_CYAN "  [2]" COLOR_RESET " >> Decrypt File (Dual-Layer Security)           " COLOR_GREEN "|\n");
    printf("    |  " COLOR_CYAN "  [3]" COLOR_RESET " >> Password Strength Analyzer                   " COLOR_GREEN "|\n");
    printf("    |  " COLOR_CYAN "  [4]" COLOR_RESET " >> Supported File Types                         " COLOR_GREEN "|\n");
    printf("    |  " COLOR_CYAN "  [5]" COLOR_RESET " >> Algorithm Information                        " COLOR_GREEN "|\n");
    printf("    |  " COLOR_CYAN "  [6]" COLOR_RESET " >> View Encryption History                      " COLOR_GREEN "|\n");
    printf("    |  " COLOR_RED "  [0]" COLOR_RESET " >> Exit System                                   " COLOR_GREEN "|\n");
    printf("    |                                                           |\n");
    printf("    +===========================================================+\n");
    printf(COLOR_RESET);
    printf("\n");
    printf(COLOR_CYAN "    >> ADMIN_ACCESS" COLOR_RESET " $ ");
}

void print_success(const char *msg) {
    printf(COLOR_GREEN "    + [SUCCESS] %s" COLOR_RESET "\n", msg);
}

void print_error(const char *msg) {
    printf(COLOR_RED "    X [ERROR] %s" COLOR_RESET "\n", msg);
}

void print_info(const char *msg) {
    printf(COLOR_CYAN "    i [INFO] %s" COLOR_RESET "\n", msg);
}

void print_warning(const char *msg) {
    printf(COLOR_YELLOW "    ! [WARNING] %s" COLOR_RESET "\n", msg);
}

void print_separator() {
    printf(COLOR_DIM "    --------------------------------------------------------\n" COLOR_RESET);
}

void show_progress(int current, int total) {
    int bar_width = 40;
    float percent = (float)current / total;
    int filled = (int)(percent * bar_width);
    
    printf("\r" COLOR_CYAN "    Progress: " COLOR_GREEN "[");
    for(int i = 0; i < bar_width; i++) {
        if(i < filled) printf("#");
        else printf("-");
    }
    printf("]" COLOR_CYAN " %.1f%%" COLOR_RESET, percent * 100);
    fflush(stdout);
    
    if(current == total) printf("\n");
}

void animate_loading(const char *text, int duration_ms) {
    const char spinner[] = "|/-\\";
    int spinner_len = 4;
    int iterations = duration_ms / 100;
    
    for(int i = 0; i < iterations; i++) {
        printf("\r" COLOR_CYAN "    %c %s..." COLOR_RESET, spinner[i % spinner_len], text);
        fflush(stdout);
        SLEEP(100);
    }
    printf("\r" COLOR_GREEN "    + %s... DONE" COLOR_RESET "\n", text);
}

void draw_box(int width, const char *title) {
    printf(COLOR_GREEN);
    
    // Top border
    printf("    %s", BOX_TL);
    for(int i = 0; i < width - 2; i++) printf("%s", BOX_H);
    printf("%s\n", BOX_TR);
    
    // Title
    if(title) {
        int title_len = strlen(title);
        int padding = (width - title_len - 4) / 2;
        printf("    %s ", BOX_V);
        for(int i = 0; i < padding; i++) printf(" ");
        printf(COLOR_BOLD "%s" COLOR_RESET COLOR_GREEN, title);
        for(int i = 0; i < padding; i++) printf(" ");
        printf(" %s\n", BOX_V);
        
        // Separator
        printf("    %s", BOX_VR);
        for(int i = 0; i < width - 2; i++) printf("%s", BOX_H);
        printf("%s\n", BOX_VL);
    }
    
    printf(COLOR_RESET);
}

void draw_box_end(int width) {
    printf(COLOR_GREEN);
    printf("    %s", BOX_BL);
    for(int i = 0; i < width - 2; i++) printf("%s", BOX_H);
    printf("%s\n", BOX_BR);
    printf(COLOR_RESET);
}
