#include "io.h"

#define PS2_DATA_PORT 0x60
#define PS2_STATUS_PORT 0x64
#define PS2_COMMAND_PORT 0x64

// Basic scan code to ASCII (US layout, simplified)
static const char kbd_map[] = {
    0, 27, '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '-', '=', '\b',
    '\t', 'q', 'w', 'e', 'r', 't', 'y', 'u', 'i', 'o', 'p', '[', ']', '\n',
    0, 'a', 's', 'd', 'f', 'g', 'h', 'j', 'k', 'l', ';', '\'', '`', 0, '\\',
    'z', 'x', 'c', 'v', 'b', 'n', 'm', ',', '.', '/', 0, '*', 0, ' '
};

uint8_t keyboard_read() { // Changé char en uint8_t
    if (inb(0x64) & 0x01) {
        uint8_t scancode = inb(0x60);
        if (scancode < 58) { // Sécurité index
            return kbd_map[scancode];
        }
    }
    return 0;
}   
void ps2_mouse_wait(uint8_t type) {
    uint32_t timeout = 100000;
    if (type == 0) { // Data
        while (timeout--) {
            if (inb(PS2_STATUS_PORT) & 1) return;
        }
    } else { // Signal
        while (timeout--) {
            if (!(inb(PS2_STATUS_PORT) & 2)) return;
        }
    }
}

void ps2_mouse_write(uint8_t data) {
    ps2_mouse_wait(1);
    outb(PS2_COMMAND_PORT, 0xD4);
    ps2_mouse_wait(1);
    outb(PS2_DATA_PORT, data);
}

void mouse_init() {
    uint8_t status;
    ps2_mouse_wait(1);
    outb(PS2_COMMAND_PORT, 0xA8); // Enable auxiliary device
    ps2_mouse_wait(1);
    outb(PS2_COMMAND_PORT, 0x20); // Get command byte
    ps2_mouse_wait(0);
    status = (inb(PS2_DATA_PORT) | 2);
    ps2_mouse_wait(1);
    outb(PS2_COMMAND_PORT, 0x60); // Set command byte
    ps2_mouse_wait(1);
    outb(PS2_DATA_PORT, status);
    ps2_mouse_write(0xF4); // Enable packet streaming
}

// Minimal Framebuffer Driver (Generic VGA/VESA)
// This is used for iGPU/GPU output in a generic way
void fb_put_pixel(uint32_t x, uint32_t y, uint32_t color, uint32_t width, uint32_t* fb) {
    fb[y * width + x] = color;
}
