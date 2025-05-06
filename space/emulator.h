#pragma once
#include <cstdint>

struct State8080 {
    uint8_t a, b, c, d, e, h, l;
    uint16_t sp;
    uint16_t pc;

    struct {
        uint8_t z : 1;
        uint8_t s : 1;
        uint8_t p : 1;
        uint8_t cy : 1;
        uint8_t ac : 1;
    } flags;

    uint8_t* memory;
    uint8_t int_enable;
};

// Emulator interface
void Emulate8080Op(State8080* state);
void generate_interrupt(State8080* state, int interrupt_num);
void Push(State8080* state, uint16_t val);

// I/O Ports used by emulator core
extern uint8_t input_port1;
extern uint8_t input_port2;
extern uint16_t shift_register;
extern uint8_t shift_offset;
