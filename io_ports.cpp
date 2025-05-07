#include "io_ports.h"

// Block comments written partly by Burkely and updated by Zach after implementing code. Currently unable to test without keyboard mapping. 

/**
* Gets input from keyboard and returns value to in struction set 'IN'.
*
* @param port location of input to be returned.
* @return val sent to port after funciton. Could change to initalize port
* within function using cpu->a.
*/
uint8_t input_port(State8080* cpu, uint8_t port) {

    uint8_t val = 0;
    switch (port) {
    case 0x00: val = cpu->ports.port0; break;
    case 0x01: val = cpu->ports.port1; break; // Player 1 input
    case 0x02: val = cpu->ports.port2; break; // Player 2 input
    case 0x03: {
        uint16_t v = (cpu->shift_registers.shift1 << 8) | cpu->shift_registers.shift0;
        val = ((v >> (8 - cpu->shift_registers.shift_offset)) & 0xff); // Shift register result
        break;
    }
    default: {
        printf("Error: Invalid input port %02x\n", port);
        break;
    }
    }
    return val;
}

/**
* Gets input from keyboard and returns value to in struction set 'IN'.
*
* @param cpu maintains cpu state. Can replace other arguments, but
* keeping port and a for testing with other opcode implementations.
* @param port Location for output to be sent.
* @param a Value from register A, to be sent to {port}.
* @return Value at {port} to be sent back to register A.
*/
void output_port(State8080* cpu, uint8_t port, uint8_t a) {

    switch (port) {
    case 0x02: {
        cpu->shift_registers.shift_offset = a & 0x7;
        break;
    }
    case 0x03: {
        cpu->ports.port3 = a;
        break;
    }
    case 0x04: {
        cpu->shift_registers.shift0 = cpu->shift_registers.shift1;
        cpu->shift_registers.shift1 = a;
        break;
    }
    case 0x05: {
        cpu->ports.port5 = a;
        break;
    }
    default: {
        printf("Error: Invalid output port %02x\n", port);
        break;
    }
    }
    printf("Output, reg A: %02x", a);
}
