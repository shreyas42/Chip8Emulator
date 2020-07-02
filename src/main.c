#include <stdio.h>
#include <math.h>
#include <stdbool.h>
#include <Windows.h>
#include "SDL2/SDL.h"
#include "chip8.h"
#include "chip8keyboard.h"
#include "chip8display.h"

const char keymap[CHIP8_MAX_KEYS] = {
    SDLK_0, SDLK_1, SDLK_2, SDLK_3,
    SDLK_4, SDLK_5, SDLK_6, SDLK_7,
    SDLK_8, SDLK_9, SDLK_a, SDLK_b,
    SDLK_c, SDLK_d, SDLK_e, SDLK_f,
};

int main(int argc, char **argv)
{
    /* Setting up the file to load the program from */
    if (argc < 2) {
        fprintf(stderr, "Correct usage: %s <File to load>\n", argv[0]);
        return -1;
    }

    const char *filename = argv[1];
    printf("Loading rom %s\n", filename);
    FILE *fp = fopen(filename, "rb");
    if (!fp) {
        fprintf(stderr, "Failed to open file!\n");
        return -1;
    }

    fseek(fp, 0, SEEK_END);
    long size = ftell(fp); /* get the size of the file, by returning current file position */
    fseek(fp, 0, SEEK_SET); /* reset the file offset */

    char buf[size];
    int res = fread(buf, size, 1, fp);
    if (res != 1) {
        printf("Failed to read file!\n");
        fclose(fp);
        return -1;
    }

    /* initialize the Chip8 interpreter */
    struct chip8 chip8;
    chip8_init(&chip8);
    chip8_load(&chip8, buf, (size_t)size);
    chip8_keyboard_set_keymap(&chip8.keyboard, keymap);

    /* initialize SDL */
    SDL_Init(SDL_INIT_EVERYTHING);
    /* create an SDL window */
    SDL_Window *window = SDL_CreateWindow(
        EMULATOR_WINDOW_TITLE, /* window name */
        SDL_WINDOWPOS_UNDEFINED, /* starting x-cordinate */
        SDL_WINDOWPOS_UNDEFINED, /* starting y-coordinate */
        EMULATOR_WINDOW_WIDTH, EMULATOR_WINDOW_HEIGHT, SDL_WINDOW_SHOWN /* dimensions and window flags */
    );

    /* creating a renderer */
    SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, SDL_TEXTUREACCESS_TARGET);
    
    while(1) {
        SDL_Event event;
        /* Poll for events and store it in the event variable */
        while (SDL_PollEvent(&event)) {
            switch (event.type) {
                case SDL_QUIT:
                    goto out;
                case SDL_KEYDOWN: {
                    char key = event.key.keysym.sym;
                    int vkey = chip8_keyboard_map(&chip8.keyboard, key);
                    if (vkey != -1)
                        chip8_keypress_down(&chip8.keyboard, vkey);
                    break;
                }
                case SDL_KEYUP: {
                    char key = event.key.keysym.sym;
                    int vkey = chip8_keyboard_map(&chip8.keyboard, key);
                    if (vkey != -1)
                        chip8_keypress_up(&chip8.keyboard, vkey);
                    break;
                }
            }
        }
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0); /* setting the color to black */
        SDL_RenderClear(renderer); /* paint over the screen with black */
        SDL_SetRenderDrawColor(renderer, 255, 255, 255, 0); /* setting draw color to white */

        for (int x = 0 ; x < CHIP8_WIDTH ; x++) {
            for (int y = 0 ; y < CHIP8_HEIGHT ; y++) {
                if (chip8_pixel_is_set(&chip8.display, x, y)) {
                    SDL_Rect r;
                    /* scale up the pixel by using rect and window multiplier */
                    r.x = x * CHIP8_WINDOW_MULTIPLIER;
                    r.y = y * CHIP8_WINDOW_MULTIPLIER;
                    r.w = CHIP8_WINDOW_MULTIPLIER;
                    r.h = CHIP8_WINDOW_MULTIPLIER;
                    /* draw the rectangle to the display */
                    /* SDL_RenderDrawRect - hollow rectangle 
                    * SDL_RenderFillRect - solid rectangle
                    */
                    SDL_RenderFillRect(renderer, &r);

                }
            }
        }
        /* throttle the frame-rate */
        SDL_Delay(1);
        /* present the changes */
        SDL_RenderPresent(renderer);
        /* simulating the delay timer - 
         * decrement the DT by 1 every 100ms
         */
        if (chip8.registers.DT > 0) {
            Sleep(1);
            chip8.registers.DT -= 1;
        }

        /* simluating the sound timer */
        if (chip8.registers.ST > 0) {
            Beep(9000, 10 * chip8.registers.ST);
            chip8.registers.ST = 0;
        }

        unsigned short opcode = chip8_memory_get_short(&chip8.memory, chip8.registers.PC);
        chip8.registers.PC += CHIP8_INSTRUCTION_WIDTH; /* move to next instruction, instructions are 2 bytes wide */
        chip8_exec(&chip8, opcode);
    }

out:
    /* destroy window */
    SDL_DestroyWindow(window);
    fclose(fp);
    return 0;
}