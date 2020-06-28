#include <stdio.h>
#include <stdbool.h>
#include "SDL2/SDL.h"
#include "chip8.h"
#include "chip8keyboard.h"

const char keymap[] = {
    SDLK_0, SDLK_1, SDLK_2, SDLK_3,
    SDLK_4, SDLK_5, SDLK_6, SDLK_7,
    SDLK_8, SDLK_9, SDLK_a, SDLK_b,
    SDLK_c, SDLK_d, SDLK_e, SDLK_f,
};
int main(int argc, char **argv)
{
    struct chip8 chip8;
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
                    int vkey = chip8_keyboard_map(keymap, key);
                    if (vkey != -1)
                        chip8_keypress_down(&chip8.keyboard, vkey);
                    break;
                }
                case SDL_KEYUP: {
                    char key = event.key.keysym.sym;
                    int vkey = chip8_keyboard_map(keymap, key);
                    if (vkey != -1)
                        chip8_keypress_up(&chip8.keyboard, vkey);
                    break;
                }
            }
        }
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0); /* setting the color to black */
        SDL_RenderClear(renderer); /* paint over the screen with black */
        SDL_SetRenderDrawColor(renderer, 255, 255, 255, 0);
        /* creating a rectangle that at pos (0,0) that is 4o
         * in height and width
         */
        SDL_Rect r;
        r.x = 0;
        r.y = 0;
        r.w = 40;
        r.h = 40;
        /* draw the rectangle to the display */
        /* SDL_RenderDrawRect - hollow rectangle 
         * SDL_RenderFillRect - solid rectangle
         */
        SDL_RenderFillRect(renderer, &r);
        /* present the changes */
        SDL_RenderPresent(renderer);
    }

out:
    /* destroy window */
    SDL_DestroyWindow(window);
    return 0;
}