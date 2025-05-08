#pragma once
#include <cstdint> // Needed for uint8_t, uint16_t
#include "access_mmap.h"


// Put the State8080 struct definition here...
/*
struct State8080 {
    // 8-bit registers
    uint8_t a = 0;
    uint8_t b = 0;
    uint8_t c = 0;
    uint8_t d = 0;
    uint8_t e = 0;
    uint8_t h = 0;
    uint8_t l = 0;

    // Stack Pointer and Program Counter
    uint16_t sp = 0;
    uint16_t pc = 0;
    uint32_t cycles = 0;

    // Flags
    struct {
        uint8_t    z : 1;   // Zero
        uint8_t    s : 1;   // Sign
        uint8_t    p : 1;   // Parity
        uint8_t    c : 1;   // Carry
        uint8_t    ac : 1;  // Auxiliary Carry
        uint8_t    pad : 3; // Unused bits
    } flags;

    // Interrupt enable
    uint8_t interrupt_enabled;

    // Halt capability
    uint8_t halted = false;

    // Memory (64KB)
    uint8_t memory[0x10000]; // 65536 bytes
};
*/

// Function Declarations
/**
* Sets Zero, Sign, and Parity flags based on {result}.
*
* @param cpu State of the cpu object.
* @param result Result that the flags will be based on.
* @return void: updates state of the flags in the cpu object.
*/
void setZSPflags(State8080* cpu, uint8_t result);

uint8_t input_port(State8080* cpu, uint8_t port);

void output_port(State8080* cpu, uint8_t port, uint8_t a);

/**
* Emulates the Intel 8080 cpu. The cpu has been initialized and the
* rom file should be loaded into memory. This function reads the
* current opcode that the program counter is pointing to, and carries
* out the corresponding instruction. This includes updating the state
* of the cpu: register values, flags, pointers, memory, stack, etc.
*
* @param cpu State of the cpu object.
* @param debug debug mode prints information about each instruction being called.
* @return void: executes instruction sets and updates cpu state.
*/
void Emulate8080Op(State8080* cpu, bool debug = false);