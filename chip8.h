#ifndef CHIP8_H
#define CHIP8_H

#include <array>
#include <bitset>
#include <chrono>
#include <fstream>
#include <iostream>
#include <stack>
#include <thread>
#include <time.h>
#include <vector>

#include <SDL2/SDL.h>

#include "macros.h"

class Chip8 {
  private:
    std::array<uint8_t, 4096> memory;
    std::array<uint8_t, 16> registers;
    std::array<uint8_t, COLS*ROWS> display;
    std::array<uint16_t, 16> stack;
    std::array<uint16_t, 8> keys;

    std::uint16_t program_counter;
    std::uint16_t index_register;
    std::uint8_t stack_pointer;
    std::uint8_t delay_timer;
    std::uint8_t sound_timer;

    const std::array<uint16_t, 16 * 5> font = {
      0xF0, 0x90, 0x90, 0x90, 0xF0, // 0
      0x20, 0x60, 0x20, 0x20, 0x70, // 1
      0xF0, 0x10, 0xF0, 0x80, 0xF0, // 2
      0xF0, 0x10, 0xF0, 0x10, 0xF0, // 3
      0x90, 0x90, 0xF0, 0x10, 0x10, // 4
      0xF0, 0x80, 0xF0, 0x10, 0xF0, // 5
      0xF0, 0x80, 0xF0, 0x90, 0xF0, // 6
      0xF0, 0x10, 0x20, 0x40, 0x40, // 7
      0xF0, 0x90, 0xF0, 0x90, 0xF0, // 8
      0xF0, 0x90, 0xF0, 0x10, 0xF0, // 9
      0xF0, 0x90, 0xF0, 0x90, 0x90, // A
      0xE0, 0x90, 0xE0, 0x90, 0xE0, // B
      0xF0, 0x80, 0x80, 0x80, 0xF0, // C
      0xE0, 0x90, 0x90, 0x90, 0xE0, // D
      0xF0, 0x80, 0xF0, 0x80, 0xF0, // E
      0xF0, 0x80, 0xF0, 0x80, 0x80  // F
    };

    SDL_Window* window;
    SDL_Texture* texture;
    SDL_Renderer* renderer;

    uint16_t fetch_instruction();
    void draw();
    void initialise_values();
    void execute_instruction(uint16_t instruction);

  public:
    Chip8(std::string rom_path, SDL_Window* window);

    void run();
    void cycle();
    void load_rom(std::string rom_path);
};

#endif

