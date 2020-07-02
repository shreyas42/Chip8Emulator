#ifndef CHIP8DISPLAY_H_
#define CHIP8DISPLAY_H_

#include <stdbool.h>
#include "config.h"

struct chip8_display {
    bool pixels[CHIP8_HEIGHT][CHIP8_WIDTH]; /* bool works for monochrome displays */
};

void chip8_pixel_set(struct chip8_display *display, int x, int y);
void chip8_display_clear(struct chip8_display *display);
bool chip8_pixel_is_set(struct chip8_display *display, int x, int y);
bool chip8_display_draw_sprite(struct chip8_display *display, int x, int y, const unsigned char *sprite, int num);

#endif