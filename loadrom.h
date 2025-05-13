#ifndef LOADROM
#define LOADROM

#include "initcpu.h"

#include <stdio.h>
#include <stdlib.h>
#include <fstream>

/**
* Loads ROM into the emulated Intel8080 CPU.
*
* @param path C-style string representing the path to the ROM file.
* @param state Pointer to a State8080 struct holding the CPU state and memory.
*/
void loadROM(const char* path, State8080* state, uint16_t offset);

#endif