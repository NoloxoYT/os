#include "stdint.h"
#include "io.h"

extern uint8_t keyboard_read();
volatile char* video = (volatile char*)0xB8000;
extern void nvim();
// Variables pour l'affichage
int cursor_pos = 320;
uint8_t default_color = 0x0F;

// --- NOUVEAU : Variables pour le Shell ---
char input_buffer[81]; // Stocke ce que l'utilisateur écrit (max 80 chars)
int input_idx = 0;     // Position dans le buffer

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
    outb(0x3D4, 0x0A); // On demande le registre "Cursor Start"
    outb(0x3D5, 0x20); // On envoie 0x20 pour désactiver le curseur
}

// Fonction pour comparer deux chaînes de texte (très important !)
int strcmp(const char* s1, const char* s2) {
    while (*s1 && (*s1 == *s2)) {
        s1++; s2++;
    }
    return *(unsigned char*)s1 - *(unsigned char*)s2;
}

// Fonction qui analyse ce que tu as tapé après avoir appuyé sur Entrée
void interpreter_commande() {
    input_buffer[input_idx] = '\0'; // On termine la chaîne proprement

    if (strcmp(input_buffer, "/help") == 0) {
        kprint("Commandes: /help, cls, /info, /vim\n");
    } 
    else if ( strcmp(input_buffer, "/vim") == 0) {
        nvim();
    }
    else if (strcmp(input_buffer, "cls") == 0) {
        // Effacer l'écran
        // for (int i = 0; i < 80 * 25 * 2; i += 2) { video[i] = ' '; }
        for (int i = 0; i < 80 * 25 * 2; i += 2) { video[i] = ' '; video[i+1] = 0x07; }
        cursor_pos = 0;
    }
    else if (strcmp(input_buffer, "/info") == 0) {
        kprint("OS: Noloxo OS v0.1\nAuthor: Noloxo\n");
    }
    else if (input_idx > 0) {
        kprint("Commande inconnue.\n");
    }

    // On vide le buffer pour la suite
    input_idx = 0;
}

void __main() {}

__attribute__((section(".text.entry")))
void main(void) {
    __asm__ volatile("cli");
    disable_hardware_cursor();
    // Clear screen
    for (int i = 0; i < 80 * 25 * 2; i += 2) { video[i] = ' '; video[i+1] = 0x07; }
    
    cursor_pos = 160;
    default_color = 0x0F;
    kprint("Noloxo OS : Tape une commande (HELP, CLS...)\n");
    
    default_color = 0x0E; 
    cursor_pos = 160*4;
    kprint("> ");
    
    while (1) {
        // mouse_init();
        // Afficher curseur
         video[cursor_pos] = '_';
         video[cursor_pos+1] = 0x0F;
        uint8_t c = keyboard_read();
        if (c != 0) {
            video[cursor_pos] = ' '; // Effacer curseur
            if (c == '\b') {
                if (input_idx > 0) {
                    input_idx--;           // Reculer dans le buffer
                    cursor_pos -= 2;       // Reculer sur l'écran
                    video[cursor_pos] = ' '; // Effacer à l'écran
                }
            } 
            else if (c == '\n') {
                cursor_pos = ((cursor_pos / 160) + 1) * 160; // Saut de ligne écran
                interpreter_commande();                      // Analyser le buffer !
                kprint("> ");                                // Afficher un prompt
            } 
            else {
                // Si le buffer n'est pas plein, on stocke la lettre
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