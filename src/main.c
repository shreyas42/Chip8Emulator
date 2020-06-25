#include <stdio.h>
#include "SDL2/SDL.h"

int main(int argc, char **argv)
{
    /* initialize SDL */
    SDL_Init(SDL_INIT_EVERYTHING);
    
    /* create an SDL window */
    SDL_Window *window = SDL_CreateWindow(
        "Chip8 Window", /* window name */
        SDL_WINDOWPOS_UNDEFINED, /* starting x-cordinate */
        SDL_WINDOWPOS_UNDEFINED, /* starting y-coordinate */
        640, 320, SDL_WINDOW_SHOWN /* dimensions and window flags */
    );

    /* creating a renderer */
    SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, SDL_TEXTUREACCESS_TARGET);
    
    while(1) {
        SDL_Event event;
        /* Poll for events and store it in the event variable */
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                /* A goto?? Nani?? */
                goto out;
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