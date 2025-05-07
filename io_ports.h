#ifndef IO_PORTS
#define IO_PORTS

#include "initCPU.h"
#include <cstdint>

// Block comments written partly by Burkely and updated by Zach after implementing code. Currently unable to test without keyboard mapping. 

/**
* Gets input from keyboard and returns value to in struction set 'IN'.
*
* @param port location of input to be returned.
* @return val sent to port after funciton. Could change to initalize port
* within function using cpu->a.
*/
uint8_t input_port(State8080* cpu, uint8_t port);

/**
* Gets input from keyboard and returns value to in struction set 'IN'.
*
* @param cpu maintains cpu state. Can replace other arguments, but
* keeping port and a for testing with other opcode implementations.
* @param port Location for output to be sent.
* @param a Value from register A, to be sent to {port}.
* @return Value at {port} to be sent back to register A.
*/
void output_port(State8080* cpu, uint8_t port, uint8_t a);

#endif

