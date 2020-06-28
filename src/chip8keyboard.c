#include "chip8keyboard.h"
#include <assert.h>

static void chip8_keyboard_check_bounds(int key)
{
    assert(key >= 0 && key < CHIP8_MAX_KEYS);
}

int chip8_keyboard_map(const char *map, char key)
{
    for (int i = 0 ; i < CHIP8_MAX_KEYS ; i++) {
        if (map[i] == key)
            return i;
    }
    return -1;
}

void chip8_keypress_down(struct chip8_keyboard *keyboard, int key)
{
    chip8_keyboard_check_bounds(key);
    keyboard->keyboard[key] = true;
}

void chip8_keypress_up(struct chip8_keyboard *keyboard, int key)
{
    chip8_keyboard_check_bounds(key);
    keyboard->keyboard[key] = false;
}

bool chip8_is_key_down(struct chip8_keyboard *keyboard, int key)
{
    chip8_keyboard_check_bounds(key);
    return keyboard->keyboard[key];
}