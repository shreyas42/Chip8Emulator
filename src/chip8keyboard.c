#include <assert.h>
#include "chip8keyboard.h"

static void chip8_keyboard_check_bounds(int key)
{
    assert(key >= 0 && key < CHIP8_MAX_KEYS);
}

void chip8_keyboard_set_keymap(struct chip8_keyboard *keyboard, const char *keymap)
{
    keyboard->keymap = keymap;
}

int chip8_keyboard_map(struct chip8_keyboard *keyboard, char key)
{
    for (int i = 0 ; i < CHIP8_MAX_KEYS ; i++) {
        if (keyboard->keymap[i] == key)
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

bool chip8_key_is_down(struct chip8_keyboard *keyboard, int key)
{
    chip8_keyboard_check_bounds(key);
    return keyboard->keyboard[key];
}