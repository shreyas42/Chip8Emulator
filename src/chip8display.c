#include <assert.h>
#include <memory.h>
#include "chip8display.h"

static void chip8_display_check_bounds(int x, int y)
{
    assert(x >= 0 && x < CHIP8_WIDTH && y >= 0 && y < CHIP8_HEIGHT);
}

void chip8_pixel_set(struct chip8_display *display, int x, int y)
{
    chip8_display_check_bounds(x, y);
    display->pixels[y][x] = true;
}

void chip8_display_clear(struct chip8_display *display)
{
    memset(display->pixels, 0, sizeof(display->pixels));
}

bool chip8_pixel_is_set(struct chip8_display *display, int x, int y)
{
    chip8_display_check_bounds(x, y);
    return display->pixels[y][x];
}

bool chip8_display_draw_sprite(struct chip8_display *display, int x, int y, const unsigned char *sprite, int num)
{
    bool pixel_collision = false;
    for (int ly = 0 ; ly < num ; ly++) {
        char c = sprite[ly];
        for (int lx = 0 ; lx < 8 ; lx++) {
            if ((c & (0b10000000 >> lx)) == 0)
                continue;
            pixel_collision |= display->pixels[(y + ly) % CHIP8_HEIGHT][(x + lx) % CHIP8_WIDTH];
            display->pixels[(y + ly) % CHIP8_HEIGHT][(x + lx) % CHIP8_WIDTH] ^= true;
        }
    }
    return pixel_collision;
}
