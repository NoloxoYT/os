#include "../kernel/stdint.h"
#include "../kernel/io.h"
#include "../kernel/wait.h"
extern int strcmp(const char* s1, const char* s2);
extern void kprint(const char* str);
extern volatile char* video;
extern int cursor_pos;
extern uint8_t keyboard_read();
extern uint8_t default_color;
extern char input_buffer[81];
extern int input_idx;
int is_on_esc = 0; // Indique si on est en train de taper une commande (après avoir appuyé sur Esc)
extern void __main(); // Pour revenir à la boucle principale du shell après nvim
void comand() {
    if (strcmp(input_buffer, "quit") == 0) {
        // Quitter nvim et revenir au shell
        for (int i = 0; i < 80 * 25 * 2; i += 2) { video[i] = ' '; video[i+1] = 0x07; }
        cursor_pos = 320;
        __main(); // Revenir à la boucle principale du shell
    }
     else {
        for (int i = 160*24; i < 80*25;) { video[i] = ' '; video[i+1] = 0x07; }
        cursor_pos = 160*24;
        kprint("Commande inconnue dans shitvim.\n");
        wait(2000);
        for (int i = 160*24; i < 80*25;) { video[i] = ' '; video[i+1] = 0x07; }
        cursor_pos = 160*24;
        kprint(": ");
    }
}
void nvim(void) {
for (int i = 0; i < 80 * 25 * 2; i += 2) { video[i] = ' '; video[i+1] = 0x07; }
cursor_pos = 160;
    while (1) {
         video[cursor_pos] = '_';
         video[cursor_pos+1] = 0x0F;
        uint8_t c = keyboard_read();
        if (c != 0) {
            video[cursor_pos] = ' '; // Effacer curseur
            if (c == '\b') {
                    cursor_pos -= 2;       // Reculer sur l'écran
                    video[cursor_pos] = ' '; // Effacer à l'écran
            }
            else if (c == '\n') {
                if (is_on_esc) {
                    comand();
                    is_on_esc = 0;
                }
                cursor_pos = ((cursor_pos / 160) + 1) * 160; // Saut de ligne écran
            }
            else if (c == 27) {
                cursor_pos = 160*24; // 24 = derniere ligne VGA
                kprint(": ");
                is_on_esc = 1;
                if (is_on_esc) {
                    is_on_esc = 0;
                }
            }
            else {
                    if (input_idx < 79) {
                    input_buffer[input_idx++] = (char)c; // On remplit le buffer
                    video[cursor_pos] = (char)c;         // On affiche
                    video[cursor_pos + 1] = 0x0E;
                    cursor_pos += 2;
                }
            }
        }
    }
}