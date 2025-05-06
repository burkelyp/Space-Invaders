#include "shift_register.h"

static uint16_t shift_data = 0;    // 16-bit shift register (hi << 8 | lo)
static uint8_t shift_offset = 0;   // Shift amount (0–7)

void reset_shift_register() {
    shift_data = 0;
    shift_offset = 0;
}

void write_shift_data(uint8_t val) {
    // OUT 4 = lower 8 bits
    // OUT 5 = upper 8 bits (shift left by 8)
    shift_data = (shift_data >> 8) | (val << 8);
}

void write_shift_offset(uint8_t val) {
    shift_offset = val & 0x07;  // Only lower 3 bits matter
}

uint8_t read_shift_result() {
    return (shift_data >> (8 - shift_offset)) & 0xFF;
}
