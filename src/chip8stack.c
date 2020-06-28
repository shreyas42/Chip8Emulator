#include "chip8stack.h"
#include "chip8.h"
#include <assert.h>

static void chip8_is_stack_in_bounds(struct chip8 *chip8)
{
    /* we can avoid the zero check since -1 will overflow */
    assert(chip8->registers.SP < CHIP8_MAX_STACK_DEPTH);
}

void chip8_stack_push(struct chip8 *chip8, unsigned short val)
{
    chip8->registers.SP += 1;
    chip8_is_stack_in_bounds(chip8);
    chip8->stack.stack[chip8->registers.SP] = val;
}

unsigned short chip8_stack_pop(struct chip8 *chip8)
{
    unsigned short result;
    chip8_is_stack_in_bounds(chip8);
    result = chip8->stack.stack[chip8->registers.SP];
    chip8->registers.SP -= 1;
    return result;
}