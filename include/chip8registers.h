#ifndef CHIP8REGISTERS_H_
#define CHIP8REGISTERS_H_

#include "config.h"

struct chip8_registers {
    unsigned char V[CHIP8_MAX_DATA_REGISTERS];
    unsigned short I;
    unsigned char DT; /* delay_timer */
    unsigned char ST; /* sound timer */
    unsigned short PC;
    unsigned char SP;
};

#endif