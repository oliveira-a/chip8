#include "chip8.h"
#include "client.h"

int main(int argc, char** args) {
  Chip8 chip8("test.rom");

  Client client(&chip8);

  client.start();

  return 0;
}
