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
        COLS*10,
        ROWS*10,
        SDL_WINDOW_SHOWN
        );

    if (window == NULL) {
      LOG_ERR("Could not create SDL window...");
      exit(1);
    }
  }

  Chip8 chip8("test.rom");

  chip8.run(window);

  return 0;
}
