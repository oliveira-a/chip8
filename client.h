#ifndef CLIENT_H
#define CLIENT_H

#include <SDL2/SDL.h>

#include "chip8.h"

class Client { 
  private:
    SDL_Window* window;
    SDL_Surface* surface;
    Chip8* chip8;

    void create_window();

  public:
    Client(Chip8* chip8);

    void start();
};

#endif
