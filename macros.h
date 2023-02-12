#ifndef MACROS_H
#define MACROS_H

#define NAME "Chip8"
#define LOG_ERR(x) std::cerr << "ERROR: " << x << std::endl;
#define LOG_STR(x) printf("LOG: %s\n", x);
#define LOG_INT(x) printf("LOG: %d\n", x);

#define COLS 64
#define ROWS 32
#define SCALE 10
#define FPS 60

#endif
