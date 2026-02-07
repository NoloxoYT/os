#include "../kernel/stdint.h"
#include "../kernel/io.h"
#include "../kernel/wait.h"
// On déclare ce qu'on utilise
extern void __main();
extern int strcmp(const char* s1, const char* s2);
extern void kprint(const char* str);
extern volatile char* video;
extern int cursor_pos;
extern uint8_t keyboard_read();
extern char input_buffer[81];
extern int input_idx;

// On utilise un int pour savoir si on est en mode commande (1) ou insertion (0)
int is_command_mode = 0; 
int saved_pos = 0; // Pour se souvenir d'où on était dans le texte

void comand() {
    input_buffer[input_idx] = '\0'; // Terminer la chaîne

    if (strcmp(input_buffer, "quit") == 0) {
        kprint("Quitter...\n");
        wait(1000);
        for (int i = 0; i < 80 * 25 * 2; i += 2) { video[i] = ' '; video[i+1] = 0x07; }
        cursor_pos = 160*4;
        __main();
    }
    else {
        // Nettoyer la ligne de commande (Ligne 23)
        for (int i = 160 * 23; i < 160 * 24; i += 2) { 
            video[i] = ' '; 
            video[i+1] = 0x07; 
        }
        cursor_pos = 160 * 23;
        kprint("Inconnu: ");
        kprint(input_buffer);
        wait(1500);
    }
}

void nvim(void) {
    // Nettoyer l'écran pour nvim
    for (int i = 0; i < 80 * 25 * 2; i += 2) { video[i] = ' '; video[i+1] = 0x07; }
    cursor_pos = 0;
    kprint("--- SHITVIM v0.1 ---\n");
    cursor_pos = 160; // On commence à la ligne 2

    while (1) {
        video[cursor_pos] = '_';
        video[cursor_pos+1] = 0x0F;

        uint8_t c = keyboard_read();
        if (c != 0) {
            video[cursor_pos] = ' '; // Effacer curseur

            if (c == 27) { // TOUCHE ESC
                is_command_mode = 1;
                saved_pos = cursor_pos; // On sauvegarde la position du texte
                cursor_pos = 160 * 23;  // On descend à la ligne de commande
                kprint(": ");
                input_idx = 0;          // On vide le buffer pour la commande
            }
            else if (c == '\n') {
                if (is_command_mode) {
                    comand();
                    if (strcmp(input_buffer, "quit") == 0) return; // Quitter si demandé
                    
                    // Retour au mode insertion
                    is_command_mode = 0;
                    cursor_pos = saved_pos; 
                } else {
                    cursor_pos = ((cursor_pos / 160) + 1) * 160;
                }
            }
            else if (c == '\b') {
                if (cursor_pos > 160) {
                    cursor_pos -= 2;
                    video[cursor_pos] = ' ';
                }
            }
            else {
                // Si on est en mode commande, on remplit le buffer
                if (is_command_mode) {
                    if (input_idx < 70) {
                        input_buffer[input_idx++] = (char)c;
                        video[cursor_pos] = (char)c;
                        video[cursor_pos+1] = 0x0E;
                        cursor_pos += 2;
                    }
                } else {
                    // Mode texte normal
                    video[cursor_pos] = (char)c;
                    video[cursor_pos+1] = 0x02; // Texte vert
                    cursor_pos += 2;
                }
            }
        }
    }
}