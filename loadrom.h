#ifndef LOADROM
#define LOADROM

#include "initcpu.h"

#include <stdio.h>
#include <stdlib.h>
#include <fstream>


void loadROM(const char* path, State8080* state);

#endif