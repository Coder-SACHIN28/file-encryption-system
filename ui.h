#ifndef UI_H
#define UI_H

// Color codes for terminal
#define COLOR_RESET   "\x1b[0m"
#define COLOR_GREEN   "\x1b[32m"
#define COLOR_CYAN    "\x1b[36m"
#define COLOR_RED     "\x1b[31m"
#define COLOR_YELLOW  "\x1b[33m"
#define COLOR_BOLD    "\x1b[1m"
#define COLOR_DIM     "\x1b[2m"

// Simple ASCII box drawing characters (works everywhere!)
#define BOX_TL "+"  // Top-left
#define BOX_TR "+"  // Top-right
#define BOX_BL "+"  // Bottom-left
#define BOX_BR "+"  // Bottom-right
#define BOX_H  "="  // Horizontal
#define BOX_V  "|"  // Vertical
#define BOX_VR "+"  // Vertical-right
#define BOX_VL "+"  // Vertical-left

// UI Functions
void clear_screen();
void print_header();
void print_success(const char *msg);
void print_error(const char *msg);
void print_info(const char *msg);
void print_warning(const char *msg);
void draw_box(int width, const char *title);
void show_progress(int current, int total);
void animate_loading(const char *text, int duration_ms);
void print_cyberpunk_banner();
void draw_menu_box();
void print_separator();

#endif
