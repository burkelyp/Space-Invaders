#include "initcpu.h"
#include <iostream>


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
    state->ports = { new uint8_t,  new uint8_t,  new uint8_t,  new uint8_t,  new uint8_t,  new uint8_t,  new uint8_t };


    if ((state->memory = (uint8_t*)malloc(MEMORY_SIZE)) == nullptr) {
        std::cerr << "Failed to allocate memory." << std::endl;
        exit(1);
    }

    // memset(state->memory, 0, MEMORY_SIZE);
}

void initCPU(State8080* state, PlatformMemoryPtr memory_ptr) {
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

    // if null, memory_ptr not passed or invalid
    if (memory_ptr == nullptr) {
        std::cerr << "Shared memory ptr is null." << std::endl;
        exit(1);
    }
    else {
        state->memory = (uint8_t*)memory_ptr; // loss of data? 
    }

    // Setting ports to end of memory 
    state->ports = { state->memory + PORT_LOCATION, 
        state->memory + PORT_LOCATION + 1, 
        state->memory + PORT_LOCATION + 2, 
        state->memory + PORT_LOCATION + 3, 
        state->memory + PORT_LOCATION + 4, 
        state->memory + PORT_LOCATION + 5, 
        state->memory + PORT_LOCATION + 6 };

    //memset(state->memory, 0, MEMORY_SIZE);
}