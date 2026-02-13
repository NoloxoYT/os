#include "ewms.h"
#include "io.h"
#include "drivers.h"

Window windows[MAX_WINDOWS];
int window_count = 0;
int active_window = 0;

extern volatile char* video;

void ewms_init() {
    window_count = 0;
    active_window = -1;
}

void ewms_create_window(uint16_t x, uint16_t y, uint16_t w, uint16_t h, const char* title) {
    if (window_count >= MAX_WINDOWS) return;
    Window* win = &windows[window_count++];
    win->x = x; win->y = y;
    win->width = w; win->height = h;
    win->is_active = 1;
    strcpy(win->title, title);
    ewms_draw_window(win);
    active_window = window_count - 1;
}

void ewms_draw_window(Window* win) {
    for (int dy = 0; dy < win->height; dy++) {
        for (int dx = 0; dx < win->width; dx++) {
            int pos = (win->y + dy) * 160 + (win->x + dx) * 2;
            if (dx == 0 || dx == win->width-1 || dy == 0 || dy == win->height-1) {
                video[pos] = ' ';
                video[pos+1] = WIN_BORDER_COLOR;
            }
        }
    }
    for (int i = 0; i < strlen(win->title) && i < win->width-2; i++) {
        int pos = (win->y) * 160 + (win->x + 1 + i) * 2;
        video[pos] = win->title[i];
        video[pos+1] = WIN_TITLE_COLOR;
    }
}

void ewms_handle_mouse(uint8_t mouse_x, uint8_t mouse_y, uint8_t buttons) {
    // TODO: Détecter clics sur les fenêtres
}

Window* ewms_get_active_window() {
    return (active_window >= 0) ? &windows[active_window] : 0;
}