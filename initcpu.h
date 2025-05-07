#ifndef INIT_CPU
#define INIT_CPU

#define MEMORY_SIZE 0x10000 //0x10000 

#include <iostream>
#include <tchar.h>
//#include <conio.h>
#include <stdio.h>
#include <Windows.h>



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
    uint8_t     int_enable = 0;

    // Flags
    struct {
        uint8_t    z = 1;
        uint8_t    s = 1;
        uint8_t    p = 1;
        uint8_t    c = 1;   // aka cy for carry flag
        uint8_t    ac = 1;
        uint8_t    pad = 3;
    } flags;

    // shift registers
    struct {
        uint8_t shift0 = 0;
        uint8_t shift1 = 0;
        uint8_t shift_offset = 0;
    } shift_registers;

    struct {
        uint8_t port0 = 0;
        uint8_t port1 = 0;
        uint8_t port2 = 0;
        uint8_t port3 = 0;
        uint8_t port4 = 0;
        uint8_t port5 = 0;
        uint8_t port6 = 0;
    } ports;

    // Interrupt enable
    uint8_t interrupt_enabled = false;

    // Halt capability
    uint8_t halted = false;


};

void initCPU(State8080* state);

void initCPU(State8080* state, LPVOID memory_ptr);

#endif