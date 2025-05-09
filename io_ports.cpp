#include "io_ports.h"

uint8_t input_port(State8080* cpu, uint8_t port) {

    uint8_t val = 0;
    switch (port) {
    case 0x00: val = *cpu->ports.port0; break;
    case 0x01: val = *cpu->ports.port1; break; // Player 1 input
    case 0x02: val = *cpu->ports.port2; break; // Player 2 input

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

void output_port(State8080* cpu, uint8_t port, uint8_t a) {

    switch (port) {
    case 0x02: {
        cpu->shift_registers.shift_offset = a & 0x7;
        break;
    }
    case 0x03: {
        *cpu->ports.port3 = a;

        break;
    }
    case 0x04: {
        cpu->shift_registers.shift0 = cpu->shift_registers.shift1;
        cpu->shift_registers.shift1 = a;
        break;
    }
    case 0x05: {
        *cpu->ports.port5 = a;

        break;
    }
    default: {
        printf("Error: Invalid output port %02x\n", port);
        break;
    }
    }
    printf("Output, reg A: %02x", a);
}
