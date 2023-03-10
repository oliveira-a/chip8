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

#include "chip8.h"

Chip8::Chip8(std::string rom_path, SDL_Window* window) {
  this->window = window;
  initialise_values();
  load_rom(rom_path);
}

void Chip8::initialise_values() {
  this->program_counter = 0x200;
  this->stack_pointer   = 0;
  this->index_register  = 0;
  this->delay_timer     = 0;
  this->sound_timer     = 0;

  this->stack.fill(0);
  this->display.fill(0);
  this->keys.fill(0);
  this->memory.fill(0);

  for (size_t i = 0; i < font.size(); i++) {
    this->memory[i] = font[i];
  }

  /* A random number generator using a time seed. */
  srand(time(NULL));

  /* SDL */
  this->renderer = SDL_CreateRenderer(this->window, -1,
      SDL_RENDERER_ACCELERATED);

  this->texture  = SDL_CreateTexture(
      renderer, SDL_PIXELFORMAT_RGBA8888, 
      SDL_TEXTUREACCESS_STREAMING, COLS, ROWS);
}

void Chip8::load_rom(std::string rom_path) {
  std::ifstream rom(rom_path, std::ios::binary | std::ios::ate);
  std::ifstream::pos_type rom_size = rom.tellg();
  std::vector<uint8_t> buffer(rom_size);
  rom.seekg(0, std::ios::beg);
  rom.read(reinterpret_cast<char *>(buffer.data()), rom_size);

  for (int i = 0; i < rom_size; i++) {
    /* Program mem space starts at 0x200 (512 base10) */
    memory[i + 512] = buffer[i];
  }
}

void Chip8::run() {
  SDL_Event event;
  uint32_t start_time;
  uint32_t delta;

  while (event.type != SDL_QUIT) {
    start_time = SDL_GetTicks();

    for (int i = 0; i < 8; i++) {
      this->cycle();
    }

    SDL_PollEvent(&event);
    // TODO
    delta = SDL_GetTicks() - start_time;
    if ((1000/60) > delta) {
      SDL_Delay((1000/60) - delta);
    }

  }
}

void Chip8::cycle() {
  uint16_t ins = fetch_instruction();
  program_counter += 2;
  execute_instruction(ins);
}

void Chip8::draw() {
  uint32_t* pixels    = nullptr;
  uint8_t pixel_draw  = 0;
  int pitch;

  SDL_LockTexture(texture, nullptr, reinterpret_cast<void**>
      (&pixels), &pitch);

  for (int i=0; i < COLS*ROWS; i++) {
    pixel_draw = this->display[i] == 1;
    pixels[i] = pixel_draw ? 0xFFFFFFFF : 0x000000FF;
  }

  SDL_UnlockTexture(texture);
  SDL_RenderClear(renderer);
  SDL_RenderCopy(renderer, texture, nullptr, nullptr);
  SDL_RenderPresent(renderer);
}

uint16_t Chip8::fetch_instruction() {
  uint16_t ins = this->memory[this->program_counter];

  ins = ins << 8;

  ins = ins | this->memory[this->program_counter + 1];

  return ins;
}

void Chip8::execute_instruction(uint16_t instruction) {
  uint16_t opcode = (instruction & 0xF000);
  uint16_t value_n = (instruction & 0x000F);
  uint16_t value_nn = (instruction & 0x00FF);
  uint16_t value_nnn = (instruction & 0x0FFF);
  uint16_t reg_x = (instruction & 0x0F00) >> 8;
  uint16_t reg_y = (instruction & 0x00F0) >> 4;

  switch (opcode) {
    case 0x0000:
      switch (instruction) {
        case 0x00E0:
          display.fill(0);
          break;
        case 0x00EE:
          program_counter = stack[stack_pointer--];
          break;
      }
      break;
    case 0x1000: // 1nnn -- JUMP to address
      program_counter = value_nnn;
      break;
    case 0x2000: // 2nnn -- CALL a subroutine
      stack[stack_pointer++] = program_counter;
      program_counter = value_nnn;
      break;
    case 0x3000:
      if (registers[reg_x] == value_nn)
        program_counter += 2;
    case 0x4000:
      if (registers[reg_x] != value_nn)
        program_counter += 2;
      break;
    case 0x5000:
      if (registers[reg_x] == registers[reg_y])
        program_counter += 2;
      break;
    case 0x6000:
      registers[reg_x] = value_nn;
      break;
    case 0x7000:
      registers[reg_x] += value_nn;
      break;
    case 0x8000:
      switch (instruction & 0x000F) {
        case 0x0000:
          registers[reg_x] = registers[reg_y];
          break;
        case 0x0001:
          registers[reg_x] = registers[reg_x] | registers[reg_y];
          break;
        case 0x0002:
          registers[reg_x] = registers[reg_x] & registers[reg_y];
          break;
        case 0x0003:
          registers[reg_x] = registers[reg_x] ^ registers[reg_y];
          break;
        case 0x0004: /* carry flag, set to 1 if > 255 */
          registers[0xF] = (registers[reg_x] + registers[reg_y]) > 0xFF;
          registers[reg_x] += registers[reg_y];
          break;
        case 0x0005:
          registers[0xF] = registers[reg_x] > registers[reg_y];
          registers[reg_x] -= registers[reg_y];
          break;
        case 0x0006:
          registers[0xF] = registers[reg_x] & 1;
          registers[reg_x] >>= 1;
          break;
        case 0x007:
          registers[0xF] = registers[reg_y] > registers[reg_x];
          registers[reg_x] = registers[reg_y] - registers[reg_x];
          break;
        case 0x00E:
          registers[0xF] = registers[reg_x] >> 7;
          registers[reg_x] <<= 1;
          break;
      }
      break;
    case 0x9000:
      if (registers[reg_x] != registers[reg_y])
        program_counter += 2;
      break;
    case 0xA000:
      index_register = value_nnn;
      break;
    case 0xB000:
      program_counter = value_nnn + registers[0];
      break;
    case 0xC000:
      registers[reg_x] = (rand() % 256) & value_nn;
      break;
    case 0xD000:
      uint16_t x;
      uint16_t y; 
      uint8_t* screen_pixel;
      
      x = registers[reg_x] % COLS;
      y= registers[reg_y] % ROWS;
      registers[0xF] = 0;

      for (size_t row = 0; row < value_n; row++) {
        uint8_t sprite = memory[index_register + row];
        for (size_t col = 0; col < 8; col++) {
          screen_pixel = &display[(y + row) * COLS + (x + col)];
          if (sprite & (0b1000'0000 >> col)) {
            if (*screen_pixel) {
              registers[0xF] = 1; /* collision */
            }
            *screen_pixel ^= 1;
          }
        }
      }

      this->draw();

      break;
    case 0xE000:
      switch (value_nn) {
        case 0x009E:
          if (keys[registers[reg_x]])
            program_counter += 2;
          break;
        case 0x00A1:
          if (!keys[registers[reg_x]])
            program_counter += 2;
          break;
      }
      break;
    case 0xF000:
      switch (value_nn) {
        case 0x0007:
          registers[reg_x] = delay_timer;
          break;
        case 0x000A:
          for (size_t i = 0; i < keys.size(); i++) {
            if (keys[i]) {
              registers[reg_x] = i;
              break;
            }
          }
          program_counter -= 2;
          break;
        case 0x0015:
          delay_timer = registers[reg_x];
          break;
        case 0x0018:
          sound_timer = registers[reg_x];
          break;
        case 0x001E:
          index_register += registers[reg_x];
          break;
        case 0x029:
          index_register = registers[reg_x];
          break;
        case 0x0033:
          //  1   5    6
          int n, nn, nnn;
          n   = registers[reg_x]/100;
          nn  = (registers[reg_x]/10) % 10;
          nnn = registers[reg_x] % 10;

          memory[index_register]     = n; 
          memory[index_register + 1] = nn; 
          memory[index_register + 2] = nnn; 
          break;
        case 0x0055:
          for (int i=0; i <= reg_x; i++) {
            memory[index_register + i] =  registers[i];
          }
          break;
        case 0x0065:
          for (int i=0; i <= reg_x; i++) {
            registers[i] = memory[index_register + i];
          }
          break;
      }
      break;
    default:
      LOG_ERR("OPCODE not recognised... Abort.");
      exit(1);
  }
}

