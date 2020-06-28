#ifndef CHIP8_H_
#define CHIP8_H_

#include "config.h"
#include "chip8memory.h"
#include "chip8registers.h"
#include "chip8stack.h"
#include "chip8keyboard.h"

/* struct representing the internals of the chip8 interpreter */
struct chip8 {
    struct chip8_memory memory;
    struct chip8_stack stack;
    struct chip8_registers registers;
    struct chip8_keyboard keyboard;
};

#endif