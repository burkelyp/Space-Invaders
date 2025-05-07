#include "initcpu.h"

/**
* Initializes the CPU state by setting default values of flags,
* registers, and pointers. Allocates 16KB of memory and sets
* memory to zero.
*
* @state Pointer to a State8080 struct.
*/

void initCPU(State8080* state) {
    state->a = 0;
    state->b = 0;
    state->c = 0;
    state->d = 0;
    state->e = 0;
    state->h = 0;
    state->l = 0;
    state->sp = 0;
    state->pc = 0;
    state->flags = { 1, 1, 1, 1, 1, 3 };
    state->shift_registers = { 0, 0, 0 };
    state->ports = { 0, 0, 0 };

    if ((state->memory = (uint8_t*)malloc(MEMORY_SIZE)) == nullptr) {
        std::cerr << "Failed to allocate memory." << std::endl;
        exit(1);
    }

    memset(state->memory, 0, MEMORY_SIZE);
}

/**
* Initializes the CPU state by setting default values of flags,
* registers, and pointers. Allocates 16KB of memory and sets
* memory to zero.
* Uses memory map pointer for Windows. 
*
* @state Pointer to a State8080 struct.
* @memory_ptr Pointer to shared memory map buffer for IPC with UI.
*/
void initCPU(State8080* state, LPVOID memory_ptr) {
    state->a = 0;
    state->b = 0;
    state->c = 0;
    state->d = 0;
    state->e = 0;
    state->h = 0;
    state->l = 0;
    state->sp = 0;
    state->pc = 0;
    state->flags = { 1, 1, 1, 1, 1, 3 };
    state->shift_registers = { 0, 0, 0 };
    state->ports = { 0, 0, 0, 0, 0, 0, 0 };

    // if null, memory_ptr not passed or invalid
    if (memory_ptr == nullptr) {
        std::cerr << "Shared memory ptr is null." << std::endl;
        exit(1);
    }
    else {
        state->memory = (uint8_t*)memory_ptr; // loss of data? 
    }

    //memset(state->memory, 0, MEMORY_SIZE);
}