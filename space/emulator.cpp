// emulator.cpp
#include "emulator.h"
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <iostream>



// Helper: Check parity of byte
int parity(uint8_t x) {
    int count = 0;
    while (x) {
        count += x & 1;
        x >>= 1;
    }
    return !(count & 1);
}
void generate_interrupt(State8080* state, int interrupt_num) {
    state->int_enable = 0;
    Push(state, state->pc);
    state->pc = 8 * interrupt_num;
}
void Push(State8080* state, uint16_t val) {
    state->memory[state->sp - 1] = (val >> 8) & 0xff;
    state->memory[state->sp - 2] = val & 0xff;
    state->sp -= 2;
}

// Helper: Get register by encoded index
uint8_t& reg_by_index(State8080* state, int index) {
    switch (index) {
        case 0: return state->b;
        case 1: return state->c;
        case 2: return state->d;
        case 3: return state->e;
        case 4: return state->h;
        case 5: return state->l;
        case 6: {
            static uint8_t dummy;
            dummy = state->memory[(state->h << 8) | state->l];
            return dummy; // Warning: readonly proxy
        }
        case 7: return state->a;
        default: exit(1);
    }
}

// Full 256-opcode implementation
void Emulate8080Op(State8080* state) {
    uint8_t *opcode = &state->memory[state->pc];
    //printf("%04x: %02x", state->pc, *opcode);

    switch (*opcode) {
        case 0x00: state->pc++; break; // NOP
        case 0x01: state->c = opcode[1]; state->b = opcode[2]; state->pc += 3; break; // LXI B
        case 0x76: printf("HLT"); exit(0); // HLT
        case 0x02: state->memory[(state->b << 8) | state->c] = state->a; state->pc++; break; // STAX B
        case 0x03: if (++state->c == 0) ++state->b; state->pc++; break; // INX B
        case 0x04: state->b++; state->flags.z = (state->b == 0); state->flags.s = (state->b & 0x80) != 0; state->flags.p = parity(state->b); state->pc++; break; // INR B
        case 0x05: state->b--; state->flags.z = (state->b == 0); state->flags.s = (state->b & 0x80) != 0; state->flags.p = parity(state->b); state->pc++; break; // DCR B
        case 0x06: state->b = opcode[1]; state->pc += 2; break; // MVI B
        case 0x07: { uint8_t x = state->a; state->a = (x << 1) | (x >> 7); state->flags.cy = (x & 0x80) != 0; state->pc++; break; } // RLC
        case 0x08: state->pc++; break; // NOP (undocumented)
        case 0x09: { uint16_t hl = (state->h << 8) | state->l; uint16_t bc = (state->b << 8) | state->c; uint32_t res = hl + bc; state->h = (res >> 8) & 0xff; state->l = res & 0xff; state->flags.cy = (res > 0xffff); state->pc++; break; } // DAD B
        case 0x0a: state->a = state->memory[(state->b << 8) | state->c]; state->pc++; break; // LDAX B
        case 0x0b: if (state->c-- == 0) state->b--; state->pc++; break; // DCX B
        case 0x0c: state->c++; state->flags.z = (state->c == 0); state->flags.s = (state->c & 0x80) != 0; state->flags.p = parity(state->c); state->pc++; break; // INR C
        case 0x0d: state->c--; state->flags.z = (state->c == 0); state->flags.s = (state->c & 0x80) != 0; state->flags.p = parity(state->c); state->pc++; break; // DCR C
        case 0x0e: state->c = opcode[1]; state->pc += 2; break; // MVI C
        case 0x0f: { uint8_t x = state->a; state->a = (x >> 1) | (x << 7); state->flags.cy = (x & 0x01) != 0; state->pc++; break; } // RRC
        case 0x10: state->pc++; break; // NOP (undocumented)
        case 0x11: state->e = opcode[1]; state->d = opcode[2]; state->pc += 3; break; // LXI D
        case 0x12: state->memory[(state->d << 8) | state->e] = state->a; state->pc++; break; // STAX D
        case 0x13: if (++state->e == 0) ++state->d; state->pc++; break; // INX D
        case 0x14: state->d++; state->flags.z = (state->d == 0); state->flags.s = (state->d & 0x80) != 0; state->flags.p = parity(state->d); state->pc++; break; // INR D
        case 0x15: state->d--; state->flags.z = (state->d == 0); state->flags.s = (state->d & 0x80) != 0; state->flags.p = parity(state->d); state->pc++; break; // DCR D
        case 0x16: state->d = opcode[1]; state->pc += 2; break; // MVI D
        case 0x17: { uint8_t x = state->a; state->a = (x << 1) | state->flags.cy; state->flags.cy = (x & 0x80) != 0; state->pc++; break; } // RAL
        case 0x18: state->pc++; break; // NOP (undocumented)
        case 0x19: { uint16_t hl = (state->h << 8) | state->l; uint16_t de = (state->d << 8) | state->e; uint32_t res = hl + de; state->h = (res >> 8) & 0xff; state->l = res & 0xff; state->flags.cy = (res > 0xffff); state->pc++; break; } // DAD D
        case 0x1a: state->a = state->memory[(state->d << 8) | state->e]; state->pc++; break; // LDAX D
        case 0x1b: if (state->e-- == 0) state->d--; state->pc++; break; // DCX D
        case 0x1c: state->e++; state->flags.z = (state->e == 0); state->flags.s = (state->e & 0x80) != 0; state->flags.p = parity(state->e); state->pc++; break; // INR E
        case 0x1d: state->e--; state->flags.z = (state->e == 0); state->flags.s = (state->e & 0x80) != 0; state->flags.p = parity(state->e); state->pc++; break; // DCR E
        case 0x1e: state->e = opcode[1]; state->pc += 2; break; // MVI E
        case 0x1f: { uint8_t x = state->a; state->a = (x >> 1) | (state->flags.cy << 7); state->flags.cy = (x & 0x01) != 0; state->pc++; break; } // RAR
        case 0x20: state->pc++; break; // NOP (undocumented)
        case 0x21: state->l = opcode[1]; state->h = opcode[2]; state->pc += 3; break; // LXI H
        case 0x22: { uint16_t addr = opcode[2] << 8 | opcode[1]; state->memory[addr] = state->l; state->memory[addr + 1] = state->h; state->pc += 3; break; } // SHLD addr
        case 0x23: if (++state->l == 0) ++state->h; state->pc++; break; // INX H
        case 0x24: state->h++; state->flags.z = (state->h == 0); state->flags.s = (state->h & 0x80) != 0; state->flags.p = parity(state->h); state->pc++; break; // INR H
        case 0x25: state->h--; state->flags.z = (state->h == 0); state->flags.s = (state->h & 0x80) != 0; state->flags.p = parity(state->h); state->pc++; break; // DCR H
        case 0x26: state->h = opcode[1]; state->pc += 2; break; // MVI H
        case 0x27: { // DAA (simplified)
            uint8_t correction = 0; if ((state->a & 0x0F) > 9 || state->flags.ac) correction |= 0x06;
            if ((state->a >> 4) > 9 || state->flags.cy) correction |= 0x60;
            uint16_t res = state->a + correction;
            state->flags.cy = (res > 0xFF);
            state->a = res & 0xFF;
            state->flags.z = (state->a == 0);
            state->flags.s = (state->a & 0x80) != 0;
            state->flags.p = parity(state->a);
            state->pc++;
            break; } // DAA
        case 0x28: state->pc++; break; // NOP (undocumented)
        case 0x29: { uint16_t hl = (state->h << 8) | state->l; uint32_t res = hl + hl; state->h = (res >> 8) & 0xff; state->l = res & 0xff; state->flags.cy = (res > 0xffff); state->pc++; break; } // DAD H
        case 0x2a: { uint16_t addr = opcode[2] << 8 | opcode[1]; state->l = state->memory[addr]; state->h = state->memory[addr + 1]; state->pc += 3; break; } // LHLD addr
        case 0x2b: if (state->l-- == 0) state->h--; state->pc++; break; // DCX H
        case 0x2c: state->l++; state->flags.z = (state->l == 0); state->flags.s = (state->l & 0x80) != 0; state->flags.p = parity(state->l); state->pc++; break; // INR L
        case 0x2d: state->l--; state->flags.z = (state->l == 0); state->flags.s = (state->l & 0x80) != 0; state->flags.p = parity(state->l); state->pc++; break; // DCR L
        case 0x2e: state->l = opcode[1]; state->pc += 2; break; // MVI L
        case 0x2f: state->a = ~state->a; state->pc++; break; // CMA
        case 0x30: state->pc++; break; // NOP (undocumented)
        case 0x31: state->sp = opcode[2] << 8 | opcode[1]; state->pc += 3; break; // LXI SP
        case 0x32: { uint16_t addr = opcode[2] << 8 | opcode[1]; state->memory[addr] = state->a; state->pc += 3; break; } // STA addr
        case 0x33: state->sp++; state->pc++; break; // INX SP
        case 0x34: { uint16_t addr = (state->h << 8) | state->l; state->memory[addr]++; uint8_t val = state->memory[addr]; state->flags.z = (val == 0); state->flags.s = (val & 0x80) != 0; state->flags.p = parity(val); state->pc++; break; } // INR M
        case 0x35: { uint16_t addr = (state->h << 8) | state->l; state->memory[addr]--; uint8_t val = state->memory[addr]; state->flags.z = (val == 0); state->flags.s = (val & 0x80) != 0; state->flags.p = parity(val); state->pc++; break; } // DCR M
        case 0x36: { uint16_t addr = (state->h << 8) | state->l; state->memory[addr] = opcode[1]; state->pc += 2; break; } // MVI M
        case 0x37: state->flags.cy = 1; state->pc++; break; // STC
        case 0x38: state->pc++; break; // NOP (undocumented)
        case 0x39: { uint16_t hl = (state->h << 8) | state->l; uint32_t res = hl + state->sp; state->h = (res >> 8) & 0xff; state->l = res & 0xff; state->flags.cy = (res > 0xffff); state->pc++; break; } // DAD SP
        case 0x3a: { uint16_t addr = opcode[2] << 8 | opcode[1]; state->a = state->memory[addr]; state->pc += 3; break; } // LDA addr
        case 0x3b: state->sp--; state->pc++; break; // DCX SP
        case 0x3c: state->a++; state->flags.z = (state->a == 0); state->flags.s = (state->a & 0x80) != 0; state->flags.p = parity(state->a); state->pc++; break; // INR A
        case 0x3d: state->a--; state->flags.z = (state->a == 0); state->flags.s = (state->a & 0x80) != 0; state->flags.p = parity(state->a); state->pc++; break; // DCR A
        case 0x3e: state->a = opcode[1]; state->pc += 2; break; // MVI A
        case 0x3f: state->flags.cy = !state->flags.cy; state->pc++; break; // CMC

        // MOV r1, r2: opcodes 0x40 to 0x7F
        case 0x40: case 0x41: case 0x42: case 0x43: case 0x44: case 0x45: case 0x46: case 0x47:
        case 0x48: case 0x49: case 0x4A: case 0x4B: case 0x4C: case 0x4D: case 0x4E: case 0x4F:
        case 0x50: case 0x51: case 0x52: case 0x53: case 0x54: case 0x55: case 0x56: case 0x57:
        case 0x58: case 0x59: case 0x5A: case 0x5B: case 0x5C: case 0x5D: case 0x5E: case 0x5F:
        case 0x60: case 0x61: case 0x62: case 0x63: case 0x64: case 0x65: case 0x66: case 0x67:
        case 0x68: case 0x69: case 0x6A: case 0x6B: case 0x6C: case 0x6D: case 0x6E: case 0x6F:
        case 0x70: case 0x71: case 0x72: case 0x73: case 0x74: case 0x75: case 0x77:
        case 0x78: case 0x79: case 0x7A: case 0x7B: case 0x7C: case 0x7D: case 0x7E: case 0x7F: {
            int dst = (*opcode >> 3) & 0x07;
            int src = (*opcode) & 0x07;
            uint16_t hl = (state->h << 8) | state->l;
            uint8_t value = (src == 6) ? state->memory[hl] : reg_by_index(state, src);
            if (dst == 6) state->memory[hl] = value;
            else reg_by_index(state, dst) = value;
            state->pc++;
            break;
        }

        // ADD r: 0x80–0x87
        case 0x80: case 0x81: case 0x82: case 0x83:
        case 0x84: case 0x85: case 0x86: case 0x87: {
            int src = *opcode & 0x07;
            uint8_t val = (src == 6) ? state->memory[(state->h << 8) | state->l] : reg_by_index(state, src);
            uint16_t res = (uint16_t)state->a + val;
            state->flags.z = ((res & 0xff) == 0);
            state->flags.s = (res & 0x80) != 0;
            state->flags.p = parity(res & 0xff);
            state->flags.cy = res > 0xff;
            state->flags.ac = ((state->a & 0xf) + (val & 0xf)) > 0xf;
            state->a = res & 0xff;
            state->pc++;
            break;
        }

        // ADC r: 0x88–0x8F
        case 0x88: case 0x89: case 0x8A: case 0x8B:
        case 0x8C: case 0x8D: case 0x8E: case 0x8F: {
            int src = *opcode & 0x07;
            uint8_t val = (src == 6) ? state->memory[(state->h << 8) | state->l] : reg_by_index(state, src);
            uint16_t res = (uint16_t)state->a + val + state->flags.cy;
            state->flags.z = ((res & 0xff) == 0);
            state->flags.s = (res & 0x80) != 0;
            state->flags.p = parity(res & 0xff);
            state->flags.cy = res > 0xff;
            state->flags.ac = ((state->a & 0xf) + (val & 0xf) + state->flags.cy) > 0xf;
            state->a = res & 0xff;
            state->pc++;
            break;
        }

        // SUB r: 0x90–0x97
        case 0x90: case 0x91: case 0x92: case 0x93:
        case 0x94: case 0x95: case 0x96: case 0x97: {
            int src = *opcode & 0x07;
            uint8_t val = (src == 6) ? state->memory[(state->h << 8) | state->l] : reg_by_index(state, src);
            uint16_t res = (uint16_t)state->a - val;
            state->flags.z = ((res & 0xff) == 0);
            state->flags.s = (res & 0x80) != 0;
            state->flags.p = parity(res & 0xff);
            state->flags.cy = (state->a < val);
            state->flags.ac = ((state->a & 0xf) < (val & 0xf));
            state->a = res & 0xff;
            state->pc++;
            break;
        }

        // SBB r: 0x98–0x9F
        case 0x98: case 0x99: case 0x9A: case 0x9B:
        case 0x9C: case 0x9D: case 0x9E: case 0x9F: { int src = *opcode & 0x07;
            uint8_t val = (src == 6) ? state->memory[(state->h << 8) | state->l] : reg_by_index(state, src);
            uint16_t res = (uint16_t)state->a - val - state->flags.cy;
            state->flags.z = ((res & 0xff) == 0);
            state->flags.s = (res & 0x80) != 0;
            state->flags.p = parity(res & 0xff);
            state->flags.cy = (state->a < (val + state->flags.cy));
            state->flags.ac = ((state->a & 0xf) < ((val + state->flags.cy) & 0xf));
            state->a = res & 0xff;
            state->pc++; break; }

        // ANA r: 0xA0–0xA7
        case 0xA0: case 0xA1: case 0xA2: case 0xA3:
        case 0xA4: case 0xA5: case 0xA6: case 0xA7: {
            int src = *opcode & 0x07;
            uint8_t val = (src == 6) ? state->memory[(state->h << 8) | state->l] : reg_by_index(state, src);
            state->a = state->a & val;
            state->flags.z = (state->a == 0);
            state->flags.s = (state->a & 0x80) != 0;
            state->flags.p = parity(state->a);
            state->flags.cy = 0;
            state->flags.ac = 1;
            state->pc++;
            break;
        }

        // XRA r: 0xA8–0xAF
        case 0xA8: case 0xA9: case 0xAA: case 0xAB:
        case 0xAC: case 0xAD: case 0xAE: case 0xAF: {
            int src = *opcode & 0x07;
            uint8_t val = (src == 6) ? state->memory[(state->h << 8) | state->l] : reg_by_index(state, src);
            state->a = state->a ^ val;
            state->flags.z = (state->a == 0);
            state->flags.s = (state->a & 0x80) != 0;
            state->flags.p = parity(state->a);
            state->flags.cy = 0;
            state->flags.ac = 0;
            state->pc++;
            break;
        }

        // ORA r: 0xB0–0xB7
        case 0xB0: case 0xB1: case 0xB2: case 0xB3:
        case 0xB4: case 0xB5: case 0xB6: case 0xB7: {
            int src = *opcode & 0x07;
            uint8_t val = (src == 6) ? state->memory[(state->h << 8) | state->l] : reg_by_index(state, src);
            state->a = state->a | val;
            state->flags.z = (state->a == 0);
            state->flags.s = (state->a & 0x80) != 0;
            state->flags.p = parity(state->a);
            state->flags.cy = 0;
            state->flags.ac = 0;
            state->pc++;
            break;
        }

        // CMP r: 0xB8–0xBF
        case 0xB8: case 0xB9: case 0xBA: case 0xBB:
        case 0xBC: case 0xBD: case 0xBE: case 0xBF: {
            int src = *opcode & 0x07;
            uint8_t val = (src == 6) ? state->memory[(state->h << 8) | state->l] : reg_by_index(state, src);
            uint16_t res = (uint16_t)state->a - val;
            state->flags.z = ((res & 0xff) == 0);
            state->flags.s = (res & 0x80) != 0;
            state->flags.p = parity(res & 0xff);
            state->flags.cy = (state->a < val);
            state->flags.ac = ((state->a & 0xf) < (val & 0xf));
            state->pc++;
            break;
        }

        // RET/Conditional RET/Conditional JMP: 0xC0–0xC9
        case 0xC0: if (!state->flags.z) { state->pc = state->memory[state->sp] | (state->memory[state->sp+1] << 8); state->sp += 2; } else state->pc++; break; // RNZ
        case 0xC1: state->c = state->memory[state->sp]; state->b = state->memory[state->sp+1]; state->sp += 2; state->pc++; break; // POP B
        case 0xC2: if (!state->flags.z) state->pc = opcode[2]<<8 | opcode[1]; else state->pc += 3; break; // JNZ addr
        case 0xC3: state->pc = opcode[2]<<8 | opcode[1]; break; // JMP addr
        case 0xC4: if (!state->flags.z) { uint16_t ret = state->pc+3; state->memory[state->sp-1] = (ret >> 8); state->memory[state->sp-2] = ret & 0xff; state->sp -= 2; state->pc = opcode[2]<<8 | opcode[1]; } else state->pc += 3; break; // CNZ addr
        case 0xC5: state->sp -= 2; state->memory[state->sp] = state->c; state->memory[state->sp+1] = state->b; state->pc++; break; // PUSH B
        case 0xC6: { uint16_t res = state->a + opcode[1]; state->flags.z = ((res & 0xff) == 0); state->flags.s = (res & 0x80) != 0; state->flags.p = parity(res & 0xff); state->flags.cy = res > 0xff; state->flags.ac = ((state->a & 0xf) + (opcode[1] & 0xf)) > 0xf; state->a = res & 0xff; state->pc += 2; break; } // ADI byte
        case 0xC7: state->sp -= 2; state->memory[state->sp] = (state->pc >> 0) & 0xff; state->memory[state->sp+1] = (state->pc >> 8) & 0xff; state->pc = 0x00; break; // RST 0
        case 0xC8: if (state->flags.z) { state->pc = state->memory[state->sp] | (state->memory[state->sp+1] << 8); state->sp += 2; } else state->pc++; break; // RZ
        case 0xC9: state->pc = state->memory[state->sp] | (state->memory[state->sp+1] << 8); state->sp += 2; break;
        case 0xCA: if (state->flags.z) state->pc = opcode[2]<<8 | opcode[1]; else state->pc += 3; break; // JZ addr
        case 0xCB: state->pc++; break; // NOP (undocumented)
        case 0xCC: if (state->flags.z) { uint16_t ret = state->pc+3; state->memory[state->sp-1] = (ret >> 8); state->memory[state->sp-2] = ret & 0xff; state->sp -= 2; state->pc = opcode[2]<<8 | opcode[1]; } else state->pc += 3; break; // CZ addr
        case 0xCD: { uint16_t ret = state->pc+3; state->memory[state->sp-1] = (ret >> 8); state->memory[state->sp-2] = ret & 0xff; state->sp -= 2; state->pc = opcode[2]<<8 | opcode[1]; break; } // CALL addr
        case 0xCE: { uint16_t res = state->a + opcode[1] + state->flags.cy; state->flags.z = ((res & 0xff) == 0); state->flags.s = (res & 0x80) != 0; state->flags.p = parity(res & 0xff); state->flags.cy = res > 0xff; state->flags.ac = ((state->a & 0xf) + (opcode[1] & 0xf) + state->flags.cy) > 0xf; state->a = res & 0xff; state->pc += 2; break; } // ACI byte
        case 0xCF: state->sp -= 2; state->memory[state->sp] = (state->pc >> 0) & 0xff; state->memory[state->sp+1] = (state->pc >> 8) & 0xff; state->pc = 0x08; break; // RST 1
        case 0xD0: if (!state->flags.cy) { state->pc = state->memory[state->sp] | (state->memory[state->sp+1] << 8); state->sp += 2; } else state->pc++; break; // RNC
        case 0xD1: state->e = state->memory[state->sp]; state->d = state->memory[state->sp+1]; state->sp += 2; state->pc++; break; // POP D
        case 0xD2: if (!state->flags.cy) state->pc = opcode[2]<<8 | opcode[1]; else state->pc += 3; break; // JNC addr
        //case 0xD3: printf("OUT %02x", opcode[1]); state->pc += 2; break; // OUT byte
        case 0xD3: {
            uint8_t port = state->memory[state->pc + 1];
            switch (port) {
                case 2:
                    shift_offset = state->a & 0x07;
                    break;
                case 4:
                    shift_register = (state->a << 8) | (shift_register >> 8);
                    break;
                default:
                    // Could log unhandled ports if desired
                    break;
            }
            state->pc += 2;
            break;
        }
        
        case 0xD4: if (!state->flags.cy) { uint16_t ret = state->pc+3; state->memory[state->sp-1] = (ret >> 8); state->memory[state->sp-2] = ret & 0xff; state->sp -= 2; state->pc = opcode[2]<<8 | opcode[1]; } else state->pc += 3; break; // CNC addr
        case 0xD5: state->sp -= 2; state->memory[state->sp] = state->e; state->memory[state->sp+1] = state->d; state->pc++; break; // PUSH D
        case 0xD6: { uint16_t res = state->a - opcode[1]; state->flags.z = ((res & 0xff) == 0); state->flags.s = (res & 0x80) != 0; state->flags.p = parity(res & 0xff); state->flags.cy = (state->a < opcode[1]); state->flags.ac = ((state->a & 0xf) < (opcode[1] & 0xf)); state->a = res & 0xff; state->pc += 2; break; } // SUI byte
        case 0xD7: state->sp -= 2; state->memory[state->sp] = (state->pc >> 0) & 0xff; state->memory[state->sp+1] = (state->pc >> 8) & 0xff; state->pc = 0x10; break; // RST 2
        case 0xD8: if (state->flags.cy) { state->pc = state->memory[state->sp] | (state->memory[state->sp+1] << 8); state->sp += 2; } else state->pc++; break; // RC
        case 0xD9: state->pc++; break; // NOP (undocumented)
        case 0xDA: if (state->flags.cy) state->pc = opcode[2]<<8 | opcode[1]; else state->pc += 3; break; // JC addr
        //case 0xDB: printf("IN %02x", opcode[1]); state->pc += 2; break; // IN byte
        case 0xDB: {
            uint8_t port = state->memory[state->pc + 1];
            uint8_t value = 0;
        
            switch (port) {
                case 0:
                    value = 0x0F; // Example: always return coin input high bits
                    break;
                case 1:
                    value = input_port1;
                    break;
                case 2:
                    value = input_port2;
                    break;
                case 3:
                    value = (shift_register >> (8 - shift_offset)) & 0xFF;
                    break;
                default:
                    std::cerr << "Unhandled IN port: " << (int)port << std::endl;
                    break;
            }
        
            state->a = value;
            state->pc += 2;
            break;
        }
        
        case 0xDC: if (state->flags.cy) { uint16_t ret = state->pc+3; state->memory[state->sp-1] = (ret >> 8); state->memory[state->sp-2] = ret & 0xff; state->sp -= 2; state->pc = opcode[2]<<8 | opcode[1]; } else state->pc += 3; break; // CC addr
        case 0xDD: state->pc++; break; // NOP (undocumented)
        case 0xDE: { uint16_t res = state->a - opcode[1] - state->flags.cy; state->flags.z = ((res & 0xff) == 0); state->flags.s = (res & 0x80) != 0; state->flags.p = parity(res & 0xff); state->flags.cy = (state->a < (opcode[1] + state->flags.cy)); state->flags.ac = ((state->a & 0xf) < ((opcode[1] + state->flags.cy) & 0xf)); state->a = res & 0xff; state->pc += 2; break; } // SBI byte
        case 0xDF: state->sp -= 2; state->memory[state->sp] = (state->pc >> 0) & 0xff; state->memory[state->sp+1] = (state->pc >> 8) & 0xff; state->pc = 0x18; break; // RST 3

        // 0xE0–0xEF
        case 0xE0: if (!state->flags.p) { state->pc = state->memory[state->sp] | (state->memory[state->sp+1] << 8); state->sp += 2; } else state->pc++; break; // RPO
        case 0xE1: state->l = state->memory[state->sp]; state->h = state->memory[state->sp+1]; state->sp += 2; state->pc++; break; // POP H
        case 0xE2: if (!state->flags.p) state->pc = opcode[2]<<8 | opcode[1]; else state->pc += 3; break; // JPO addr
        case 0xE3: { uint8_t temp = state->l; state->l = state->memory[state->sp]; state->memory[state->sp] = temp; temp = state->h; state->h = state->memory[state->sp+1]; state->memory[state->sp+1] = temp; state->pc++; break; } // XTHL
        case 0xE4: if (!state->flags.p) { uint16_t ret = state->pc+3; state->memory[state->sp-1] = (ret >> 8); state->memory[state->sp-2] = ret & 0xff; state->sp -= 2; state->pc = opcode[2]<<8 | opcode[1]; } else state->pc += 3; break; // CPO addr
        case 0xE5: state->sp -= 2; state->memory[state->sp] = state->l; state->memory[state->sp+1] = state->h; state->pc++; break; // PUSH H
        case 0xE6: state->a = state->a & opcode[1]; state->flags.z = (state->a == 0); state->flags.s = (state->a & 0x80) != 0; state->flags.p = parity(state->a); state->flags.cy = 0; state->flags.ac = 1; state->pc += 2; break; // ANI byte
        case 0xE7: state->sp -= 2; state->memory[state->sp] = (state->pc >> 0) & 0xff; state->memory[state->sp+1] = (state->pc >> 8) & 0xff; state->pc = 0x20; break; // RST 4
        case 0xE8: if (state->flags.p) { state->pc = state->memory[state->sp] | (state->memory[state->sp+1] << 8); state->sp += 2; } else state->pc++; break; // RPE
        case 0xE9: state->pc = (state->h << 8) | state->l; break; // PCHL
        case 0xEA: if (state->flags.p) state->pc = opcode[2]<<8 | opcode[1]; else state->pc += 3; break; // JPE addr
        case 0xEB: { uint8_t t = state->d; state->d = state->h; state->h = t; t = state->e; state->e = state->l; state->l = t; state->pc++; break; } // XCHG
        case 0xEC: if (state->flags.p) { uint16_t ret = state->pc+3; state->memory[state->sp-1] = (ret >> 8); state->memory[state->sp-2] = ret & 0xff; state->sp -= 2; state->pc = opcode[2]<<8 | opcode[1]; } else state->pc += 3; break; // CPE addr
        case 0xED: state->pc++; break; // NOP (undocumented)
        case 0xEE: state->a = state->a ^ opcode[1]; state->flags.z = (state->a == 0); state->flags.s = (state->a & 0x80) != 0; state->flags.p = parity(state->a); state->flags.cy = 0; state->flags.ac = 0; state->pc += 2; break; // XRI byte
        case 0xEF: state->sp -= 2; state->memory[state->sp] = (state->pc >> 0) & 0xff; state->memory[state->sp+1] = (state->pc >> 8) & 0xff; state->pc = 0x28; break; // RST 5

        // 0xF0–0xFF
        case 0xF0: if (!state->flags.s) { state->pc = state->memory[state->sp] | (state->memory[state->sp+1] << 8); state->sp += 2; } else state->pc++; break; // RP
        case 0xF1: state->flags.z = (state->memory[state->sp] & 0x40) != 0; state->flags.s = (state->memory[state->sp] & 0x80) != 0; state->flags.p = (state->memory[state->sp] & 0x04) != 0; state->flags.cy = (state->memory[state->sp] & 0x01) != 0; state->flags.ac = (state->memory[state->sp] & 0x10) != 0; state->a = state->memory[state->sp+1]; state->sp += 2; state->pc++; break; // POP PSW
        case 0xF2: if (!state->flags.s) state->pc = opcode[2]<<8 | opcode[1]; else state->pc += 3; break; // JP addr
        case 0xF3: state->int_enable = 0; state->pc++; break; // DI
        case 0xF4: if (!state->flags.s) { uint16_t ret = state->pc+3; state->memory[state->sp-1] = (ret >> 8); state->memory[state->sp-2] = ret & 0xff; state->sp -= 2; state->pc = opcode[2]<<8 | opcode[1]; } else state->pc += 3; break; // CP addr
        case 0xF5: state->sp -= 2; state->memory[state->sp] = (state->flags.z << 6) | (state->flags.s << 7) | (state->flags.p << 2) | (state->flags.cy) | (state->flags.ac << 4); state->memory[state->sp+1] = state->a; state->pc++; break; // PUSH PSW
        case 0xF6: state->a = state->a | opcode[1]; state->flags.z = (state->a == 0); state->flags.s = (state->a & 0x80) != 0; state->flags.p = parity(state->a); state->flags.cy = 0; state->flags.ac = 0; state->pc += 2; break; // ORI byte
        case 0xF7: state->sp -= 2; state->memory[state->sp] = (state->pc >> 0) & 0xff; state->memory[state->sp+1] = (state->pc >> 8) & 0xff; state->pc = 0x30; break; // RST 6
        case 0xF8: if (state->flags.s) { state->pc = state->memory[state->sp] | (state->memory[state->sp+1] << 8); state->sp += 2; } else state->pc++; break; // RM
        case 0xF9: state->sp = (state->h << 8) | state->l; state->pc++; break; // SPHL
        case 0xFA: if (state->flags.s) state->pc = opcode[2]<<8 | opcode[1]; else state->pc += 3; break; // JM addr
        case 0xFB: state->int_enable = 1; state->pc++; break; // EI
        case 0xFC: if (state->flags.s) { uint16_t ret = state->pc+3; state->memory[state->sp-1] = (ret >> 8); state->memory[state->sp-2] = ret & 0xff; state->sp -= 2; state->pc = opcode[2]<<8 | opcode[1]; } else state->pc += 3; break; // CM addr
        case 0xFD: state->pc++; break; // NOP (undocumented)
        case 0xFE: { uint16_t res = state->a - opcode[1]; state->flags.z = ((res & 0xff) == 0); state->flags.s = (res & 0x80) != 0; state->flags.p = parity(res & 0xff); state->flags.cy = (state->a < opcode[1]); state->flags.ac = ((state->a & 0xf) < (opcode[1] & 0xf)); state->pc += 2; break; } // CPI byte
        case 0xFF: state->sp -= 2; state->memory[state->sp] = (state->pc >> 0) & 0xff; state->memory[state->sp+1] = (state->pc >> 8) & 0xff; state->pc = 0x38; break; // RST 7 // RST 1


        default:
            printf("Unimplemented instruction %02x at PC=%04x", *opcode, state->pc);
            exit(1);
    }
}
