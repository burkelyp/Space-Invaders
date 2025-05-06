#include "input.h"
#include <SDL2/SDL.h>
#include <cstdint>

// Global I/O state variables used in emulator.cpp
uint8_t input_port1 = 0;
uint8_t input_port2 = 0;
uint16_t shift_register = 0;
uint8_t shift_offset = 0;

// I/O ports
static uint8_t port1 = 0;
static uint8_t port2 = 0;

// Shift register is handled in a separate module
extern uint8_t read_shift_result();

// Key mapping
// Bit positions in port 1 (player 1):
// Bit 0: Coin
// Bit 1: P2 start (optional)
// Bit 2: P1 start
// Bit 3: Always 1
// Bit 4: P1 fire
// Bit 5: P1 left
// Bit 6: P1 right
// Bit 7: Always 0

void init_input() {
    port1 = 0x08;  // Bit 3 must be always 1
    port2 = 0x00;
}

void update_input_ports() {
    const Uint8* keys = SDL_GetKeyboardState(nullptr);
    

    // Reset and force bit 3 (0x08) always ON for both ports
    port1 = 0x08;
    port2 = 0x08;

    // Player 1
    if (keys[SDL_SCANCODE_C])      port1 |= 0x01;
    if (keys[SDL_SCANCODE_2])      port1 |= 0x02;
    if (keys[SDL_SCANCODE_1])      port1 |= 0x04;
    if (keys[SDL_SCANCODE_SPACE])  port1 |= 0x10;
    if (keys[SDL_SCANCODE_LEFT])   port1 |= 0x20;
    if (keys[SDL_SCANCODE_RIGHT])  port1 |= 0x40;

    // Player 2
    if (keys[SDL_SCANCODE_F])      port2 |= 0x01;
    if (keys[SDL_SCANCODE_A])      port2 |= 0x02;
    if (keys[SDL_SCANCODE_D])      port2 |= 0x04;
    if (keys[SDL_SCANCODE_2])      port2 |= 0x10;

    // Ensure bit 3 stays set even if no keys are pressed
    port2 |= 0x08;

    input_port1 = port1;
    input_port2 = port2;

    printf("PORT2 = %02X\n", input_port2);
}





// Called when 8080 executes IN port
uint8_t read_port(uint8_t port) {
    switch (port) {
        case 1: return port1;
        case 2: return port2;
        case 3: return read_shift_result(); // From shift_register module
        default: return 0;
    }
}

// Called when 8080 executes OUT port
void write_port(uint8_t port, uint8_t value) {
    // Shift register and sound handling expected here
    extern void write_shift_data(uint8_t);
    extern void write_shift_offset(uint8_t);

    switch (port) {
        case 2: write_shift_offset(value); break;
        case 4: write_shift_data(value); break;
        case 5: /* Sound control, optional */ break;
        default: break;
    }
}
