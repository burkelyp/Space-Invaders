#ifndef IO_PORTS
#define IO_PORTS

#include "initCPU.h"
#include <cstdint>

uint8_t input_port(State8080* cpu, uint8_t port);

void output_port(State8080* cpu, uint8_t port, uint8_t a);

#endif

