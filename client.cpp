#include "macros.h"
#include "client.h"
#include "chip8.h"

Client::Client(Chip8* chip8) {
  this->chip8 = chip8;
}

void Client::start() {
  create_window();

  this->chip8->run();
}

void Client::create_window() {
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
}
