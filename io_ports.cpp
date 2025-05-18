#include "io_ports.h"
#include "sound.h"
#include "miniaudio.h"
#include "emulator.h"
#include <bitset>

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
    case 0x02:
        cpu->shift_registers.shift_offset = a & 0x7;
        break;
    case 0x03: {
        static uint8_t last_port3 = 0;
        if ((a & 0x01) && !(last_port3 & 0x01)) {
            playSound(SOUND_UFO_HIGH);
        } else if (!(a & 0x01) && (last_port3 & 0x01)) {
            stopSound(SOUND_UFO_HIGH);
        }
        if ((a & 0x02) && !(last_port3 & 0x02)) {
            playSound(SOUND_SHOOT);
        }
        if ((a & 0x04) && !(last_port3 & 0x04)) {
            playSound(SOUND_EXPLOSION);
        }
        if ((a & 0x08) && !(last_port3 & 0x08)) {
            playSound(SOUND_INVADER_KILLED);
        }
        last_port3 = a;
        *cpu->ports.port3 = a;
        break;
    }
    case 0x04:
        cpu->shift_registers.shift0 = cpu->shift_registers.shift1;
        cpu->shift_registers.shift1 = a;
        break;
    case 0x05: {
        static uint8_t last_port5 = 0;
        static int current_step = 0;
        uint8_t changed = a ^ last_port5;
        for (int bit = 0; bit <= 3; ++bit) {
            if ((changed & (1 << bit)) && (a & (1 << bit))) {
                playSound(static_cast<SoundEffect>(SOUND_FAST_INVADER_1 + current_step));
                current_step = (current_step + 1) % 4;
                break;
            }
        }
        last_port5 = a;
        *cpu->ports.port5 = a;
        break;
    }
    case 0x06:
        *cpu->ports.port6 = a;
        break;
    default:
        printf("Error: Invalid output port %02x\n", port);
        break;
    }
}
