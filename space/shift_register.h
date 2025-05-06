#pragma once
#include <cstdint>

void reset_shift_register();
void write_shift_data(uint8_t val);     // OUT 4 or OUT 5
void write_shift_offset(uint8_t val);   // OUT 2
uint8_t read_shift_result();            // IN 3
