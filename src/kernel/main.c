#include "stdint.h"
#include "io.h"
#include "ewms.h"

extern uint8_t keyboard_read();
volatile char* video = (volatile char*)0xB8000;
extern void nvim();

int cursor_pos = 320;
uint8_t default_color = 0x0F;
char input_buffer[81];
int input_idx = 0;

void kprint(const char* str) {
    for (int i = 0; str[i] != '\0'; i++) {
        if (str[i] == '\n') {
            cursor_pos = ((cursor_pos / 160) + 1) * 160;
        } else {
            video[cursor_pos] = str[i];
            video[cursor_pos + 1] = default_color;
            cursor_pos += 2;
        }
    }
}

void disable_hardware_cursor() {
    outb(0x3D4, 0x0A); outb(0x3D5, 0x20);
}

int strcmp(const char* s1, const char* s2) {
    while (*s1 && (*s1 == *s2)) { s1++; s2++; }
    return *(unsigned char*)s1 - *(unsigned char*)s2;
}

void interpreter_commande() {
    input_buffer[input_idx] = '\0';
    if (strcmp(input_buffer, "/help") == 0) {
        kprint("Commandes: /help, cls, /info, /vim, /win\n");
    }
    else if (strcmp(input_buffer, "/vim") == 0) {
        nvim();
    }
    else if (strcmp(input_buffer, "cls") == 0) {
        for (int i = 0; i < 80 * 25 * 2; i += 2) { video[i] = ' '; video[i+1] = 0x07; }
        cursor_pos = 0;
    }
    else if (strcmp(input_buffer, "/info") == 0) {
        kprint("OS: Noloxo OS v0.1\nAuthor: Noloxo\n");
    }
    else if (strcmp(input_buffer, "/win") == 0) {
        ewms_create_window(10, 5, 40, 10, "Terminal");
        ewms_create_window(55, 5, 30, 8, "Info");
    }
    else if (input_idx > 0) {
        kprint("Commande inconnue.\n");
    }
    input_idx = 0;
}

__attribute__((section(".text.entry")))
void __main(void) {
    __asm__ volatile("cli");
    disable_hardware_cursor();
    for (int i = 0; i < 80 * 25 * 2; i += 2) { video[i] = ' '; video[i+1] = 0x07; }
    cursor_pos = 160;
    default_color = 0x0F;
    kprint("Noloxo OS : Tape une commande (HELP, CLS, WIN...)\n");
    ewms_init();

    default_color = 0x0E;
    cursor_pos = 160*4;
    kprint("> ");

    while (1) {
        video[cursor_pos] = '_';
        video[cursor_pos+1] = 0x0F;
        uint8_t c = keyboard_read();
        if (c != 0) {
            video[cursor_pos] = ' ';
            if (c == '\b') {
                if (input_idx > 0) {
                    input_idx--;
                    cursor_pos -= 2;
                    video[cursor_pos] = ' ';
                }
            }
            else if (c == '\n') {
                cursor_pos = ((cursor_pos / 160) + 1) * 160;
                interpreter_commande();
                kprint("> ");
            }
            else if (input_idx < 79) {
                input_buffer[input_idx++] = (char)c;
                video[cursor_pos] = (char)c;
                video[cursor_pos + 1] = 0x0E;
                cursor_pos += 2;
            }
        }
    }
}