#ifndef INIT_CPU
#define INIT_CPU

#define MEMORY_SIZE 0x10000 

#include <iostream>

// Flags
struct ConditionCodes {
    uint8_t    z : 1;
    uint8_t    s : 1;
    uint8_t    p : 1;
    uint8_t    cy : 1;
    uint8_t    ac : 1;
    uint8_t    pad : 3;
};

// Registers, memory, and CoditionCodes maintain CPU state
struct State8080 {
    uint8_t     a = 0;
    uint8_t     b = 0;
    uint8_t     c = 0;
    uint8_t     d = 0;
    uint8_t     e = 0;
    uint8_t     h = 0;
    uint8_t     l = 0;
    uint16_t    sp = 0;
    uint16_t    pc = 0;
    uint8_t* memory = 0;
    ConditionCodes      cc;
    uint8_t     int_enable = 0;
};

void initCPU(State8080* state);

#endif