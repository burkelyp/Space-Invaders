#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <typeinfo>
#include <iostream>
#include <cstring>
#include <cstdint>
#include <map>


struct State8080 {
    // 8-bit registers
    uint8_t a, b, c, d, e, h, l;

    // Stack Pointer and Program Counter
    uint16_t sp, pc;

    // Flags
    struct {
        uint8_t    z : 1;   // Zero
        uint8_t    s : 1;   // Sign
        uint8_t    p : 1;   // Parity
        uint8_t    c : 1;   // Carry
        uint8_t    ac : 1;  // Auxiliary Carry
        uint8_t    pad : 3; // Unused bits
    } flags;

    // Interrupt enable
    uint8_t interrupt_enabled;

    // Memory (64KB)
    uint8_t memory[0x10000]; // 65536 bytes
};


void setZSPflags(State8080* cpu, uint8_t result) {
    cpu->flags.z = (result == 0);
    cpu->flags.s = ((result * 0x80) != 0);
    
    int count = 0;
    uint8_t x = result;
    for (int i = 0; i< 8; i++) {
        if (x & 0x01) count++;
        x >>= 1;
    };
    cpu->flags.p = (count % 2 == 0);
}


void Emulate8080Op(State8080* cpu, unsigned char *codebuffer, bool debug = false) {

    uint8_t *code = &codebuffer[cpu->pc];
    if (debug) { printf("%04x ", cpu->pc); }
    switch (*code) {
        case 0x00:
        {
            if (debug) { printf("NOP"); }
            cpu->pc += 1;
            break;
        }
        case 0x01:
        {
            if (debug) { printf("LXI    B, %02x%02x", code[2], code[1]); }
            cpu->b = code[2];
            cpu->c = code[1];
            cpu->pc += 3;
            break;
        }
        case 0x02:
        {
            if (debug) { printf("STAX   B"); }
            uint16_t addr = (cpu->b << 8) | cpu->c;
            cpu->memory[addr] = cpu->a;
            cpu->pc += 1;
            break;
        }
        case 0x03:
        {
            if (debug) { printf("INX    B"); }
            uint16_t bc = (cpu->b << 8) | cpu->c;
            bc += 1;
            cpu->b = (bc >> 8) & 0xFF;
            cpu->c = bc & 0xFF;
            cpu->pc += 1;
            break;
        }
        case 0x04:
        {
            if (debug) { printf("INR    B"); }
            cpu->flags.ac = ((cpu->b & 0x0F) + 1) > 0x0F;
            cpu->b += 1;
            setZSPflags(cpu, cpu->b);
            cpu->pc += 1;
            break;
        }
        case 0x05:
        {
            if (debug) { printf("DCR    B"); }
            cpu->flags.ac = ((cpu->b & 0x0F) - 1) < 0;
            cpu->b -= 1;
            setZSPflags(cpu, cpu->b);
            cpu->pc += 1;
            break;
        }
        case 0x06:
        {
            if (debug) { printf("MVI    B, %02x", code[1]); }
            cpu->b = code[1];
            cpu->pc += 2;
            break;
        }
        case 0x07:
        {
            if (debug) { printf("RLC"); }
            cpu->pc += 1;
            break;
        }
        case 0x08:
        {
            if (debug) { printf("NOP"); }
            cpu->pc += 1;
            break;
        }
        case 0x09:
        {
            if (debug) { printf("DAD    B"); } // Add BC to HL
            uint16_t bc = (cpu->b << 8) | cpu->c;
            uint16_t hl = (cpu->h << 8) | cpu->l;
            uint32_t answer = hl + bc;
            cpu->flags.c = (answer > 0xFFFF);
            cpu->h = (answer >> 8) & 0xFF;
            cpu->l = answer & 0xFF;
            cpu->pc += 1;
            break;
        }
        case 0x0a:
        {
            if (debug) { printf("LDAX   B"); } // Load A indirect
            uint16_t addr = (cpu->b << 8) | cpu->c;
            cpu->a = cpu->memory[addr];
            cpu->pc += 1;
            break;
        }
        case 0x0b:
        {
            if (debug) { printf("DCX    B"); } // Decrement BC
            uint16_t bc = (cpu->b << 8) | cpu->c;
            bc -= 1;
            cpu->b = (bc >> 8) & 0xFF;
            cpu->c = bc & 0xFF;
            cpu->pc += 1;
            break;
        }
        case 0x0c:
        {
            if (debug) { printf("INR    C"); } // Increment C
            cpu->flags.ac = ((cpu->c & 0x0F) + 1) > 0x0F;
            cpu->c += 1;
            setZSPflags(cpu, cpu->c);
            cpu->pc += 1;
            break;
        }
        case 0x0d:
        {
            if (debug) { printf("DCR    C"); } // Decrement C
            cpu->flags.ac = ((cpu->c & 0x0F) - 1) < 0;
            cpu->c -= 1;
            setZSPflags(cpu, cpu->c);
            cpu->pc += 1;
            break;
        }
        case 0x0e:
        {
            if (debug) { printf("MVI    C, %02x", code[1]); } // Move immediate register to C
            cpu->c = code[1];
            cpu->pc += 2;
            break;
        }
        case 0x0f:
        {
            if (debug) { printf("RRC"); } // Rotate A right
            cpu->pc += 1;
            break;
        }
        case 0x10:
        {
            if (debug) { printf("NOP"); }
            cpu->pc += 1;
            break;
        }
        case 0x11:
        {
            if (debug) { printf("LXI    D, %02x%02x", code[2], code[1]); } // Load immediate register pair
            cpu->d = code[2];
            cpu->e = code[1];
            cpu->pc += 3;
            break;
        }
        case 0x12:
        {
            if (debug) { printf("STAX   D"); } // Store A indirect
            uint16_t addr = (cpu->d << 8) | cpu->e;
            cpu->memory[addr] = cpu->a;
            cpu->pc += 1;
            break;
        }
        case 0x13:
        {
            if (debug) { printf("INX    D"); } // Increment DE
            uint16_t de = (cpu->d << 8) | cpu->e;
            de += 1;
            cpu->d = (de >> 8) & 0xFF;
            cpu->e = de & 0xFF;
            cpu->pc += 1;
            break;
        }
        case 0x14:
        {
            if (debug) { printf("INR    D"); } // Increment D
            cpu->flags.ac = ((cpu->d & 0x0F) + 1) > 0x0F;
            cpu->d += 1;
            setZSPflags(cpu, cpu->d);
            break;
        }
        case 0x15:
        {
            if (debug) { printf("DCR    D"); } // Decrement D
            cpu->flags.ac = ((cpu->c & 0x0F) - 1) < 0;
            cpu->c -= 1;
            setZSPflags(cpu, cpu->c);
            cpu->pc += 1;
            break;
        }
        case 0x16:
        {
            if (debug) { printf("MVI    D, %02x", code[1]); } // Move immediate register to D
            cpu->d = code[1];
            cpu->pc += 2;
            break;
        }
        case 0x17:
        {
            if (debug) { printf("RAL"); } // Rotate A left through carry
            cpu->pc += 1;
            break;
        }
        case 0x18:
        {
            if (debug) { printf("NOP"); }
            cpu->pc += 1;
            break;
        }
        case 0x19:
        {
            if (debug) { printf("DAD    D"); } // Add DE to HL
            uint16_t de = (cpu->d << 8) | cpu->e;
            uint16_t hl = (cpu->h << 8) | cpu->l;
            uint32_t answer = hl + de;
            cpu->flags.c = (answer > 0xFFFF);
            cpu->h = (answer >> 8) & 0xFF;
            cpu->l = answer & 0xFF;
            cpu->pc += 1;
            break;
        }
        case 0x1a:
        {
            if (debug) { printf("LDAX   D"); } // Load A indirect
            uint16_t addr = (cpu->d << 8) | cpu->e;
            cpu->a = cpu->memory[addr];
            cpu->pc += 1;
            break;
        }
        case 0x1b:
        {
            if (debug) { printf("DCX    D"); } // Decrement DE
            uint16_t de = (cpu->d << 8) | cpu->e;
            de -= 1;
            cpu->d = (de >> 8) & 0xFF;
            cpu->e = de & 0xFF;
            cpu->pc += 1;
            break;
        }
        case 0x1c:
        {
            if (debug) { printf("INR    E"); } // Increment E
            cpu->flags.ac = ((cpu->e & 0x0F) + 1) > 0x0F;
            cpu->e += 1;
            setZSPflags(cpu, cpu->e);
            cpu->pc += 1;
            break;
        }
        case 0x1d:
        {
            if (debug) { printf("DCR    E"); } // Decrement E
            cpu->flags.ac = ((cpu->e & 0x0F) - 1) < 0;
            cpu->e -= 1;
            setZSPflags(cpu, cpu->e);
            cpu->pc += 1;
            break;
        }
        case 0x1e:
        {
            if (debug) { printf("MVI    E, %02x", code[1]); } // Move immediate register to E
            cpu->e = code[1];
            cpu->pc += 2;
            break;
        }
        case 0x1f:
        {
            /*
            "Rotate Accumulator Right through Carry" (RAR) is a bit-manipulation instruction that shifts the bits of an accumulator register to the right,
            with the least significant bit (LSB) being moved into the carry flag, and the carry flag being moved into the most significant bit (MSB)
            */
            if (debug) { printf("RAR"); } // Rotate A right through carry
            cpu->pc += 1;
            break;
        }
        case 0x20:
        {
            if (debug) { printf("NOP"); }
            cpu->pc += 1;
            break;
        }
        case 0x21:
        {
            if (debug) { printf("LXI    H, %02x%02x", code[2], code[1]); } // Load immediate register pair HL
            cpu->h = code[2];
            cpu->l = code[1];
            cpu->pc += 3;
            break;
        }
        case 0x22:
        {
            uint16_t addr = (code[2] << 8) | code[1];
            if (debug) { printf("SHLD   addr: %04x", addr); } // Store HL direct: from HL to memory
            cpu->memory[addr] = cpu->h;
            cpu->memory[addr+1] = cpu->l;            
            cpu->pc += 3;
            break;
        }
        case 0x23:
        {
            if (debug) { printf("INX    H"); } // Increment HL
            uint16_t hl = (cpu->h << 8) | cpu->l;
            hl += 1;
            cpu->h = (hl >> 8) & 0xFF;
            cpu->l = hl & 0xFF;
            cpu->pc += 1;
            break;
        }
        case 0x24:
        {
            if (debug) { printf("INR    H"); } // Increment H
            cpu->flags.ac = ((cpu->h & 0x0F) + 1) > 0x0F;
            cpu->h += 1;
            setZSPflags(cpu, cpu->h);
            cpu->pc += 1;
            break;
        }
        case 0x25:
        {
            if (debug) { printf("DCR    H"); } // Decrement H
            cpu->flags.ac = ((cpu->h & 0x0F) - 1) < 0;
            cpu->h -= 1;
            setZSPflags(cpu, cpu->h);
            cpu->pc += 1;
            break;
        }
        case 0x26:
        {
            if (debug) { printf("MVI    H, %02x", code[1]); } // Move immediate register to H
            cpu->h = code[1];
            cpu->pc += 2;
            break;
        }
        case 0x27:
        {
            /*
            Decimal Adjust Accumulator. It's an instruction used to convert the result of an 8-bit binary addition of two binary
            coded decimal (BCD) numbers into two valid BCD digits.
            */
            if (debug) { printf("DAA"); } // Decimal adjust A
            cpu->pc += 1;
            break;
        }
        case 0x28:
        {
            if (debug) { printf("NOP"); }
            cpu->pc += 1;
            break;
        }
        case 0x29:
        {
            if (debug) { printf("DAD    H"); } // Add HL to HL
            uint16_t hl = (cpu->h << 8) | cpu->l;
            uint32_t answer = hl + hl;
            cpu->flags.c = (answer > 0xFFFF);
            cpu->h = (answer >> 8) & 0xFF;
            cpu->l = answer & 0xFF;
            cpu->pc += 1;
            break;
        }
        case 0x2a:
        {
            uint16_t addr = (code[2] << 8) | code[1];
            if (debug) { printf("LHLD   addr: %04x", addr); } // Load HL direct: from memory to HL
            cpu->h = cpu->memory[addr];
            cpu->l = cpu->memory[addr + 1];
            cpu->pc += 3;
            break;
        }
        case 0x2b:
        {
            if (debug) { printf("DCX    H"); } // Decrement HL
            uint16_t hl = (cpu->h << 8) | cpu->l;
            hl -= 1;
            cpu->h = (hl >> 8) & 0xFF;
            cpu->l = hl & 0xFF;
            cpu->pc += 1;
            break;
        }
        case 0x2c:
        {
            if (debug) { printf("INR    L"); } // Increment L
            cpu->flags.ac = ((cpu->l & 0x0F) + 1) > 0x0F;
            cpu->l += 1;
            setZSPflags(cpu, cpu->l);
            cpu->pc += 1;
            break;
        }
        case 0x2d:
        {
            if (debug) { printf("DCR    L"); } // Decrement L
            cpu->flags.ac = ((cpu->l & 0x0F) - 1) < 0;
            cpu->l -= 1;
            setZSPflags(cpu, cpu->l);
            cpu->pc += 1;
            break;
        }
        case 0x2e:
        {
            if (debug) { printf("MVI    L, %02x", code[1]); } // Move immediate register to L
            cpu->l = code[1];
            cpu->pc += 2;
            break;
        }
        case 0x2f:
        {
            if (debug) { printf("CMA"); } // Complements the contents of A, assigns to A
            cpu->a = ~cpu->a;
            cpu->pc += 1;
            break;
        }
        case 0x30:
        {
            if (debug) { printf("NOP"); }
            cpu->pc += 1;
            break;
        }
        case 0x31:
        {
            if (debug) { printf("LXI    SP, %02x%02x", code[2], code[1]); } // Load immediate to stack pointer
            cpu->sp = (code[2] << 8) | code[1];
            cpu->pc += 3;
            break;
        }
        case 0x32:
        {
            uint16_t addr = (code[2] << 8) | code[1];
            if (debug) { printf("STA    addr: %04x", addr); } // Store A direct: from A to memory
            cpu->memory[addr] = cpu->a;
            cpu->pc += 3;
            break;
        }
        case 0x33:
        {
            if (debug) { printf("INX    SP"); } // Increment stack pointer
            cpu->sp += 1;
            cpu->pc += 1;
            break;
        }
        case 0x34:
        {
            if (debug) { printf("INR    M"); } // Increment value stored in memory at HL
            uint16_t addr = (cpu->h << 8) | cpu->l;
            uint8_t value = cpu->memory[addr];
            cpu->flags.ac = ((value & 0x0F) + 1) > 0x0F;
            value += 1;
            cpu->memory[addr] = value;
            setZSPflags(cpu, value);
            cpu->pc += 1;
            break;
        }
        case 0x35:
        {
            if (debug) { printf("DCR    M"); } // Decrement value in memory at HL
            uint16_t addr = (cpu->h << 8) | cpu->l;
            uint8_t value = cpu->memory[addr]; 
            cpu->flags.ac = ((value & 0x0F) - 1) < 0;
            value -= 1;
            cpu->memory[addr] = value;
            setZSPflags(cpu, value);
            cpu->pc += 1;
            break;
        }
        case 0x36:
        {
            if (debug) { printf("MVI    M, %02x", code[1]); } // Move immediate to memory (memory designated by HL)
            cpu->pc += 2;
            break;
        }
        case 0x37:
        {
            if (debug) { printf("STC"); } // Sets the Carry flag bit in the status register to 1
            cpu->flags.c = 1;
            cpu->pc += 1;
            break;
        }
        case 0x38:
        {
            if (debug) { printf("NOP"); }
            cpu->pc += 1;
            break;
        }
        case 0x39:
        {
            if (debug) { printf("DAD    SP"); } // Add stack pointer to HL (memory)
            uint16_t hl = (cpu->h << 8) | cpu->l;
            uint32_t answer = hl + cpu->sp;
            cpu->flags.c = (answer > 0xFFFF);
            cpu->h = (answer >> 8) & 0xFF;
            cpu->l = answer & 0xFF;
            cpu->pc += 1;
            break;
        }
        case 0x3a:
        {
            uint16_t addr = (code[2] << 8) | code[1];
            if (debug) { printf("LDA    addr: %04x", addr); } // Load into A direct from memory
            cpu->a = cpu->memory[addr];
            cpu->pc += 3;
            break;
        }
        case 0x3b:
        {
            if (debug) { printf("DCX    SP"); } // Decrement SP
            cpu->sp -= 1;
            cpu->pc += 1;
            break;
        }
        case 0x3c:
        {
            if (debug) { printf("INR    A"); } // Increment A
            cpu->flags.ac = ((cpu->a & 0x0F) + 1) > 0x0F;
            cpu->a += 1;
            setZSPflags(cpu, cpu->a);
            cpu->pc += 1;
            break;
        }
        case 0x3d:
        {
            if (debug) { printf("DCR    A"); } // Decrement A
            cpu->flags.ac = ((cpu->a & 0x0F) - 1) < 0;
            cpu->a -= 1;
            setZSPflags(cpu, cpu->a);
            cpu->pc += 1;
            break;
        }
        case 0x3e:
        {
            if (debug) { printf("MVI    A, %02x", code[1]); } // Move immediate register to A
            cpu->a = code[1];
            cpu->pc += 2;
            break;
        }
        case 0x3f:
        {
            if (debug) { printf("CMC"); } // Complement carry: flips value of the carry flag
            if (cpu->flags.c == 0) { cpu->flags.c = 1; }
            else { cpu->flags.c = 0; }
            cpu->pc += 1;
            break;
        }
        case 0x40:
        {
            if (debug) { printf("MOV    B, B"); } // Move register B to register B
            cpu->b = cpu->b;
            cpu->pc += 1;
            break;
        }
        case 0x41:
        {
            if (debug) { printf("MOV    B, C"); } // Move register C to register B
            cpu->b = cpu->c;
            cpu->pc += 1;
            break;
        }
        case 0x42:
        {
            if (debug) { printf("MOV    B, D"); } // Move register D to register B
            cpu->b = cpu->d;
            cpu->pc += 1;
            break;
        }
        case 0x43:
        {
            if (debug) { printf("MOV    B, E"); } // Move register E to register B
            cpu->b = cpu->e;
            cpu->pc += 1;
            break;
        }
        case 0x44:
        {
            if (debug) { printf("MOV    B, H"); } // Move register H to register B
            cpu->b = cpu->h;
            cpu->pc += 1;
            break;
        }
        case 0x45:
        {
            if (debug) { printf("MOV    B, L"); } // Move register L to register B
            cpu->b = cpu->l;
            cpu->pc += 1;
            break;
        }
        case 0x46:
        {
            if (debug) { printf("MOV    B, M"); } // Move value at memory (HL) to register B
            cpu->b = cpu->memory[(cpu->h << 8) | cpu->l];
            cpu->pc += 1;
            break;
        }
        case 0x47:
        {
            if (debug) { printf("MOV    B, A"); } // Move register A to register B
            cpu->b = cpu->a;
            cpu->pc += 1;
            break;
        }
        case 0x48:
        {
            if (debug) { printf("MOV    C, B"); }
            cpu->c = cpu->b;
            cpu->pc += 1;
            break;
        }
        case 0x49:
        {
            if (debug) { printf("MOV    C, C"); }
            cpu->c = cpu->c;
            cpu->pc += 1;
            break;
        }
        case 0x4a:
        {
            if (debug) { printf("MOV    C, D"); }
            cpu->c = cpu->d;
            cpu->pc += 1;
            break;
        }
        case 0x4b:
        {
            if (debug) { printf("MOV    C, E"); }
            cpu->c = cpu->e;
            cpu->pc += 1;
            break;
        }
        case 0x4c:
        {
            if (debug) { printf("MOV    C, H"); }
            cpu->c = cpu->h;
            cpu->pc += 1;
            break;
        }
        case 0x4d:
        {
            if (debug) { printf("MOV    C, L"); }
            cpu->c = cpu->l;
            cpu->pc += 1;
            break;
        }
        case 0x4e:
        {
            if (debug) { printf("MOV    C, M"); }
            cpu->c = cpu->memory[(cpu->h << 8) | cpu->l];
            cpu->pc += 1;
            break;
        }
        case 0x4f:
        {
            if (debug) { printf("MOV    C, A"); }
            cpu->c = cpu->a;
            cpu->pc += 1;
            break;
        }
        case 0x50:
        {
            if (debug) { printf("MOV    D, B"); }
            cpu->d = cpu->b;
            cpu->pc += 1;
            break;
        }
        case 0x51:
        {
            if (debug) { printf("MOV    D, C"); }
            cpu->d = cpu->c;
            cpu->pc += 1;
            break;
        }
        case 0x52:
        {
            if (debug) { printf("MOV    D, D"); }
            cpu->d = cpu->d;
            cpu->pc += 1;
            break;
        }
        case 0x53:
        {
            if (debug) { printf("MOV    D, E"); }
            cpu->d = cpu->e;
            cpu->pc += 1;
            break;
        }
        case 0x54:
        {
            if (debug) { printf("MOV    D, H"); }
            cpu->d = cpu->h;
            cpu->pc += 1;
            break;
        }
        case 0x55:
        {
            if (debug) { printf("MOV    D, L"); }
            cpu->d = cpu->l;
            cpu->pc += 1;
            break;
        }
        case 0x56:
        {
            if (debug) { printf("MOV    D, M"); }
            cpu->d = cpu->memory[(cpu->h << 8) | cpu->l];
            cpu->pc += 1;
            break;
        }
        case 0x57:
        {
            if (debug) { printf("MOV    D, A"); }
            cpu->d = cpu->a;
            cpu->pc += 1;
            break;
        }
        case 0x58:
        {
            if (debug) { printf("MOV    E, B"); }
            cpu->e = cpu->b;
            cpu->pc += 1;
            break;
        }
        case 0x59:
        {
            if (debug) { printf("MOV    E, C"); }
            cpu->e = cpu->c;
            cpu->pc += 1;
            break;
        }
        case 0x5a:
        {
            if (debug) { printf("MOV    E, D"); }
            cpu->e = cpu->d;
            cpu->pc += 1;
            break;
        }
        case 0x5b:
        {
            if (debug) { printf("MOV    E, E"); }
            cpu->e = cpu->e;
            cpu->pc += 1;
            break;
        }
        case 0x5c:
        {
            if (debug) { printf("MOV    E, H"); }
            cpu->e = cpu->h;
            cpu->pc += 1;
            break;
        }
        case 0x5d:
        {
            if (debug) { printf("MOV    E, L"); }
            cpu->e = cpu->l;
            cpu->pc += 1;
            break;
        }
        case 0x5e:
        {
            if (debug) { printf("MOV    E, M"); }
            cpu->e = cpu->memory[(cpu->h << 8) | cpu->l];
            cpu->pc += 1;
            break;
        }
        case 0x5f:
        {
            if (debug) { printf("MOV    E, A"); }
            cpu->e = cpu->a;
            cpu->pc += 1;
            break;
        }
        case 0x60:
        {
            if (debug) { printf("MOV    H, B"); }
            cpu->h = cpu->b;
            cpu->pc += 1;
            break;
        }
        case 0x61:
        {
            if (debug) { printf("MOV    H, C"); }
            cpu->h = cpu->c;
            cpu->pc += 1;
            break;
        }
        case 0x62:
        {
            if (debug) { printf("MOV    H, D"); }
            cpu->h = cpu->d;
            cpu->pc += 1;
            break;
        }
        case 0x63:
        {
            if (debug) { printf("MOV    H, E"); }
            cpu->h = cpu->e;
            cpu->pc += 1;
            break;
        }
        case 0x64:
        {
            if (debug) { printf("MOV    H, H"); }
            cpu->h = cpu->h;
            cpu->pc += 1;
            break;
        }
        case 0x65:
        {
            if (debug) { printf("MOV    H, L"); }
            cpu->h = cpu->l;
            cpu->pc += 1;
            break;
        }
        case 0x66:
        {
            if (debug) { printf("MOV    H, M"); }
            cpu->h = cpu->memory[(cpu->h << 8) | cpu->l];
            cpu->pc += 1;
            break;
        }
        case 0x67:
        {
            if (debug) { printf("MOV    H, A"); }
            cpu->h = cpu->a;
            cpu->pc += 1;
            break;
        }
        case 0x68:
        {
            if (debug) { printf("MOV    L, B"); }
            cpu->l = cpu->b;
            cpu->pc += 1;
            break;
        }
        case 0x69:
        {
            if (debug) { printf("MOV    L, C"); }
            cpu->l = cpu->c;
            cpu->pc += 1;
            break;
        }
        case 0x6a:
        {
            if (debug) { printf("MOV    L, D"); }
            cpu->l = cpu->d;
            cpu->pc += 1;
            break;
        }
        case 0x6b:
        {
            if (debug) { printf("MOV    L, E"); }
            cpu->l = cpu->e;
            cpu->pc += 1;
            break;
        }
        case 0x6c:
        {
            if (debug) { printf("MOV    L, H"); }
            cpu->l = cpu->h;
            cpu->pc += 1;
            break;
        }
        case 0x6d:
        {
            if (debug) { printf("MOV    L, L"); }
            cpu->l = cpu->l;
            cpu->pc += 1;
            break;
        }
        case 0x6e:
        {
            if (debug) { printf("MOV    L, M"); }
            cpu->l = cpu->memory[(cpu->h << 8) | cpu->l];
            cpu->pc += 1;
            break;
        }
        case 0x6f:
        {
            if (debug) { printf("MOV    L, A"); }
            cpu->l = cpu->a;
            cpu->pc += 1;
            break;
        }
        case 0x70:
        {
            if (debug) { printf("MOV    M, B"); }
            cpu->memory[(cpu->h << 8) | cpu->l] = cpu->b;
            cpu->pc += 1;
            break;
        }
        case 0x71:
        {
            if (debug) { printf("MOV    M, C"); }
            cpu->memory[(cpu->h << 8) | cpu->l] = cpu->c;
            cpu->pc += 1;
            break;
        }
        case 0x72:
        {
            if (debug) { printf("MOV    M, D"); }
            cpu->memory[(cpu->h << 8) | cpu->l] = cpu->d;
            cpu->pc += 1;
            break;
        }
        case 0x73:
        {
            if (debug) { printf("MOV    M, E"); }
            cpu->memory[(cpu->h << 8) | cpu->l] = cpu->e;
            cpu->pc += 1;
            break;
        }
        case 0x74:
        {
            if (debug) { printf("MOV    M, H"); }
            cpu->memory[(cpu->h << 8) | cpu->l] = cpu->h;
            cpu->pc += 1;
            break;
        }
        case 0x75:
        {
            if (debug) { printf("MOV    M, L"); }
            cpu->memory[(cpu->h << 8) | cpu->l] = cpu->l;
            cpu->pc += 1;
            break;
        }
        case 0x76:
        {
            /*
            tops the CPU's execution until an external interrupt or reset signal is received.
            The CPU will remain in a waiting state, essentially doing nothing, until an external event signals it to resume operation. 
            */
            if (debug) { printf("HLT"); } // Halt
            break;
        }
        case 0x77:
        {
            if (debug) { printf("MOV    M, A"); }
            cpu->memory[(cpu->h << 8) | cpu->l] = cpu->a;
            cpu->pc += 1;
            break;
        }
        case 0x78:
        {
            if (debug) { printf("MOV    A, B"); }
            cpu->a = cpu->b;
            cpu->pc += 1;
            break;
        }
        case 0x79:
        {
            if (debug) { printf("MOV    A, C"); }
            cpu->a = cpu->c;
            cpu->pc += 1;
            break;
        }
        case 0x7a:
        {
            if (debug) { printf("MOV    A, D"); }
            cpu->a = cpu->d;
            cpu->pc += 1;
            break;
        }
        case 0x7b:
        {
            if (debug) { printf("MOV    A, E"); }
            cpu->a = cpu->e;
            cpu->pc += 1;
            break;
        }
        case 0x7c:
        {
            if (debug) { printf("MOV    A, H"); }
            cpu->a = cpu->h;
            cpu->pc += 1;
            break;
        }
        case 0x7d:
        {
            if (debug) { printf("MOV    A, L"); }
            cpu->a = cpu->l;
            cpu->pc += 1;
            break;
        }
        case 0x7e:
        {
            if (debug) { printf("MOV    A, M"); }
            cpu->a = cpu->memory[(cpu->h << 8) | cpu->l];
            cpu->pc += 1;
            break;
        }
        case 0x7f:
        {
            if (debug) { printf("MOV    A, A"); }
            cpu->a = cpu->a;
            cpu->pc += 1;
            break;
        }
        case 0x80:
        {
            if (debug) { printf("ADD    B"); } // Adds contents of register B to register A
            uint16_t answer = cpu->a + cpu->b;
            cpu->flags.c = (answer > 0xFF);
            cpu->flags.ac = ((cpu->a & 0x0F) + (cpu->b & 0x0F)) > 0x0F; // Auxiliary carry
            cpu->a = answer & 0xFF;
            setZSPflags(cpu, cpu->a);
            cpu->pc += 1;
            break;
        }
        case 0x81:
        {
            if (debug) { printf("ADD    C"); } // Adds contents of register C to register A
            uint16_t answer = cpu->a + cpu->c;
            cpu->flags.c = (answer > 0xFF);
            cpu->flags.ac = ((cpu->a & 0x0F) + (cpu->c & 0x0F)) > 0x0F; // Auxiliary carry
            cpu->a = answer & 0xFF;
            setZSPflags(cpu, cpu->a);
            cpu->pc += 1;
            break;
        }
        case 0x82:
        {
            if (debug) { printf("ADD    D"); } // Adds contents of register D to register A
            uint16_t answer = cpu->a + cpu->d;
            cpu->flags.c = (answer > 0xFF);
            cpu->flags.ac = ((cpu->a & 0x0F) + (cpu->d & 0x0F)) > 0x0F; // Auxiliary carry
            cpu->a = answer & 0xFF;
            setZSPflags(cpu, cpu->a);
            cpu->pc += 1;
            break;
        }
        case 0x83:
        {
            if (debug) { printf("ADD    E"); } // Adds contents of register E to register A
            uint16_t answer = cpu->a + cpu->e;
            cpu->flags.c = (answer > 0xFF);
            cpu->flags.ac = ((cpu->a & 0x0F) + (cpu->e & 0x0F)) > 0x0F; // Auxiliary carry
            cpu->a = answer & 0xFF;
            setZSPflags(cpu, cpu->a);
            cpu->pc += 1;
            break;
        }
        case 0x84:
        {
            if (debug) { printf("ADD    H"); } // Adds contents of register H to register A
            uint16_t answer = cpu->a + cpu->h;
            cpu->flags.c = (answer > 0xFF);
            cpu->flags.ac = ((cpu->a & 0x0F) + (cpu->h & 0x0F)) > 0x0F; // Auxiliary carry
            cpu->a = answer & 0xFF;
            setZSPflags(cpu, cpu->a);
            cpu->pc += 1;
            break;
        }
        case 0x85:
        {
            if (debug) { printf("ADD    L"); } // Adds contents of register L to register A
            uint16_t answer = cpu->a + cpu->l;
            cpu->flags.c = (answer > 0xFF);
            cpu->flags.ac = ((cpu->a & 0x0F) + (cpu->l & 0x0F)) > 0x0F; // Auxiliary carry
            cpu->a = answer & 0xFF;
            setZSPflags(cpu, cpu->a);
            cpu->pc += 1;
            break;
        }
        case 0x86:
        {
            if (debug) { printf("ADD    M"); } // Adds contents of register M to register A
            uint16_t addr = (cpu->h << 8) | cpu->l;
            uint8_t value = cpu->memory[addr];
            uint16_t answer = cpu->a + value;
            cpu->flags.c = (answer > 0xFF);
            cpu->flags.ac = ((cpu->a & 0x0F) + (value & 0x0F)) > 0x0F; // Auxiliary carry
            cpu->a = answer & 0xFF;
            setZSPflags(cpu, cpu->a);
            cpu->pc += 1;
            break;
        }
        case 0x87:
        {
            if (debug) { printf("ADD    A"); } // Adds contents of register A to register A
            uint16_t answer = cpu->a + cpu->a;
            cpu->flags.c = (answer > 0xFF);
            cpu->flags.ac = ((cpu->a & 0x0F) + (cpu->a & 0x0F)) > 0x0F; // Auxiliary carry
            cpu->a = answer & 0xFF;
            setZSPflags(cpu, cpu->a);
            cpu->pc += 1;
            break;
        }
        case 0x88:
        {
            if (debug) { printf("ADC    B"); } // Adds contents of register B to register A with carry
            uint16_t answer = cpu->a + cpu->b + cpu->flags.c;
            cpu->flags.c = (answer > 0xFF);
            cpu->flags.ac = ((cpu->a & 0x0F) + (cpu->b & 0x0F) + cpu->flags.c) > 0x0F; // Auxiliary carry
            cpu->a = answer & 0xFF;
            setZSPflags(cpu, cpu->a);
            cpu->pc += 1;
            break;
        }
        case 0x89:
        {
            if (debug) { printf("ADC    C"); } // Adds contents of register C to register A with carry
            uint16_t answer = cpu->a + cpu->c + cpu->flags.c;
            cpu->flags.c = (answer > 0xFF);
            cpu->flags.ac = ((cpu->a & 0x0F) + (cpu->c & 0x0F) + cpu->flags.c) > 0x0F; // Auxiliary carry
            cpu->a = answer & 0xFF;
            setZSPflags(cpu, cpu->a);
            cpu->pc += 1;
            break;
        }
        case 0x8a:
        {
            if (debug) { printf("ADC    D"); } // Adds contents of register D to register A with carry
            uint16_t answer = cpu->a + cpu->d + cpu->flags.c;
            cpu->flags.c = (answer > 0xFF);
            cpu->flags.ac = ((cpu->a & 0x0F) + (cpu->d & 0x0F) + cpu->flags.c) > 0x0F; // Auxiliary carry
            cpu->a = answer & 0xFF;
            setZSPflags(cpu, cpu->a);
            cpu->pc += 1;
            break;
        }
        case 0x8b:
        {
            if (debug) { printf("ADC    E"); } // Adds contents of register E to register A with carry
            uint16_t answer = cpu->a + cpu->e + cpu->flags.c;
            cpu->flags.c = (answer > 0xFF);
            cpu->flags.ac = ((cpu->a & 0x0F) + (cpu->e & 0x0F) + cpu->flags.c) > 0x0F; // Auxiliary carry
            cpu->a = answer & 0xFF;
            setZSPflags(cpu, cpu->a);
            cpu->pc += 1;
            break;
        }
        case 0x8c:
        {
            if (debug) { printf("ADC    H"); } // Adds contents of register H to register A with carry
            uint16_t answer = cpu->a + cpu->h + cpu->flags.c;
            cpu->flags.c = (answer > 0xFF);
            cpu->flags.ac = ((cpu->a & 0x0F) + (cpu->h & 0x0F) + cpu->flags.c) > 0x0F; // Auxiliary carry
            cpu->a = answer & 0xFF;
            setZSPflags(cpu, cpu->a);
            cpu->pc += 1;
            break;
        }
        case 0x8d:
        {
            if (debug) { printf("ADC    L"); } // Adds contents of register L to register A with carry
            uint16_t answer = cpu->a + cpu->l + cpu->flags.c;
            cpu->flags.c = (answer > 0xFF);
            cpu->flags.ac = ((cpu->a & 0x0F) + (cpu->l & 0x0F) + cpu->flags.c) > 0x0F; // Auxiliary carry
            cpu->a = answer & 0xFF;
            setZSPflags(cpu, cpu->a);
            cpu->pc += 1;
            break;
        }
        case 0x8e:
        {
            if (debug) { printf("ADC    M"); } // Adds contents of register M to register A with carry
            uint16_t addr = (cpu->h << 8) | cpu->l;
            uint8_t value = cpu->memory[addr];
            uint16_t answer = cpu->a + value + cpu->flags.c;
            cpu->flags.c = (answer > 0xFF);
            cpu->flags.ac = ((cpu->a & 0x0F) + (value & 0x0F) + cpu->flags.c) > 0x0F; // Auxiliary carry
            cpu->a = answer & 0xFF;
            setZSPflags(cpu, cpu->a);
            cpu->pc += 1;
            break;
        }
        case 0x8f:
        {
            if (debug) { printf("ADC    A"); } // Adds contents of register A to register A with carry
            uint16_t answer = cpu->a + cpu->a + cpu->flags.c;
            cpu->flags.c = (answer > 0xFF);
            cpu->flags.ac = ((cpu->a & 0x0F) + (cpu->a & 0x0F) + cpu->flags.c) > 0x0F; // Auxiliary carry
            cpu->a = answer & 0xFF;
            setZSPflags(cpu, cpu->a);
            cpu->pc += 1;
            break;
        }
        case 0x90:
        {
            if (debug) { printf("SUB    B"); } // Subtracts contents of register B from register A
            uint16_t answer = (uint16_t)cpu->a - (uint16_t)cpu->b;
            cpu->flags.ac = (cpu->a & 0x0F) < (cpu->b & 0x0F);
            cpu->flags.c = (cpu->a < cpu->b);
            cpu->a = answer & 0xFF;
            setZSPflags(cpu, cpu->a);
            cpu->pc += 1;
            break;
        }
        case 0x91:
        {
            if (debug) { printf("SUB    C"); } // Subtracts contents of register C from register A
            uint16_t answer = (uint16_t)cpu->a - (uint16_t)cpu->c;
            cpu->flags.ac = (cpu->a & 0x0F) < (cpu->c & 0x0F);
            cpu->flags.c = (cpu->a < cpu->c);
            cpu->a = answer & 0xFF;
            setZSPflags(cpu, cpu->a);
            cpu->pc += 1;
            break;
        }
        case 0x92:
        {
            if (debug) { printf("SUB    D"); } // Subtracts contents of register D from register A
            uint16_t answer = (uint16_t)cpu->a - (uint16_t)cpu->d;
            cpu->flags.ac = (cpu->a & 0x0F) < (cpu->d & 0x0F);
            cpu->flags.c = (cpu->a < cpu->d);
            cpu->a = answer & 0xFF;
            setZSPflags(cpu, cpu->a);
            cpu->pc += 1;
            break;
        }
        case 0x93:
        {
            if (debug) { printf("SUB    E"); } // Subtracts contents of register E from register A
            uint16_t answer = (uint16_t)cpu->a - (uint16_t)cpu->e;
            cpu->flags.ac = (cpu->a & 0x0F) < (cpu->e & 0x0F);
            cpu->flags.c = (cpu->a < cpu->e);
            cpu->a = answer & 0xFF;
            setZSPflags(cpu, cpu->a);
            cpu->pc += 1;
            break;
        }
        case 0x94:
        {
            if (debug) { printf("SUB    H"); } // Subtracts contents of register H from register A
            uint16_t answer = (uint16_t)cpu->a - (uint16_t)cpu->h;
            cpu->flags.ac = (cpu->a & 0x0F) < (cpu->h & 0x0F);
            cpu->flags.c = (cpu->a < cpu->h);
            cpu->a = answer & 0xFF;
            setZSPflags(cpu, cpu->a);
            cpu->pc += 1;
            break;
        }
        case 0x95:
        {
            if (debug) { printf("SUB    L"); } // Subtracts contents of register L from register A
            uint16_t answer = (uint16_t)cpu->a - (uint16_t)cpu->l;
            cpu->flags.ac = (cpu->a & 0x0F) < (cpu->l & 0x0F);
            cpu->flags.c = (cpu->a < cpu->l);
            cpu->a = answer & 0xFF;
            setZSPflags(cpu, cpu->a);
            cpu->pc += 1;
            break;
        }
        case 0x96:
        {
            if (debug) { printf("SUB    M"); } // Subtracts contents of memory[HL] from register A
            uint16_t addr = (cpu->h << 8) | cpu->l;
            uint16_t value = cpu->memory[addr];
            uint16_t answer = (uint16_t)cpu->a - value;
            cpu->flags.ac = (cpu->a & 0x0F) < (value & 0x0F);
            cpu->flags.c = (cpu->a < value);
            cpu->a = answer & 0xFF;
            setZSPflags(cpu, cpu->a);
            cpu->pc += 1;
            break;
        }
        case 0x97:
        {
            if (debug) { printf("SUB    A"); } // Subtracts contents of register A from register A
            uint16_t answer = (uint16_t)cpu->a - (uint16_t)cpu->a;
            cpu->flags.ac = (cpu->a & 0x0F) < (cpu->a & 0x0F);
            cpu->flags.c = (cpu->a < cpu->a);
            cpu->a = answer & 0xFF;
            setZSPflags(cpu, cpu->a);
            cpu->pc += 1;
            break;
        }
        case 0x98:
        {
            if (debug) { printf("SBB    B"); } // Subtracts contents of register B from register A with borrow
            uint16_t answer = (uint16_t)cpu->a - (uint16_t)cpu->b - cpu->flags.c;
            cpu->flags.ac = (cpu->a & 0x0F) < ((cpu->b + cpu->flags.c) & 0x0F);
            cpu->flags.c = (cpu->a < (cpu->b + cpu->flags.c));
            cpu->a = answer & 0xFF;
            setZSPflags(cpu, cpu->a);
            cpu->pc += 1;
            break;
        }
        case 0x99:
        {
            if (debug) { printf("SBB    C"); } // Subtracts contents of register C from register A with borrow
            uint16_t answer = (uint16_t)cpu->a - (uint16_t)cpu->c - cpu->flags.c;
            cpu->flags.ac = (cpu->a & 0x0F) < ((cpu->c + cpu->flags.c) & 0x0F);
            cpu->flags.c = (cpu->a < (cpu->c + cpu->flags.c));
            cpu->a = answer & 0xFF;
            setZSPflags(cpu, cpu->a);
            cpu->pc += 1;
            break;
        }
        case 0x9a:
        {
            if (debug) { printf("SBB    D"); } // Subtracts contents of register D from register A with borrow
            uint16_t answer = (uint16_t)cpu->a - (uint16_t)cpu->d - cpu->flags.c;
            cpu->flags.ac = (cpu->a & 0x0F) < ((cpu->d + cpu->flags.c) & 0x0F);
            cpu->flags.c = (cpu->a < (cpu->d + cpu->flags.c));
            cpu->a = answer & 0xFF;
            setZSPflags(cpu, cpu->a);
            cpu->pc += 1;
            break;
        }
        case 0x9b:
        {
            if (debug) { printf("SBB    E"); } // Subtracts contents of register E from register A with borrow
            uint16_t answer = (uint16_t)cpu->a - (uint16_t)cpu->e - cpu->flags.c;
            cpu->flags.ac = (cpu->a & 0x0F) < ((cpu->e + cpu->flags.c) & 0x0F);
            cpu->flags.c = (cpu->a < (cpu->e + cpu->flags.c));
            cpu->a = answer & 0xFF;
            setZSPflags(cpu, cpu->a);
            cpu->pc += 1;
            break;
        }
        case 0x9c:
        {
            if (debug) { printf("SBB    H"); } // Subtracts contents of register H from register A with borrow
            uint16_t answer = (uint16_t)cpu->a - (uint16_t)cpu->h - cpu->flags.c;
            cpu->flags.ac = (cpu->a & 0x0F) < ((cpu->h + cpu->flags.c) & 0x0F);
            cpu->flags.c = (cpu->a < (cpu->h + cpu->flags.c));
            cpu->a = answer & 0xFF;
            setZSPflags(cpu, cpu->a);
            cpu->pc += 1;
            break;
        }
        case 0x9d:
        {
            if (debug) { printf("SBB    L"); } // Subtracts contents of register L from register A with borrow
            uint16_t answer = (uint16_t)cpu->a - (uint16_t)cpu->l - cpu->flags.c;
            cpu->flags.ac = (cpu->a & 0x0F) < ((cpu->l + cpu->flags.c) & 0x0F);
            cpu->flags.c = (cpu->a < (cpu->l + cpu->flags.c));
            cpu->a = answer & 0xFF;
            setZSPflags(cpu, cpu->a);
            cpu->pc += 1;
            break;
        }
        case 0x9e:
        {
            if (debug) { printf("SBB    M"); } // Subtracts contents of register M from register A with borrow
            uint16_t addr = (cpu->h << 8) | cpu->l;
            uint16_t value = cpu->memory[addr];
            uint16_t answer = (uint16_t)cpu->a - value - cpu->flags.c;
            cpu->flags.ac = (cpu->a & 0x0F) < ((value + cpu->flags.c) & 0x0F);
            cpu->flags.c = (cpu->a < (value + cpu->flags.c));
            cpu->a = answer & 0xFF;
            setZSPflags(cpu, cpu->a);
            cpu->pc += 1;
            break;
        }
        case 0x9f:
        {
            if (debug) { printf("SBB    A"); } // Subtracts contents of register A from register A with borrow
            uint16_t answer = (uint16_t)cpu->a - (uint16_t)cpu->a - cpu->flags.c;
            cpu->flags.ac = (cpu->a & 0x0F) < ((cpu->a + cpu->flags.c) & 0x0F);
            cpu->flags.c = (cpu->a < (cpu->a + cpu->flags.c));
            cpu->a = answer & 0xFF;
            setZSPflags(cpu, cpu->a);
            cpu->pc += 1;
            break;
        }
        case 0xa0:
        {
            if (debug) { printf("ANA    B"); } // Register B AND register A
            cpu->a = cpu->a & cpu->b;
            cpu->flags.c = 0;
            cpu->flags.ac = ((cpu->a | cpu->b) & 0x08) != 0; // Not sure if this is correct, but space-invaders does not use auxiliary carry
            setZSPflags(cpu, cpu->a);
            cpu->pc += 1;
            break;
        }
        case 0xa1:
        {
            if (debug) { printf("ANA    C"); } // Register C AND register A
            cpu->a = cpu->a & cpu->c;
            cpu->flags.c = 0;
            cpu->flags.ac = ((cpu->a | cpu->c) & 0x08) != 0; // Not sure if this is correct, but space-invaders does not use auxiliary carry
            setZSPflags(cpu, cpu->a);
            cpu->pc += 1;
            break;
        }
        case 0xa2:
        {
            if (debug) { printf("ANA    D"); } // Register D AND register A
            cpu->a = cpu->a & cpu->d;
            cpu->flags.c = 0;
            cpu->flags.ac = ((cpu->a | cpu->d) & 0x08) != 0; // Not sure if this is correct, but space-invaders does not use auxiliary carry
            setZSPflags(cpu, cpu->a);
            cpu->pc += 1;
            break;
        }
        case 0xa3:
        {
            if (debug) { printf("ANA    E"); } // Register E AND register A
            cpu->a = cpu->a & cpu->e;
            cpu->flags.c = 0;
            cpu->flags.ac = ((cpu->a | cpu->e) & 0x08) != 0; // Not sure if this is correct, but space-invaders does not use auxiliary carry
            setZSPflags(cpu, cpu->a);
            cpu->pc += 1;
            break;
        }
        case 0xa4:
        {
            if (debug) { printf("ANA    H"); } // Register H AND register A
            cpu->a = cpu->a & cpu->h;
            cpu->flags.c = 0;
            cpu->flags.ac = ((cpu->a | cpu->h) & 0x08) != 0; // Not sure if this is correct, but space-invaders does not use auxiliary carry
            setZSPflags(cpu, cpu->a);
            cpu->pc += 1;
            break;
        }
        case 0xa5:
        {
            if (debug) { printf("ANA    L"); } // Register L AND register A
            cpu->a = cpu->a & cpu->l;
            cpu->flags.c = 0;
            cpu->flags.ac = ((cpu->a | cpu->l) & 0x08) != 0; // Not sure if this is correct, but space-invaders does not use auxiliary carry
            setZSPflags(cpu, cpu->a);
            cpu->pc += 1;
            break;
        }
        case 0xa6:
        {
            if (debug) { printf("ANA    M"); } // Register M AND register A
            uint16_t addr = (cpu->h << 8) | cpu->l;
            uint8_t value = cpu->memory[addr];
            cpu->a = cpu->a & value;
            cpu->flags.c = 0;
            cpu->flags.ac = ((cpu->a | value) & 0x08) != 0; // Not sure if this is correct, but space-invaders does not use auxiliary carry
            setZSPflags(cpu, cpu->a);
            cpu->pc += 1;
            break;
        }
        case 0xa7:
        {
            if (debug) { printf("ANA    A"); } // Register A AND register A
            cpu->a = cpu->a & cpu->a;
            cpu->flags.c = 0;
            cpu->flags.ac = ((cpu->a | cpu->a) & 0x08) != 0; // Not sure if this is correct, but space-invaders does not use auxiliary carry
            setZSPflags(cpu, cpu->a);
            cpu->pc += 1;
            break;
        }
        case 0xa8:
        {
            if (debug) { printf("XRA    B"); } // Register B OR register A (exclusive)
            cpu->a = cpu->a ^ cpu->b;
            cpu->flags.c = 0;
            cpu->flags.ac = 0;
            setZSPflags(cpu, cpu->a);
            cpu->pc += 1;
            break;
        }
        case 0xa9:
        {
            if (debug) { printf("XRA    C"); } // Register B OR register A (exclusive)
            cpu->a = cpu->a ^ cpu->c;
            cpu->flags.c = 0;
            cpu->flags.ac = 0;
            setZSPflags(cpu, cpu->a);
            cpu->pc += 1;
            break;
        }
        case 0xaa:
        {
            if (debug) { printf("XRA    D"); } // Register B OR register A (exclusive)
            cpu->a = cpu->a ^ cpu->d;
            cpu->flags.c = 0;
            cpu->flags.ac = 0;
            setZSPflags(cpu, cpu->a);
            cpu->pc += 1;
            break;
        }
        case 0xab:
        {
            if (debug) { printf("XRA    E"); } // Register B OR register A (exclusive)
            cpu->a = cpu->a ^ cpu->e;
            cpu->flags.c = 0;
            cpu->flags.ac = 0;
            setZSPflags(cpu, cpu->a);
            cpu->pc += 1;
            break;
        }
        case 0xac:
        {
            if (debug) { printf("XRA    H"); } // Register B OR register A (exclusive)
            cpu->a = cpu->a ^ cpu->h;
            cpu->flags.c = 0;
            cpu->flags.ac = 0;
            setZSPflags(cpu, cpu->a);
            cpu->pc += 1;
            break;
        }
        case 0xad:
        {
            if (debug) { printf("XRA    L"); } // Register B OR register A (exclusive)
            cpu->a = cpu->a ^ cpu->l;
            cpu->flags.c = 0;
            cpu->flags.ac = 0;
            setZSPflags(cpu, cpu->a);
            cpu->pc += 1;
            break;
        }
        case 0xae:
        {
            if (debug) { printf("XRA    M"); } // Register B OR register A (exclusive)
            uint16_t addr = (cpu->h << 8) | cpu->l;
            uint8_t value = cpu->memory[addr];
            cpu->a = cpu->a ^ value;
            cpu->flags.c = 0;
            cpu->flags.ac = 0;
            setZSPflags(cpu, cpu->a);
            cpu->pc += 1;
            break;
        }
        case 0xaf:
        {
            if (debug) { printf("XRA    A"); } // Register B OR register A (exclusive)
            cpu->a = cpu->a ^ cpu->a;
            cpu->flags.c = 0;
            cpu->flags.ac = 0;
            setZSPflags(cpu, cpu->a);
            cpu->pc += 1;
            break;
        }
        case 0Xb0:
        {
            if (debug) { printf("ORA    B"); } // Register B OR register A
            cpu->a = cpu->a | cpu->b;
            cpu->flags.c = 0;
            cpu->flags.ac = 0;
            setZSPflags(cpu, cpu->a);
            cpu->pc += 1;
            break;
        }
        case 0Xb1:
        {
            if (debug) { printf("ORA    C"); } // Register C OR register A
            cpu->a = cpu->a | cpu->c;
            cpu->flags.c = 0;
            cpu->flags.ac = 0;
            setZSPflags(cpu, cpu->a);
            cpu->pc += 1;
            break;
        }
        case 0Xb2:
        {
            if (debug) { printf("ORA    D"); } // Register D OR register A
            cpu->a = cpu->a | cpu->d;
            cpu->flags.c = 0;
            cpu->flags.ac = 0;
            setZSPflags(cpu, cpu->a);
            cpu->pc += 1;
            break;
        }
        case 0Xb3:
        {
            if (debug) { printf("ORA    E"); } // Register E OR register A
            cpu->a = cpu->a | cpu->e;
            cpu->flags.c = 0;
            cpu->flags.ac = 0;
            setZSPflags(cpu, cpu->a);
            cpu->pc += 1;
            break;
        }
        case 0Xb4:
        {
            if (debug) { printf("ORA    H"); } // Register H OR register A
            cpu->a = cpu->a | cpu->h;
            cpu->flags.c = 0;
            cpu->flags.ac = 0;
            setZSPflags(cpu, cpu->a);
            cpu->pc += 1;
            break;
        }
        case 0Xb5:
        {
            if (debug) { printf("ORA    L"); } // Register L OR register A
            cpu->a = cpu->a | cpu->l;
            cpu->flags.c = 0;
            cpu->flags.ac = 0;
            setZSPflags(cpu, cpu->a);
            cpu->pc += 1;
            break;
        }
        case 0Xb6:
        {
            if (debug) { printf("ORA    M"); } // Register M OR register A
            uint16_t addr = (cpu->h << 8) | cpu->l;
            uint8_t value = cpu->memory[addr];
            cpu->a = cpu->a | value;
            cpu->flags.c = 0;
            cpu->flags.ac = 0;
            setZSPflags(cpu, cpu->a);
            cpu->pc += 1;
            break;
        }
        case 0Xb7:
        {
            if (debug) { printf("ORA    A"); } // Register A OR register A
            cpu->a = cpu->a | cpu->a;
            cpu->flags.c = 0;
            cpu->flags.ac = 0;
            setZSPflags(cpu, cpu->a);
            cpu->pc += 1;
            break;
        }
        case 0Xb8:
        {
            if (debug) { printf("CMP    B"); } // Compare register B with register A
            uint8_t difference = cpu->a - cpu->b;
            cpu->flags.c = (cpu->a < cpu->b);
            cpu->flags.ac = (cpu->a & 0x0F) < (cpu->b & 0x0F);
            setZSPflags(cpu, difference);
            cpu->pc += 1;
            break;
        }
        case 0Xb9:
        {
            if (debug) { printf("CMP    C"); } // Compare register C with register A
            uint8_t difference = cpu->a - cpu->c;
            cpu->flags.c = (cpu->a < cpu->c);
            cpu->flags.ac = (cpu->a & 0x0F) < (cpu->c & 0x0F);
            setZSPflags(cpu, difference);
            cpu->pc += 1;
            break;
        }
        case 0Xba:
        {
            if (debug) { printf("CMP    D"); } // Compare register D with register A
            uint8_t difference = cpu->a - cpu->d;
            cpu->flags.c = (cpu->a < cpu->d);
            cpu->flags.ac = (cpu->a & 0x0F) < (cpu->d & 0x0F);
            setZSPflags(cpu, difference);
            cpu->pc += 1;
            break;
        }
        case 0Xbb:
        {
            if (debug) { printf("CMP    E"); } // Compare register E with register A
            uint8_t difference = cpu->a - cpu->e;
            cpu->flags.c = (cpu->a < cpu->e);
            cpu->flags.ac = (cpu->a & 0x0F) < (cpu->e & 0x0F);
            setZSPflags(cpu, difference);
            cpu->pc += 1;
            break;
        }
        case 0Xbc:
        {
            if (debug) { printf("CMP    H"); } // Compare register H with register A
            uint8_t difference = cpu->a - cpu->h;
            cpu->flags.c = (cpu->a < cpu->h);
            cpu->flags.ac = (cpu->a & 0x0F) < (cpu->h & 0x0F);
            setZSPflags(cpu, difference);
            cpu->pc += 1;
            break;
        }
        case 0Xbd:
        {
            if (debug) { printf("CMP    L"); } // Compare register L with register A
            uint8_t difference = cpu->a - cpu->l;
            cpu->flags.c = (cpu->a < cpu->l);
            cpu->flags.ac = (cpu->a & 0x0F) < (cpu->l & 0x0F);
            setZSPflags(cpu, difference);
            cpu->pc += 1;
            break;
        }
        case 0Xbe:
        {
            if (debug) { printf("CMP    M"); } // Compare register M with register A
            uint16_t addr = (cpu->h << 8) | cpu->l;
            uint8_t value = cpu->memory[addr];
            uint8_t difference = cpu->a - value;
            cpu->flags.c = (cpu->a < cpu->h);
            cpu->flags.ac = (cpu->a & 0x0F) < (cpu->h & 0x0F);
            setZSPflags(cpu, difference);
            cpu->pc += 1;
            break;
        }
        case 0Xbf:
        {
            if (debug) { printf("CMP    A"); } // Compare register A with register A
            uint8_t difference = cpu->a - cpu->a;
            cpu->flags.c = (cpu->a < cpu->a);
            cpu->flags.ac = (cpu->a & 0x0F) < (cpu->a & 0x0F);
            setZSPflags(cpu, difference);
            cpu->pc += 1;
            break;
        }
        case 0Xc0:
        {
            if (debug) { printf("RNZ"); } // Return on no zero: if zero flag is not set, jump to address stored on stack
            if (cpu->flags.z == 0) {
                cpu->pc = (cpu->memory[cpu->sp + 1] << 8) | cpu->memory[cpu->sp];
                cpu->sp += 2;
            } else {
                cpu->pc += 1;
            }
            break;
        }
        case 0Xc1:
        {
            if (debug) { printf("POP    B"); } // Pop BC from stack
            cpu->c = cpu->memory[cpu->sp];
            cpu->b = cpu->memory[cpu->sp + 1];
            cpu->sp += 2;
            cpu->pc += 1;
            break;
        }
        case 0Xc2:
        {
            uint16_t addr = (code[2] << 8) | code[1];
            if (debug) { printf("JNZ    addr: %04x", addr); } // Jump if zero flag is not set
            if (cpu->flags.z == 0) { cpu->pc = addr; }
            else { cpu->pc += 3; }
            break;
        }
        case 0Xc3:
        {
            uint16_t addr = (code[2] << 8) | code[1];
            if (debug) { printf("JMP    addr: %04x", addr); } // Jump unconditional
            cpu->pc = addr;
            break;
        }
        case 0Xc4:
        {
            u_int16_t addr = (code[2] << 8) | code[1];
            if (debug) { printf("CNZ    addr: %04x", addr); } // Call on non-zero: jump to a new location in memory if the zero flag is not set
            if (cpu->flags.z == 0) {
                cpu->memory[cpu->sp - 1] = (cpu->pc >> 8) & 0xFF;
                cpu->memory[cpu->sp - 2] = cpu->pc & 0xFF;
                cpu->sp -= 2;
                cpu->pc = addr;
            }
            else { cpu->pc += 3; }
            break;
        }
        case 0Xc5:
        {
            if (debug) { printf("PUSH   B"); } // Push contents of register BC to stack
            cpu->memory[cpu->sp - 1] = cpu->b;
            cpu->memory[cpu->sp - 2] = cpu->c;
            cpu->sp -= 2;
            cpu->pc += 1;
            break;
        }
        case 0Xc6:
        {
            if (debug) { printf("ADI    %02x", code[1]); } // Add immediate to register A
            uint16_t answer = code[1] + cpu->a;
            cpu->flags.c = (answer > 0xFF);
            cpu->flags.ac = ((code[1] & 0x0F) + (cpu->a & 0x0F)) > 0x0F;
            cpu->a = answer;
            setZSPflags(cpu, cpu->a);
            cpu->pc += 2;
            break;
        }
        case 0Xc7:
        {
            if (debug) { printf("RST    0"); } // Restart 0
            cpu->memory[cpu->sp - 1] = (cpu->pc >> 8) & 0xFF;
            cpu->memory[cpu->sp - 2] = cpu->pc & 0xFF;
            cpu->sp -= 2;
            cpu->pc = 8 * (((*code & 0x38) >> 3) & 0x07); // This gets bits 3,4,5 of the opcode and multiplies it by 8
            break;
        }
        case 0Xc8:
        {
            if (debug) { printf("RZ"); } // Return on zero: if zero flag is set, jump to address stored on stack
            if (cpu->flags.z) {
                cpu->pc = (cpu->memory[cpu->sp + 1] << 8) | cpu->memory[cpu->sp];
                cpu->sp += 2;
            } else {
                cpu->pc += 1;
            }
            break;
        }
        case 0Xc9:
        {
            if (debug) { printf("RET"); } // Unconditional return: jump to address on stack
            cpu->pc = (cpu->memory[cpu->sp + 1] << 8) | cpu->memory[cpu->sp];
            cpu->sp += 2;
            break;
        }
        case 0Xca:
        {
            uint16_t addr = (code[2] << 8) | code[1];
            if (debug) { printf("JZ     addr: %04x", addr); } // Jump if zero flag is set
            if (cpu->flags.z) { cpu->pc = addr; }
            else { cpu->pc += 3; }
            break;
        }
        case 0Xcb:
        {
            uint16_t addr = (code[2] << 8) | code[1];
            if (debug) { printf("JMP    addr: %04x", addr); } // Jump unconditional
            cpu->pc = addr;            break;
        }
        case 0Xcc:
        {
            u_int16_t addr = (code[2] << 8) | code[1];
            if (debug) { printf("CZ     addr: %04x", addr); } // Call on zero: jump to new location in memory if zero flag is set
            if (cpu->flags.z) {
                cpu->memory[cpu->sp - 1] = (cpu->pc >> 8) & 0xFF;
                cpu->memory[cpu->sp - 2] = cpu->pc & 0xFF;
                cpu->sp -= 2;
                cpu->pc = addr;
            }
            else { cpu->pc += 3; }
            break;
        }
        case 0Xcd:
        {
            u_int16_t addr = (code[2] << 8) | code[1];
            if (debug) { printf("CALL   addr: %04x", addr); } // Unconditional call: jump to new location in memory
            cpu->memory[cpu->sp - 1] = (cpu->pc >> 8) & 0xFF;
            cpu->memory[cpu->sp - 2] = cpu->pc & 0xFF;
            cpu->sp -= 2;
            cpu->pc = addr;
            break;
        }
        case 0Xce:
        {
            if (debug) { printf("ACI    %02x", code[1]); } // Add immediate to A with carry
            uint16_t answer = code[1] + cpu->a + cpu->flags.c;
            cpu->flags.c = (answer > 0xFF);
            cpu->flags.ac = ((code[1] & 0x0F) + (cpu->a & 0x0F) + cpu->flags.c) > 0x0F;
            cpu->a = answer;
            setZSPflags(cpu, cpu->a);
            cpu->pc += 2;
            break;
        }
        case 0Xcf:
        {
            if (debug) { printf("RST    1"); } // Restart 1
            cpu->memory[cpu->sp - 1] = (cpu->pc >> 8) & 0xFF;
            cpu->memory[cpu->sp - 2] = cpu->pc & 0xFF;
            cpu->sp -= 2;
            cpu->pc = 8 * (((*code & 0x38) >> 3) & 0x07); // This gets bits 3,4,5 of the opcode and multiplies it by 8
            break;
        }
        case 0Xd0:
        {
            if (debug) { printf("RNC"); } // Return on no carry: if carry flag is not set, jump to address stored on stack
            if (cpu->flags.c == 0) {
                cpu->pc = (cpu->memory[cpu->sp + 1] << 8) | cpu->memory[cpu->sp];
                cpu->sp += 2;
            } else {
                cpu->pc += 1;
            }
            break;
        }
        case 0Xd1:
        {
            if (debug) { printf("POP    D"); } // Pop DE from stack
            cpu->e = cpu->memory[cpu->sp];
            cpu->d = cpu->memory[cpu->sp + 1];
            cpu->sp += 2;
            cpu->pc += 1;
            break;
        }
        case 0Xd2:
        {
            uint16_t addr = (code[2] << 8) | code[1];
            if (debug) { printf("JNC    addr: %04x", addr); } // Jump if carry flag is not set
            if (cpu->flags.c == 0) { cpu->pc = addr; }
            else { cpu->pc += 3; }
            break;
        }
        case 0Xd3:
        {
            /*
            (data) <- (A)
            The content of register A is placed on the eight bit
            bi-directional data bus for transmission to the specified port.
            */
            if (debug) { printf("OUT    port: %02x", code[1]); } // Output content from A to port address
            cpu->pc += 2;
            break;
        }
        case 0Xd4:
        {
            u_int16_t addr = (code[2] << 8) | code[1];
            if (debug) { printf("CNC    addr: %04x", addr); } // Call on non-carry: jump to a new location in memory if the carry flag is not set
            if (cpu->flags.c == 0) {
                cpu->memory[cpu->sp - 1] = (cpu->pc >> 8) & 0xFF;
                cpu->memory[cpu->sp - 2] = cpu->pc & 0xFF;
                cpu->sp -= 2;
                cpu->pc = addr;
            }
            else { cpu->pc += 3; }
            break;
        }
        case 0Xd5:
        {
            if (debug) { printf("PUSH   D"); } // Push contents of register DE to stack
            cpu->memory[cpu->sp - 1] = cpu->d;
            cpu->memory[cpu->sp - 2] = cpu->e;
            cpu->sp -= 2;
            cpu->pc += 1;
            break;
        }
        case 0Xd6:
        {
            if (debug) { printf("SUI    %02x", code[1]); } // Subtract immediate from register A
            uint16_t answer = (uint16_t)cpu->a - (uint16_t)code[1];
            cpu->flags.ac = ((cpu->a & 0x0F) < (code[1] & 0x0F));
            cpu->flags.c = (cpu->a < code[1]);
            cpu->a = answer & 0xFF;
            setZSPflags(cpu, cpu->a);
            cpu->pc += 2;
            break;
        }
        case 0Xd7:
        {
            if (debug) { printf("RST    2"); } // Restart 2
            cpu->memory[cpu->sp - 1] = (cpu->pc >> 8) & 0xFF;
            cpu->memory[cpu->sp - 2] = cpu->pc & 0xFF;
            cpu->sp -= 2;
            cpu->pc = 8 * (((*code & 0x38) >> 3) & 0x07); // This gets bits 3,4,5 of the opcode
            break;
        }
        case 0Xd8:
        {
            if (debug) { printf("RC"); } // Return on carry: if carry flag is set, jump to address stored on stack
            if (cpu->flags.c) {
                cpu->pc = (cpu->memory[cpu->sp + 1] << 8) | cpu->memory[cpu->sp];
                cpu->sp += 2;
            } else {
                cpu->pc += 1;
            }
            break;
        }
        case 0Xd9:
        {
            if (debug) { printf("RET"); } // Unconditional return: jump to address on stack
            cpu->pc = (cpu->memory[cpu->sp + 1] << 8) | cpu->memory[cpu->sp];
            cpu->sp += 2;
            break;
        }
        case 0Xda:
        {
            uint16_t addr = (code[2] << 8) | code[1];
            if (debug) { printf("JNC    addr: %04x", addr); } // Jump if carry flag is set
            if (cpu->flags.c) { cpu->pc = addr; }
            else { cpu->pc += 3; }
            break;
        }
        case 0Xdb:
        {
            /*
            (A) <- (data)
            The data placed on the eight bit bi-directional data
            bus by the specified port is moved to register A.
            */
            if (debug) { printf("IN     port: %02x", code[1]); } // Input content from port address
            cpu->pc += 2;
            break;
        }
        case 0Xdc:
        {
            u_int16_t addr = (code[2] << 8) | code[1];
            if (debug) { printf("CC     addr: %04x", addr); } // Call on carry: jump to new location in memory if carry flag is set
            if (cpu->flags.c) {
                cpu->memory[cpu->sp - 1] = (cpu->pc >> 8) & 0xFF;
                cpu->memory[cpu->sp - 2] = cpu->pc & 0xFF;
                cpu->sp -= 2;
                cpu->pc = addr;
            }
            else { cpu->pc += 3; }
            break;
        }
        case 0Xdd:
        {
            u_int16_t addr = (code[2] << 8) | code[1];
            if (debug) { printf("CALL   addr: %04x", addr); } // Unconditional call: jump to new location in memory
            cpu->memory[cpu->sp - 1] = (cpu->pc >> 8) & 0xFF;
            cpu->memory[cpu->sp - 2] = cpu->pc & 0xFF;
            cpu->sp -= 2;
            cpu->pc = addr;
            break;
        }
        case 0Xde:
        {
            if (debug) { printf("SBI    %02x", code[1]); } // Subtract immediate from A with borrow
            uint16_t answer = (uint16_t)cpu->a - (uint16_t)code[1] - cpu->flags.c;
            cpu->flags.ac = ((cpu->a & 0x0F) < ((code[1] + cpu->flags.c) & 0x0F));
            cpu->flags.c = (cpu->a < (code[1] + cpu->flags.c));
            cpu->a = answer & 0xFF;
            setZSPflags(cpu, cpu->a);
            cpu->pc += 2;
            break;
        }
        case 0Xdf:
        {
            if (debug) { printf("RST    3"); } // Restart 3
            cpu->memory[cpu->sp - 1] = (cpu->pc >> 8) & 0xFF;
            cpu->memory[cpu->sp - 2] = cpu->pc & 0xFF;
            cpu->sp -= 2;
            cpu->pc = 8 * (((*code & 0x38) >> 3) & 0x07); // This gets bits 3,4,5 of the opcode
            break;
        }
        case 0Xe0:
        {
            if (debug) { printf("RPO"); } // Return on parity odd: if parity flag is odd, jump to address stored on stack
            if (cpu->flags.p == 0) {
                cpu->pc = (cpu->memory[cpu->sp + 1] << 8) | cpu->memory[cpu->sp];
                cpu->sp += 2;
            } else {
                cpu->pc += 1;
            }
            break;
        }
        case 0Xe1:
        {
            if (debug) { printf("POP    H"); } // Pop HL from stack
            cpu->l = cpu->memory[cpu->sp];
            cpu->h = cpu->memory[cpu->sp + 1];
            cpu->sp += 2;
            cpu->pc += 1;
            break;
        }
        case 0Xe2:
        {
            uint16_t addr = (code[2] <<8) | code[1];
            if (debug) { printf("JPO    addr: %04x", addr); } // Jump if parity flag is odd
            if (cpu->flags.p == 0) { cpu->pc = addr; }
            else { cpu->pc += 3; }
            break;
        }
        case 0Xe3:
        {
            if (debug) { printf("XTHL"); } // Exchange contents from HL and top of stack
            uint8_t h = cpu->h;
            uint8_t l = cpu->l;
            cpu->l = cpu->memory[cpu->sp];
            cpu->h = cpu->memory[cpu->sp + 1];
            cpu->memory[cpu->sp] = l;
            cpu->memory[cpu->sp + 1] = h;
            cpu->pc += 1;
            break;
        }
        case 0Xe4:
        {
            uint16_t addr = (code[2] <<8) | code[1];
            if (debug) { printf("CPO    addr: %04x", addr); } // Call on parity-odd: jump to a new location in memory if the parity flag is odd
            if (cpu->flags.p == 0) {
                cpu->memory[cpu->sp - 1] = (cpu->pc >> 8) & 0xFF;
                cpu->memory[cpu->sp - 2] = cpu->pc & 0xFF;
                cpu->sp -= 2;
                cpu->pc = addr;
            }
            else { cpu->pc += 3; }
            break;
        }
        case 0Xe5:
        {
            if (debug) { printf("PUSH   H"); } // Push contents of register HL to stack
            cpu->memory[cpu->sp - 1] = cpu->h;
            cpu->memory[cpu->sp - 2] = cpu->l;
            cpu->sp -= 2;
            cpu->pc += 1;
            break;
        }
        case 0Xe6:
        {
            if (debug) { printf("ANI    %02x", code[1]); } // Add immediate to register A
            cpu->a = cpu->a & code[1];
            cpu->flags.c = 0;
            cpu->flags.ac = ((cpu->a | code[1]) & 0x08) != 0;
            setZSPflags(cpu, cpu->a);
            cpu->pc += 2;
            break;
        }
        case 0Xe7:
        {
            if (debug) { printf("RST    4"); } // Restart 4
            cpu->memory[cpu->sp - 1] = (cpu->pc >> 8) & 0xFF;
            cpu->memory[cpu->sp - 2] = cpu->pc & 0xFF;
            cpu->sp -= 2;
            cpu->pc = 8 * (((*code & 0x38) >> 3) & 0x07); // This gets bits 3,4,5 of the opcode
            break;
        }
        case 0Xe8:
        {
            if (debug) { printf("RPE"); } // Return on parity even: if parity flag is even, jump to address stored on stack
            if (cpu->flags.p) {
                cpu->pc = (cpu->memory[cpu->sp + 1] << 8) | cpu->memory[cpu->sp];
                cpu->sp += 2;
            } else {
                cpu->pc += 1;
            }
            break;
        }
        case 0Xe9:
        {
            if (debug) { printf("PCHL"); } // Copies the contents from HL to the program counter
            cpu->pc = (cpu->h << 8) | cpu->l;
            break;
        }
        case 0Xea:
        {
            uint16_t addr = (code[2] <<8) | code[1];
            if (debug) { printf("JPE    addr: %04x", addr); } // Jump if parity flag is even
            if (cpu->flags.p) { cpu->pc = addr; }
            else { cpu->pc += 3; }
            break;
        }
        case 0Xeb:
        {
            if (debug) { printf("XCHG"); } // Exchange contents of HL and DE
            uint8_t d = cpu->d;
            uint8_t e = cpu->e;
            cpu->d = cpu->h;
            cpu->e = cpu->l;
            cpu->h = d;
            cpu->l = e;
            cpu->pc += 1;
            break;
        }
        case 0Xec:
        {
            uint16_t addr = (code[2] <<8) | code[1];
            if (debug) { printf("CPO    addr: %04x", addr); } // Call on parity-even: jump to a new location in memory if the parity flag is even
            if (cpu->flags.p) {
                cpu->memory[cpu->sp - 1] = (cpu->pc >> 8) & 0xFF;
                cpu->memory[cpu->sp - 2] = cpu->pc & 0xFF;
                cpu->sp -= 2;
                cpu->pc = addr;
            }
            else { cpu->pc += 3; }
            break;
        }
        case 0Xed:
        {
            u_int16_t addr = (code[2] << 8) | code[1];
            if (debug) { printf("CALL   addr: %04x", addr); } // Unconditional call: jump to new location in memory
            cpu->memory[cpu->sp - 1] = (cpu->pc >> 8) & 0xFF;
            cpu->memory[cpu->sp - 2] = cpu->pc & 0xFF;
            cpu->sp -= 2;
            cpu->pc = addr;
            break;
        }
        case 0Xee:
        {
            if (debug) { printf("XRI    %02x", code[1]); } // immediate OR A (exclusive)
            cpu->a = cpu->a ^ code[1];
            cpu->flags.c = 0;
            cpu->flags.ac = 0;
            setZSPflags(cpu, cpu->a);
            cpu->pc += 2;
            break;
        }
        case 0Xef:
        {
            if (debug) { printf("RST    5"); } // Restart 3
            cpu->memory[cpu->sp - 1] = (cpu->pc >> 8) & 0xFF;
            cpu->memory[cpu->sp - 2] = cpu->pc & 0xFF;
            cpu->sp -= 2;
            cpu->pc = 8 * (((*code & 0x38) >> 3) & 0x07); // This gets bits 3,4,5 of the opcode
            break;
        }
        case 0Xf0:
        {
            if (debug) { printf("RP"); } // Return on positive: if sign flag is not set, jump to address stored on stack
            if (cpu->flags.s == 0) {
                cpu->pc = (cpu->memory[cpu->sp + 1] << 8) | cpu->memory[cpu->sp];
                cpu->sp += 2;
            } else {
                cpu->pc += 1;
            }
            break;
        }
        case 0Xf1:
        {
            if (debug) { printf("POP    PSW"); } // Pop A and flags from stack
            uint8_t flags = cpu->memory[cpu->sp];
            cpu->a = cpu->memory[cpu->sp + 1];
            
            cpu->flags.c = flags & 1;
            cpu->flags.p = (flags >> 2) & 1;
            cpu->flags.ac = (flags >> 4) & 1;
            cpu->flags.z = (flags >> 6) & 1;
            cpu->flags.s = (flags >> 7) & 1;
            
            cpu->sp += 2;
            cpu->pc += 1;
            break;
        }
        case 0Xf2:
        {
            uint16_t addr = (code[2] << 8) | code[1];
            if (debug) { printf("JP     addr: %04x", addr); } // Jump if sign flag is not set
            if (cpu->flags.s == 0) { cpu->pc = addr; }
            else { cpu->pc += 3; }
            break;
        }
        case 0Xf3:
        {
            if (debug) { printf("DI"); } // Disable interupt
            cpu->pc += 1;
            break;
        }
        case 0Xf4:
        {
            uint16_t addr = (code[2] << 8) | code[1];
            if (debug) { printf("CP     addr: %04x", addr); } // Call on positive: jump to a new location in memory if the sign flag is not set
            if (cpu->flags.s == 0) {
                cpu->memory[cpu->sp - 1] = (cpu->pc >> 8) & 0xFF;
                cpu->memory[cpu->sp - 2] = cpu->pc & 0xFF;
                cpu->sp -= 2;
                cpu->pc = addr;
            }
            else { cpu->pc += 3; }
            break;
        }
        case 0Xf5:
        {
            if (debug) { printf("PUSH   PSW"); } // Push contents of register A and flags to stack
            cpu->memory[cpu->sp - 1] = cpu->a;

            uint8_t flags = 0;
            flags |= cpu->flags.s << 7;
            flags |= cpu->flags.z << 6;
            flags |= cpu->flags.ac << 4;
            flags |= cpu->flags.p << 2;
            flags |= 1 << 1;
            flags |= cpu->flags.c;

            cpu->memory[cpu->sp - 2] = flags;
            cpu->sp -= 2;
            cpu->pc += 1;
            break;
        }
        case 0Xf6:
        {
            if (debug) { printf("ORI    %02x", code[1]); } // Immediate OR register A
            cpu->a = cpu->a | code[1];
            cpu->flags.c = 0;
            cpu->flags.ac = 0;
            setZSPflags(cpu, cpu->a);
            cpu->pc += 2;
            break;
        }
        case 0Xf7:
        {
            if (debug) { printf("RST    6"); } // Restart 6
            cpu->memory[cpu->sp - 1] = (cpu->pc >> 8) & 0xFF;
            cpu->memory[cpu->sp - 2] = cpu->pc & 0xFF;
            cpu->sp -= 2;
            cpu->pc = 8 * (((*code & 0x38) >> 3) & 0x07); // This gets bits 3,4,5 of the opcode
            break;
        }
        case 0Xf8:
        {
            if (debug) { printf("RM"); } // Return on minus: if sign flag is set, jump to address stored on stack
            if (cpu->flags.s) {
                cpu->pc = (cpu->memory[cpu->sp + 1] << 8) | cpu->memory[cpu->sp];
                cpu->sp += 2;
            } else {
                cpu->pc += 1;
            }
            break;
        }
        case 0Xf9:
        {
            if (debug) { printf("SPHL"); } // Copies the contents from HL to the stack pointer
            cpu->sp = (cpu->h << 8) | cpu->l;
            cpu->pc += 1;
            break;
        }
        case 0Xfa:
        {
            uint16_t addr = (code[2] << 8) | code[1];
            if (debug) { printf("JM     addr: %04x", addr); } // Jump on minus: if sign flag is set
            if (cpu->flags.s) { cpu->pc = addr; }
            else { cpu->pc += 3; }
            break;
        }
        case 0Xfb:
        {
            if (debug) { printf("EI"); } // Enable interupts
            cpu->pc += 1;
            break;
        }
        case 0Xfc:
        {
            u_int16_t addr = (code[2] << 8) | code[1];
            if (debug) { printf("CM     addr: %04x", addr); } // Call on minus: jump to new location in memory if sign flag is set
            if (cpu->flags.s == 0) {
                cpu->memory[cpu->sp - 1] = (cpu->pc >> 8) & 0xFF;
                cpu->memory[cpu->sp - 2] = cpu->pc & 0xFF;
                cpu->sp -= 2;
                cpu->pc = addr;
            }
            else { cpu->pc += 3; }
            break;
        }
        case 0Xfd:
        {
            u_int16_t addr = (code[2] << 8) | code[1];
            if (debug) { printf("CALL   addr: %04x", addr); } // Unconditional call: jump to new location in memory
            cpu->memory[cpu->sp - 1] = (cpu->pc >> 8) & 0xFF;
            cpu->memory[cpu->sp - 2] = cpu->pc & 0xFF;
            cpu->sp -= 2;
            cpu->pc = addr;
            break;
        }
        case 0Xfe:
        {
            if (debug) { printf("CPI    %02x", code[1]); } // Compare immediate with contents of A
            uint8_t difference = cpu->a - code[1];
            cpu->flags.c = (cpu->a < code[1]);
            cpu->flags.ac = (cpu->a & 0x0F) < (code[1] & 0x0F);
            setZSPflags(cpu, difference);
            cpu->pc += 2;
            break;
        }
        case 0Xff:
        {
            if (debug) { printf("RST    7"); } // Restart 7
            cpu->memory[cpu->sp - 1] = (cpu->pc >> 8) & 0xFF;
            cpu->memory[cpu->sp - 2] = cpu->pc & 0xFF;
            cpu->sp -= 2;
            cpu->pc = 8 * (((*code & 0x38) >> 3) & 0x07); // This gets bits 3,4,5 of the opcode
            break;
        }
    }

    if (debug) { printf("\n"); }

}


int main (int argc, char**argv) {

    State8080* cpu = new State8080();
    memset(cpu, 0, sizeof(State8080));


    FILE *f= fopen(argv[1], "rb");
    if (f==NULL)
    {
        printf("error: Couldn't open %s\n", argv[1]);
        exit(1);
    };

    bool debug = false;
    if (argc >= 3) {
        if (std::string(argv[2]) == "--debug") {
        debug = true;
        }
    };

    // Get the file size and read it into a memory buffer
    fseek(f, 0L, SEEK_END);
    int fsize = ftell(f);
    fseek(f, 0L, SEEK_SET);

    unsigned char *buffer = (unsigned char *) malloc(fsize);

    fread(buffer, fsize, 1, f);
    fclose(f);

    int i = 0;

    while (i < 1000) {
        Emulate8080Op(cpu, buffer, debug);
        i++;
    }
    return 0;
};