#include <assert.h>
#include <memory.h>
#include <stdbool.h>
#include <stdlib.h>
#include <time.h>
#include "chip8.h"
#include "SDL2/SDL.h"

/* default character sprites used by chip8 programs */
/* loaded at address 0x00 */
const unsigned char chip8_default_charset[] = {
    0xf0, 0x90, 0x90, 0x90, 0xf0, /* character '0' */
    0x20, 0x60, 0x20, 0x20, 0x70, /* character '1' */
    0xf0, 0x10, 0xf0, 0x80, 0xf0, /* character '2' */
    0xf0, 0x10, 0xf0, 0x10, 0xf0, /* character '3' */
    0x90, 0x90, 0xf0, 0x10, 0x10, /* character '4' */
    0xf0, 0x80, 0xf0, 0x10, 0xf0, /* character '5' */
    0xf0, 0x80, 0xf0, 0x90, 0xf0, /* character '6' */
    0xf0, 0x10, 0x20, 0x40, 0x40, /* character '7' */
    0xf0, 0x90, 0xf0, 0x90, 0xf0, /* character '8' */
    0xf0, 0x90, 0xf0, 0x10, 0xf0, /* character '9' */
    0xf0, 0x90, 0xf0, 0x90, 0x90, /* character 'A' */
    0xe0, 0x90, 0xe0, 0x90, 0xe0, /* character 'B' */
    0xf0, 0x80, 0x80, 0x80, 0xf0, /* character 'C' */
    0xe0, 0x90, 0x90, 0x90, 0xe0, /* character 'D' */
    0xf0, 0x80, 0xf0, 0x80, 0xf0, /* character 'E' */
    0xf0, 0x80, 0xf0, 0x80, 0x80 /* character 'F' */
};

void chip8_init(struct chip8 *chip8)
{
    memset(chip8, 0, sizeof(struct chip8));
    memcpy(chip8->memory.memory + CHIP8_DEFAULT_CHARSET_LOAD_ADDR, chip8_default_charset, sizeof(chip8_default_charset));
}

void chip8_load(struct chip8 *chip8, const unsigned char *buf, size_t size)
{
    assert(CHIP8_PROGRAM_LOAD_ADDR + size < CHIP8_MEMORY_SIZE);
    memcpy(chip8->memory.memory + CHIP8_PROGRAM_LOAD_ADDR, buf, size); /* this will work since the memory is of type unsigned char */
    chip8->registers.PC = CHIP8_PROGRAM_LOAD_ADDR;
}

/* handle opcodes of the form 0x8xyn */
static void chip8_exec_extended_reg_ops(struct chip8 *chip8, unsigned char x, unsigned char y, unsigned char operation)
{
    unsigned short tmp = 0;
    switch(operation) {
        /* LD Vx, Vy - 8xy0: Stores the value of Vy in Vx */
        case 0x00:
            chip8->registers.V[x] = chip8->registers.V[y];
            break;
        
        /* OR Vx, Vy - 8xy1: Perform Vx OR Vy and store the result in Vx */
        case 0x01:
            chip8->registers.V[x] |= chip8->registers.V[y];
            break;

        /* AND Vx, Vy - 8xy2: Perform Vx AND Vy and store the result in Vx */
        case 0x02:
            chip8->registers.V[x] &= chip8->registers.V[y];
            break;
        
        /* XOR Vx, Vy - 8xy3: Perform Vx XOR Vy and store the result in Vx */
        case 0x03:
            chip8->registers.V[x] ^= chip8->registers.V[y];
            break;
        
        /* ADD Vx, Vy - 8xy4: Perform Vx + Vy, store least signficant 8 bits in Vx
         * Set VF to carry if overflow
         */
        case 0x04:
            tmp = chip8->registers.V[x] + chip8->registers.V[y];
            chip8->registers.V[x] = tmp;
            chip8->registers.V[0x0f] = (tmp >> 8) & 0x0001;
            break;

        /* SUB Vx, Vy - 8xy5: Vx = Vx - Vy, set Vf to NOT borrow */
        case 0x05:
            chip8->registers.V[0x0f] = chip8->registers.V[x] > chip8->registers.V[y];
            chip8->registers.V[x] = chip8->registers.V[x] - chip8->registers.V[y];
            break;
        
        /* SHR Vx {, Vy} - 8xy6: Set Vx = Vx SHR 1, set VF is lsb of Vx is 1 */
        case 0x06:
            chip8->registers.V[0x0f] = chip8->registers.V[x] & 0x01;
            chip8->registers.V[x] >>= 1;
            break;
        
        /* SUBN Vx, Vy - 8xy7: Set Vx = Vy - Vx, set Vf to NOT borrow */
        case 0x07:
            chip8->registers.V[0x0f] = chip8->registers.V[y] > chip8->registers.V[x];
            chip8->registers.V[x] = chip8->registers.V[y] - chip8->registers.V[x];
            break;

        /* SHL Vx {, Vy} - 8xyE: Vx = Vx SHL 1, if msb of Vx is 1, set Vf to 1 */
        case 0x0E:
            chip8->registers.V[0x0f] = chip8->registers.V[x] & 0x80;
            chip8->registers.V[x] <<= 1;
            break;
    }
}

static char chip8_wait_for_key_press(struct chip8 *chip8)
{
    SDL_Event event;
    while (SDL_WaitEvent(&event)) {
        if (event.type != SDL_KEYDOWN)
            continue;
        char c = event.key.keysym.sym;
        char chip8_key = chip8_keyboard_map(&chip8->keyboard, c);
        if (chip8_key != -1)
            return chip8_key;
    }
    return -1;
}
/* handle opcodes of the form 0xFxnn */
static void chip8_exec_extended_control(struct chip8 *chip8, unsigned short opcode, unsigned char x)
{
    switch(opcode & 0x00ff) {
        /* LD Vx, DT - Fx07: Set Vx = DT */
        case 0x07:
            chip8->registers.V[x] = chip8->registers.DT;
            break;
        
        /* LD Vx, K - Fx0A: Wait for a keypress, store keypress value in Vx */
        case 0x0A:
            chip8->registers.V[x] = chip8_wait_for_key_press(chip8);
            break;
        
        /* LD DT, Vx - Fx15: Set DT to value in Vx */
        case 0x15:
            chip8->registers.DT = chip8->registers.V[x];
            break;
        
        /* LD ST, Vx - Fx18: Set sound timer = Vx */
        case 0x18:
            chip8->registers.ST = chip8->registers.V[x];
            break;
        
        /* ADD I, Vx - Fx1E: Set I = I + Vx */
        case 0x1E:
            chip8->registers.I += chip8->registers.V[x];
            break;

        /* LD F, Vx - Fx29: Set I = location of sprite for digit Vx */
        case 0x29:
            /* default sprites are at offset of 0 */
            chip8->registers.I = chip8->registers.V[x] * CHIP8_DEFAULT_CHARSET_BYTES;
            break;
        
        /* LD B, Vx - Fx33: Store BCD representation of Vx in I, I+1, I+2 */
        case 0x33: {
            unsigned char hundreds = chip8->registers.V[x] / 100;
            unsigned char tens = chip8->registers.V[x] / 10 % 10;
            unsigned char units = chip8->registers.V[x] % 10;
            chip8_memory_set(&chip8->memory, chip8->registers.I, hundreds);
            chip8_memory_set(&chip8->memory, chip8->registers.I + 1, tens);
            chip8_memory_set(&chip8->memory, chip8->registers.I + 2, units);
            break;
        }

        /* LD [I], Vx - Fx55: Store registers V0 to Vx in memory starting at I */
        case 0x55: {
            for (int i = 0 ; i <= x ; i++)
                chip8_memory_set(&chip8->memory, chip8->registers.I + i, chip8->registers.V[i]);
            break;
        }

        /* LD Vx, [i] - Fx65: Load registers V0 to Vx starting from memory location I */
        case 0x65: {
            for (int i = 0 ; i <= x ; i++)
                chip8->registers.V[i] = chip8_memory_get(&chip8->memory, chip8->registers.I + i);
            break;
        }
    }
}

/* execute opcodes that can't be handled via simple switch cases */
static void chip8_exec_extended(struct chip8 *chip8, unsigned short opcode)
{
    /* always use bitwise operators here instead of reading bytes from memory and such */
    /* bitwise operators will abstract away endianness differences */

    bool conditional = false;
    unsigned short nnn = opcode & 0x0fff;
    unsigned char x = (opcode & 0x0f00) >> 8;
    unsigned char y = (opcode & 0x00f0) >> 4;
    unsigned char kk = opcode & 0x00ff;
    unsigned char last_nibble = opcode & 0x000f;
    /* switch-case on the opcode value, the most significant 4 bits */
    switch(opcode & 0xf000) {

        /* JP addr - 1nnn: jump to location nnn */
        case 0x1000:
            /* set the PC */
            chip8->registers.PC = nnn;
            break;

        /* CALL addr - 2nnn: call subroutine at nnn */
        case 0x2000:
            /* save the pc onto the stack */
            chip8_stack_push(chip8, chip8->registers.PC);
            /* set the new PC */
            chip8->registers.PC = nnn;
            break;

        /* SE Vx, byte - 3xkk: Skip next instruction if Vx == kk */
        case 0x3000:
            if (chip8->registers.V[x] == kk)
                /* skip the current PC */
                /* PC is always incremented before chip8_exec is called */
                /* so one addition will skip the next intended PC */
                chip8->registers.PC += CHIP8_INSTRUCTION_WIDTH;
            break;

        /* SNE Vx, byte - 4xkk: Skip next instruction if Vx != kk */
        case 0x4000:
            if (chip8->registers.V[x] != kk)
                /* same as SE instruction */
                chip8->registers.PC += CHIP8_INSTRUCTION_WIDTH;
            break;

        /* SE Vx Vy - 5xy0: Skip next instruction if Vx == Vy */
        case 0x5000:
            if (chip8->registers.V[x] == chip8->registers.V[y])
                chip8->registers.PC += CHIP8_INSTRUCTION_WIDTH;
            break;

        /* LD Vx, byte - 6xkk: Load the value kk into register Vx */
        case 0x6000:
            chip8->registers.V[x] = kk;
            break;

        /* ADD Vx, byte - 7xkk: Adds the value kk to Vx and stores the result in Vx */
        case 0x7000:
            chip8->registers.V[x] += kk;
            break;

        /* handle register-register arithmetic operations */
        case 0x8000:
            chip8_exec_extended_reg_ops(chip8, x, y, last_nibble);
            break;
        
        /* SNE Vx, Vy - 9xy0: Skip next instruction if Vx != Vy */
        case 0x9000:
            conditional = chip8->registers.V[x] != chip8->registers.V[y];
            chip8->registers.PC += conditional * CHIP8_INSTRUCTION_WIDTH;
            break;

        /* LD I, addr - Annn: Set I = nnn */
        case 0xA000:
            chip8->registers.I = nnn;
            break;
        
        /* JP V0, addr - Bnnn: Jump to location nnn + V0 */
        case 0xB000:
            chip8->registers.PC = chip8->registers.V[0x00] + nnn;
            break;

        /* RND Vx, byte - Cxkk: Generate a random 8-bit unsigned number and AND with kk, store result in Vx */
        case 0xC000:
            srand(clock());
            chip8->registers.V[x] = (rand() & 255) & kk;
            break;
        
        /* DRW Vx, Vy, nibble - Dxyn: Draw a sprite, memory address in I register, at co-ords (Vx, Vy), sprite is n bytes */
        case 0xD000: {
            const unsigned char *sprite = (const unsigned char *)(chip8->memory.memory + chip8->registers.I);
            chip8->registers.V[0x0f] = chip8_display_draw_sprite(&chip8->display, chip8->registers.V[x], chip8->registers.V[y], sprite, last_nibble);
            break;
        }

        /* skip instruction based on key input */
        case 0xE000: {
            switch(opcode & 0x00ff) {

                /* SKP Vx - Ex9E: Skip next instruction if key with value in Vx is pressed */
                case 0x9E:
                    conditional = chip8_key_is_down(&chip8->keyboard, chip8->registers.V[x]);
                    chip8->registers.PC += conditional * CHIP8_INSTRUCTION_WIDTH;
                    break;
                
                /* SKNP Vx - ExA1: Skip next instruction if key with value in Vx is not pressed */
                case 0xA1:
                    conditional = !chip8_key_is_down(&chip8->keyboard, chip8->registers.V[x]);
                    chip8->registers.PC += conditional * CHIP8_INSTRUCTION_WIDTH;
                    break;
            }
        }

        /* control and processor state instructions */
        case 0xF000:
            chip8_exec_extended_control(chip8, opcode, x);
            break;
    }
}

/* refer to Cowgod's CHIP-8 reference for an explanation of the ISA */
void chip8_exec(struct chip8 *chip8, unsigned short opcode)
{
    /* this function implements the ISA by matching opcodes */
    switch(opcode) {
        /* CLS - clear the screen */
        case 0x00E0: {
            chip8_display_clear(&chip8->display);
            break;
        }
        /* RET - return from a sub-routine */
        case 0x00EE: {
            /* pop the last saved address and set PC*/
            chip8->registers.PC = chip8_stack_pop(chip8);
            break;
        }
        default: {
            chip8_exec_extended(chip8, opcode);
        }
    }
}