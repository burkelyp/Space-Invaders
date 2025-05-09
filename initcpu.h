#ifndef INIT_CPU
#define INIT_CPU

#define MEMORY_SIZE 0x10007 //0x10003 For now
#define PORT_LOCATION 0x10000 //0x10000 


#include <iostream>
//#include <conio.h>
#include <stdio.h>
#include <stdint.h>

#ifdef _WIN64
    #include <Windows.h>
    #include <tchar.h>
    typedef LPVOID PlatformMemoryPtr;
#else
    typedef void* PlatformMemoryPtr;  // fallback for non-Windows
#endif

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
    uint32_t    cycles = 0;
    uint8_t* memory = nullptr;

    uint8_t     int_enable = 0;

    // Flags
    struct {
        uint8_t    z = 0;
        uint8_t    s = 0;
        uint8_t    p = 0;
        uint8_t    c = 0;   // aka cy for carry flag
        uint8_t    ac = 0;

        uint8_t    pad = 3;
    } flags;

    // shift registers
    struct {
        uint8_t shift0 = 0;
        uint8_t shift1 = 0;
        uint8_t shift_offset = 0;
    } shift_registers;

    struct {
        uint8_t* port0 = nullptr;
        uint8_t* port1 = nullptr;
        uint8_t* port2 = nullptr;
        uint8_t* port3 = nullptr;
        uint8_t* port4 = nullptr;
        uint8_t* port5 = nullptr;
        uint8_t* port6 = nullptr;

    } ports;

    // Interrupt enable
    uint8_t interrupt_enabled = false;

    // Halt capability
    uint8_t halted = false;


};

/**
* Initializes the CPU state by setting default values of flags,
* registers, and pointers. Allocates 16KB of memory and sets
* memory to zero.
*
* @state Pointer to a State8080 struct.
*/
void initCPU(State8080* state);

/**
* Initializes the CPU state by setting default values of flags,
* registers, and pointers. Allocates 16KB of memory and sets
* memory to zero.
* Uses memory map pointer for Windows.
*
* @state Pointer to a State8080 struct.
* @memory_ptr Pointer to shared memory map buffer for IPC with UI.
*/
void initCPU(State8080* state, PlatformMemoryPtr memory_ptr);

#endif