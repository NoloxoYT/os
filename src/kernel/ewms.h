#ifndef EWMS_H
#define EWMS_H

#include "stdint.h"

#define MAX_WINDOWS 10
#define WIN_BORDER_COLOR 0x0F
#define WIN_TITLE_COLOR  0x0E
#define WIN_BG_COLOR     0x07

typedef struct {
    uint16_t x, y;
    uint16_t width, height;
    uint8_t  is_active;
    char     title[32];
    char     buffer[80*25];
} Window;

void ewms_init();
void ewms_create_window(uint16_t x, uint16_t y, uint16_t w, uint16_t h, const char* title);
void ewms_draw_window(Window* win);
void ewms_handle_mouse(uint8_t mouse_x, uint8_t mouse_y, uint8_t buttons);
Window* ewms_get_active_window();

#endif