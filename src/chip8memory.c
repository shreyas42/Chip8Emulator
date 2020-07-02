#include <assert.h>
#include "chip8memory.h"

static void chip8_is_memory_in_bounds(int index)
{
    assert(index >=0 && index < CHIP8_MEMORY_SIZE);
}

void chip8_memory_set(struct chip8_memory *memory, int index, unsigned char val)
{
    chip8_is_memory_in_bounds(index);
    memory->memory[index] = val;
}

unsigned char chip8_memory_get(struct chip8_memory *memory, int index)
{
    chip8_is_memory_in_bounds(index);
    return memory->memory[index];
}

unsigned short chip8_memory_get_short(struct chip8_memory *memory, int index)
{
    /* memory is stored big endian */
    unsigned char hi_byte = chip8_memory_get(memory, index);
    unsigned char lo_byte = chip8_memory_get(memory, index + 1);
    return ((hi_byte << 8) | lo_byte);
}