#include <SDL2/SDL.h> 

#include "chip8.h"
#include "macros.h"

int main(int argc, char** args) {
  SDL_Window* window;

  if (SDL_Init(SDL_INIT_VIDEO) < 0) {
    LOG_ERR("SDL could not initiliase...");
    exit(1);
  } else {
    window = SDL_CreateWindow(
        NAME,
        SDL_WINDOWPOS_UNDEFINED,
        SDL_WINDOWPOS_UNDEFINED,
        COLS*SCALE,
        ROWS*SCALE,
        SDL_WINDOW_SHOWN
        );

    if (window == NULL) {
      LOG_ERR("Could not create SDL window...");
      exit(1);
    }
  }

  Chip8 chip8("pong.rom", window);

  chip8.run();

  SDL_Quit();

  return 0;
}

