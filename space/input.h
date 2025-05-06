#pragma once
#include <cstdint>


void init_input();                      // Call once during startup
void update_input_ports();              // Call every frame to poll keys and update port state

uint8_t read_port(uint8_t port);        // Used by IN instruction
void write_port(uint8_t port, uint8_t value); // Used by OUT instruction
