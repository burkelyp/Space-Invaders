#include "initcpu.h"

/**
* Initializes the CPU state by setting default values of flags,
* registers, and pointers. Allocates 65KB of memory and sets
* memory to zero.
*
* @state Pointer to a State8080 struct.
*/
void initCPU(State8080* state) {
    state->cc = { 1, 1, 1, 1, 1, 3 };
    state->a = 0;
    state->b = 0;
    state->c = 0;
    state->d = 0;
    state->e = 0;
    state->h = 0;
    state->l = 0;
    state->sp = 0;
    state->pc = 0;

    if ((state->memory = (uint8_t*)malloc(MEMORY_SIZE)) == nullptr) {
        std::cerr << "Failed to allocate memory." << std::endl;
        exit(1);
    }
    memset(state->memory, 0, MEMORY_SIZE);
}