#ifndef CHIP8_KEYBOARD_H_
#define CHIP8_KEYBOARD_H_

#include <stdbool.h>
#include "config.h"

struct chip8_keyboard {
    bool keyboard[CHIP8_MAX_KEYS];
};

int chip8_keyboard_map(const char *map, char key);
void chip8_keypress_down(struct chip8_keyboard *keyboard, int key);
void chip8_keypress_up(struct chip8_keyboard *keyboard, int key);
bool chip8_key_is_down(struct chip8_keyboard *keyboard, int key);

#endif