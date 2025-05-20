/*
 * This file is part of intel8080 emulator package.
 *
 * Developed for CS 467 - Capstone Project.
 *
 * This program can be run from the terminal with a rom file
 * with or without the debug flag that disassembles the opcodes
 * 
 * ./emulator ./path/to/rom_file.bin --debug
 * 
 */


#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <typeinfo>
#include <iostream>
#include <cstring>
#include <cstdint>
#include <map>
#include "emulator.h"


void setZSPflags(State8080* cpu, uint8_t result) {
    /**
    * Sets Zero, Sign, and Parity flags based on {result}.
    *
    * @param cpu State of the cpu object.
    * @param result Result that the flags will be based on.
    * @return void: updates state of the flags in the cpu object.
    */
    cpu->flags.z = (result == 0);
    cpu->flags.s = (result >= 0x80);
    
    int count = 0;
    uint8_t x = result;
    for (int i = 0; i< 8; i++) {
        if (x & 0x01) count++;
        x >>= 1;
    };
    cpu->flags.p = (count % 2 == 0);
}


void Emulate8080Op(State8080* cpu) {
    uint8_t* code = &cpu->memory[cpu->pc];
    #ifdef DEBUG
		printf("pc: %04x  sp: %04x  a: %02x  bc: %02x%02x  de: %02x%02x  hl: %02x%02x  flags: z: %01x  s: %01x  p: %01x  cy: %01x  ac: %01x\n\t",
            cpu->pc, cpu->sp, cpu->a, cpu->b, cpu->c, cpu->d, cpu->e, cpu->h, cpu->l, cpu->flags.z, cpu->flags.s, cpu->flags.p, cpu->flags.c, cpu->flags.ac);
	#endif
    switch (*code) {
    case 0x00:
    {
        #ifdef DEBUG
			printf("NOP");
		#endif
        cpu->pc += 1;
        cpu->cycles += 4;
        break;
    }
    case 0x01:
    {
        #ifdef DEBUG
			printf("LXI    B, %02x%02x", code[2], code[1]);
		#endif
        cpu->b = code[2];
        cpu->c = code[1];
        cpu->pc += 3;
        cpu->cycles += 10;
        break;
    }
    case 0x02:
    {
        #ifdef DEBUG
			printf("STAX   B");
		#endif
        uint16_t addr = (cpu->b << 8) | cpu->c;
        cpu->memory[addr] = cpu->a;
        cpu->pc += 1;
        cpu->cycles += 7;
        break;
    }
    case 0x03:
    {
        #ifdef DEBUG
			printf("INX    B");
		#endif
        uint16_t bc = (cpu->b << 8) | cpu->c;
        bc += 1;
        cpu->b = (bc >> 8) & 0xFF;
        cpu->c = bc & 0xFF;
        cpu->pc += 1;
        cpu->cycles += 5;
        break;
    }
    case 0x04:
    {
        #ifdef DEBUG
			printf("INR    B");
		#endif
        cpu->flags.ac = ((cpu->b & 0x0F) + 1) > 0x0F;
        cpu->b += 1;
        setZSPflags(cpu, cpu->b);
        cpu->pc += 1;
        cpu->cycles += 5;
        break;
    }
    case 0x05:
    {
        #ifdef DEBUG
			printf("DCR    B");
		#endif
        cpu->flags.ac = ((cpu->b & 0x0F) == 0);
        cpu->b -= 1;
        setZSPflags(cpu, cpu->b);
        cpu->pc += 1;
        cpu->cycles += 5;
        break;
    }
    case 0x06:
    {
        #ifdef DEBUG
			printf("MVI    B, %02x", code[1]);
		#endif
        cpu->b = code[1];
        cpu->pc += 2;
        cpu->cycles += 7;
        break;
    }
    case 0x07:
    {
        #ifdef DEBUG
			printf("RLC");
		#endif
        uint8_t a = cpu->a;
        cpu->flags.c = a >> 7;
        cpu->a = (a << 1) | (a >> 7);
        cpu->pc += 1;
        cpu->cycles += 4;
        break;
    }
    case 0x08:
    {
        #ifdef DEBUG
			printf("NOP");
		#endif
        cpu->pc += 1;
        cpu->cycles += 4;
        break;
    }
    case 0x09:
    {
        #ifdef DEBUG
			printf("DAD    B");
		#endif // Add BC to HL
        uint16_t bc = (cpu->b << 8) | cpu->c;
        uint16_t hl = (cpu->h << 8) | cpu->l;
        uint32_t answer = hl + bc;
        cpu->flags.c = (answer > 0xFFFF);
        cpu->h = (answer >> 8) & 0xFF;
        cpu->l = answer & 0xFF;
        cpu->pc += 1;
        cpu->cycles += 10;
        break;
    }
    case 0x0a:
    {
        #ifdef DEBUG
			printf("LDAX   B");
		#endif // Load A indirect
        uint16_t addr = (cpu->b << 8) | cpu->c;
        cpu->a = cpu->memory[addr];
        cpu->pc += 1;
        cpu->cycles += 7;
        break;
    }
    case 0x0b:
    {
        #ifdef DEBUG
			printf("DCX    B");
		#endif // Decrement BC
        uint16_t bc = (cpu->b << 8) | cpu->c;
        bc -= 1;
        cpu->b = (bc >> 8) & 0xFF;
        cpu->c = bc & 0xFF;
        cpu->pc += 1;
        cpu->cycles += 5;
        break;
    }
    case 0x0c:
    {
        #ifdef DEBUG
			printf("INR    C");
		#endif // Increment C
        cpu->flags.ac = ((cpu->c & 0x0F) + 1) > 0x0F;
        cpu->c += 1;
        setZSPflags(cpu, cpu->c);
        cpu->pc += 1;
        cpu->cycles += 5;
        break;
    }
    case 0x0d:
    {
        #ifdef DEBUG
			printf("DCR    C");
		#endif // Decrement C
        cpu->flags.ac = ((cpu->c & 0x0F) == 0);
        cpu->c -= 1;
        setZSPflags(cpu, cpu->c);
        cpu->pc += 1;
        cpu->cycles += 5;
        break;
    }
    case 0x0e:
    {
        #ifdef DEBUG
			printf("MVI    C, %02x", code[1]);
		#endif // Move immediate register to C
        cpu->c = code[1];
        cpu->pc += 2;
        cpu->cycles += 7;
        break;
    }
    case 0x0f:
    {
        #ifdef DEBUG
			printf("RRC");
		#endif // Rotate A right
        uint8_t a = cpu->a;
        cpu->a = ((a & 1) << 7) | (a >> 1);
        cpu->flags.c = ((a & 1) == 1);
        cpu->pc += 1;
        cpu->cycles += 4;
        break;
    }
    case 0x10:
    {
        #ifdef DEBUG
			printf("NOP");
		#endif
        cpu->pc += 1;
        cpu->cycles += 4;
        break;
    }
    case 0x11:
    {
        #ifdef DEBUG
			printf("LXI    D, %02x%02x", code[2], code[1]);
		#endif // Load immediate register pair
        cpu->d = code[2];
        cpu->e = code[1];
        cpu->pc += 3;
        cpu->cycles += 10;
        break;
    }
    case 0x12:
    {
        #ifdef DEBUG
			printf("STAX   D");
		#endif // Store A indirect
        uint16_t addr = (cpu->d << 8) | cpu->e;
        cpu->memory[addr] = cpu->a;
        cpu->pc += 1;
        cpu->cycles += 7;
        break;
    }
    case 0x13:
    {
        #ifdef DEBUG
			printf("INX    D");
		#endif // Increment DE
        uint16_t de = (cpu->d << 8) | cpu->e;
        de += 1;
        cpu->d = (de >> 8) & 0xFF;
        cpu->e = de & 0xFF;
        cpu->pc += 1;
        cpu->cycles += 5;
        break;
    }
    case 0x14:
    {
        #ifdef DEBUG
			printf("INR    D");
		#endif // Increment D
        cpu->flags.ac = ((cpu->d & 0x0F) + 1) > 0x0F;
        cpu->d += 1;
        setZSPflags(cpu, cpu->d);
        cpu->pc += 1;
        cpu->cycles += 5;
        break;
    }
    case 0x15:
    {
        #ifdef DEBUG
			printf("DCR    D");
		#endif // Decrement D
        cpu->flags.ac = ((cpu->c & 0x0F) == 0);
        cpu->d -= 1;
        setZSPflags(cpu, cpu->c);
        cpu->pc += 1;
        cpu->cycles += 5;
        break;
    }
    case 0x16:
    {
        #ifdef DEBUG
			printf("MVI    D, %02x", code[1]);
		#endif // Move immediate register to D
        cpu->d = code[1];
        cpu->pc += 2;
        cpu->cycles += 7;
        break;
    }
    case 0x17:
    {
        #ifdef DEBUG
			printf("RAL");
		#endif // Rotate A left through carry
        uint8_t a = cpu->a;
        cpu->a = (a << 1) | (cpu->flags.c);
        cpu->flags.c = a >> 7;
        cpu->pc += 1;
        cpu->cycles += 4;
        break;
    }
    case 0x18:
    {
        #ifdef DEBUG
			printf("NOP");
		#endif
        cpu->pc += 1;
        cpu->cycles += 4;
        break;
        
    }
    case 0x19:
    {
        #ifdef DEBUG
			printf("DAD    D");
		#endif // Add DE to HL
        uint16_t de = (cpu->d << 8) | cpu->e;
        uint16_t hl = (cpu->h << 8) | cpu->l;
        uint32_t answer = hl + de;
        cpu->flags.c = (answer > 0xFFFF);
        cpu->h = (answer >> 8) & 0xFF;
        cpu->l = answer & 0xFF;
        cpu->pc += 1;
        cpu->cycles += 10;
        break;
    }
    case 0x1a:
    {
        #ifdef DEBUG
			printf("LDAX   D");
		#endif // Load A indirect
        uint16_t addr = (cpu->d << 8) | cpu->e;
        cpu->a = cpu->memory[addr];
        cpu->pc += 1;
        cpu->cycles += 7;
        break;
    }
    case 0x1b:
    {
        #ifdef DEBUG
			printf("DCX    D");
		#endif // Decrement DE
        uint16_t de = (cpu->d << 8) | cpu->e;
        de -= 1;
        cpu->d = (de >> 8) & 0xFF;
        cpu->e = de & 0xFF;
        cpu->pc += 1;
        cpu->cycles += 5;
        break;
    }
    case 0x1c:
    {
        #ifdef DEBUG
			printf("INR    E");
		#endif // Increment E
        cpu->flags.ac = ((cpu->e & 0x0F) + 1) > 0x0F;
        cpu->e += 1;
        setZSPflags(cpu, cpu->e);
        cpu->pc += 1;
        cpu->cycles += 5;
        break;
    }
    case 0x1d:
    {
        #ifdef DEBUG
			printf("DCR    E");
		#endif // Decrement E
        cpu->flags.ac = ((cpu->e & 0x0F) == 0);
        cpu->e -= 1;
        setZSPflags(cpu, cpu->e);
        cpu->pc += 1;
        cpu->cycles += 5;
        break;
    }
    case 0x1e:
    {
        #ifdef DEBUG
			printf("MVI    E, %02x", code[1]);
		#endif // Move immediate register to E
        cpu->e = code[1];
        cpu->pc += 2;
        cpu->cycles += 7;
        break;
    }
    case 0x1f:
    {
        /*
        "Rotate Accumulator Right through Carry" (RAR) is a bit-manipulation instruction that shifts the bits of an accumulator register to the right,
        with the least significant bit (LSB) being moved into the carry flag, and the carry flag being moved into the most significant bit (MSB)
        (An) <- (A n+l); (CY)..- (AO)
        (A7) <- (CY)
        */
        #ifdef DEBUG
			printf("RAR");
		#endif // Rotate A right through carry
        uint8_t a = cpu->a;
        cpu->a = (cpu->flags.c << 7) | (a >> 1);
        cpu->flags.c = a & 1;
        cpu->pc += 1;
        cpu->cycles += 4;
        break;
    }
    case 0x20:
    {
        #ifdef DEBUG
			printf("NOP");
		#endif
        cpu->pc += 1;
        cpu->cycles += 4;
        break;
    }
    case 0x21:
    {
        #ifdef DEBUG
			printf("LXI    H, %02x%02x", code[2], code[1]);
		#endif // Load immediate register pair HL
        cpu->h = code[2];
        cpu->l = code[1];
        cpu->pc += 3;
        cpu->cycles += 10;
        break;
    }
    case 0x22:
    {
        uint16_t addr = (code[2] << 8) | code[1];
        #ifdef DEBUG
			printf("SHLD   addr: %04x", addr);
		#endif // Store HL direct: from HL to memory
        cpu->memory[addr] = cpu->l;
        cpu->memory[addr + 1] = cpu->h;
        cpu->pc += 3;
        cpu->cycles += 16;
        break;
    }
    case 0x23:
    {
        #ifdef DEBUG
			printf("INX    H");
		#endif // Increment HL
        uint16_t hl = (cpu->h << 8) | cpu->l;
        hl += 1;
        cpu->h = (hl >> 8) & 0xFF;
        cpu->l = hl & 0xFF;
        cpu->pc += 1;
        cpu->cycles += 5;
        break;
    }
    case 0x24:
    {
        #ifdef DEBUG
			printf("INR    H");
		#endif // Increment H
        cpu->flags.ac = ((cpu->h & 0x0F) + 1) > 0x0F;
        cpu->h += 1;
        setZSPflags(cpu, cpu->h);
        cpu->pc += 1;
        cpu->cycles += 5;
        break;
    }
    case 0x25:
    {
        #ifdef DEBUG
			printf("DCR    H");
		#endif // Decrement H
        cpu->flags.ac = ((cpu->h & 0x0F) == 0);
        cpu->h -= 1;
        setZSPflags(cpu, cpu->h);
        cpu->pc += 1;
        cpu->cycles += 5;
        break;
    }
    case 0x26:
    {
        #ifdef DEBUG
			printf("MVI    H, %02x", code[1]);
		#endif // Move immediate register to H
        cpu->h = code[1];
        cpu->pc += 2;
        cpu->cycles += 7;
        break;
    }
    case 0x27:
    {
        /*
        Decimal Adjust Accumulator. It's an instruction used to convert the result of an 8-bit binary addition of two binary
        coded decimal (BCD) numbers into two valid BCD digits.
        */
        #ifdef DEBUG
			printf("DAA");
		#endif // Decimal adjust A
        uint8_t ls4 = cpu->a & 0x0F;
        bool set_ac = false;
        bool set_c = false;
        if (ls4 > 9 || cpu->flags.ac) {
            cpu->a += 0x06;
            set_ac = true;

        }
        if (cpu->a > 0x99 || cpu->flags.c) {
            cpu->a += (0x60);
            set_c = true;
        }
        setZSPflags(cpu, cpu->a);
        cpu->flags.ac = set_ac;
        cpu->flags.c = set_c;
        cpu->pc += 1;
        cpu->cycles += 4;
        break;
    }
    case 0x28:
    {
        #ifdef DEBUG
			printf("NOP");
		#endif
        cpu->pc += 1;
        cpu->cycles += 4;
        break;
    }
    case 0x29:
    {
        #ifdef DEBUG
			printf("DAD    H");
		#endif // Add HL to HL
        uint16_t hl = (cpu->h << 8) | cpu->l;
        uint32_t answer = hl + hl;
        cpu->flags.c = (answer > 0xFFFF);
        cpu->h = (answer >> 8) & 0xFF;
        cpu->l = answer & 0xFF;
        cpu->pc += 1;
        cpu->cycles += 10;
        break;
    }
    case 0x2a:
    {
        uint16_t addr = (code[2] << 8) | code[1];
        #ifdef DEBUG
			printf("LHLD   addr: %04x", addr);
		#endif // Load HL direct: from memory to HL
        cpu->l = cpu->memory[addr];
        cpu->h = cpu->memory[addr + 1];
        cpu->pc += 3;
        cpu->cycles += 16;
        break;
    }
    case 0x2b:
    {
        #ifdef DEBUG
			printf("DCX    H");
		#endif // Decrement HL
        uint16_t hl = (cpu->h << 8) | cpu->l;
        hl -= 1;
        cpu->h = (hl >> 8) & 0xFF;
        cpu->l = hl & 0xFF;
        cpu->pc += 1;
        cpu->cycles += 5;
        break;
    }
    case 0x2c:
    {
        #ifdef DEBUG
			printf("INR    L");
		#endif // Increment L
        cpu->flags.ac = ((cpu->l & 0x0F) + 1) > 0x0F;
        cpu->l += 1;
        setZSPflags(cpu, cpu->l);
        cpu->pc += 1;
        cpu->cycles += 5;
        break;
    }
    case 0x2d:
    {
        #ifdef DEBUG
			printf("DCR    L");
		#endif // Decrement L
        cpu->flags.ac = ((cpu->l & 0x0F) == 0);
        cpu->l -= 1;
        setZSPflags(cpu, cpu->l);
        cpu->pc += 1;
        cpu->cycles += 5;
        break;
    }
    case 0x2e:
    {
        #ifdef DEBUG
			printf("MVI    L, %02x", code[1]);
		#endif // Move immediate register to L
        cpu->l = code[1];
        cpu->pc += 2;
        cpu->cycles += 7;
        break;
    }
    case 0x2f:
    {
        #ifdef DEBUG
			printf("CMA");
		#endif // Complements the contents of A, assigns to A
        cpu->a = ~cpu->a;
        cpu->pc += 1;
        cpu->cycles += 4;
        break;
    }
    case 0x30:
    {
        #ifdef DEBUG
			printf("NOP");
		#endif
        cpu->pc += 1;
        cpu->cycles += 4;
        break;
    }
    case 0x31:
    {
        #ifdef DEBUG
			printf("LXI    SP, %02x%02x", code[2], code[1]);
		#endif // Load immediate to stack pointer
        cpu->sp = (code[2] << 8) | code[1];
        cpu->pc += 3;
        cpu->cycles += 10;
        break;
    }
    case 0x32:
    {
        uint16_t addr = (code[2] << 8) | code[1];
        #ifdef DEBUG
			printf("STA    addr: %04x", addr);
		#endif // Store A direct: from A to memory
        cpu->memory[addr] = cpu->a;
        cpu->pc += 3;
        cpu->cycles += 13;
        break;
    }
    case 0x33:
    {
        #ifdef DEBUG
			printf("INX    SP");
		#endif // Increment stack pointer
        cpu->sp += 1;
        cpu->pc += 1;
        cpu->cycles += 5;
        break;
    }
    case 0x34:
    {
        #ifdef DEBUG
			printf("INR    M");
		#endif // Increment value stored in memory at HL
        uint16_t addr = (cpu->h << 8) | cpu->l;
        uint8_t value = cpu->memory[addr];
        cpu->flags.ac = ((value & 0x0F) + 1) > 0x0F;
        value += 1;
        cpu->memory[addr] = value;
        setZSPflags(cpu, value);
        cpu->pc += 1;
        cpu->cycles += 10;
        break;
    }
    case 0x35:
    {
        #ifdef DEBUG
			printf("DCR    M");
		#endif // Decrement value in memory at HL
        uint16_t addr = (cpu->h << 8) | cpu->l;
        uint8_t value = cpu->memory[addr];
        cpu->flags.ac = ((value & 0x0F) == 0);
        value -= 1;
        cpu->memory[addr] = value;
        setZSPflags(cpu, value);
        cpu->pc += 1;
        cpu->cycles += 10;
        break;
    }
    case 0x36:
    {
        #ifdef DEBUG
			printf("MVI    M, %02x", code[1]);
		#endif // Move immediate to memory (memory designated by HL)
        uint16_t addr = (cpu->h << 8) | cpu->l;
        cpu->memory[addr] = code[1];
        cpu->pc += 2;
        cpu->cycles += 10;
        break;
    }
    case 0x37:
    {
        #ifdef DEBUG
			printf("STC");
		#endif // Sets the Carry flag bit in the status register to 1
        cpu->flags.c = 1;
        cpu->pc += 1;
        cpu->cycles += 4;
        break;
    }
    case 0x38:
    {
        #ifdef DEBUG
			printf("NOP");
		#endif
        cpu->pc += 1;
        cpu->cycles += 4;
        break;
    }
    case 0x39:
    {
        #ifdef DEBUG
			printf("DAD    SP");
		#endif // Add stack pointer to HL (memory)
        uint16_t hl = (cpu->h << 8) | cpu->l;
        uint32_t answer = hl + cpu->sp;
        cpu->flags.c = (answer > 0xFFFF);
        cpu->h = (answer >> 8) & 0xFF;
        cpu->l = answer & 0xFF;
        cpu->pc += 1;
        cpu->cycles += 10;
        break;
    }
    case 0x3a:
    {
        uint16_t addr = (code[2] << 8) | code[1];
        #ifdef DEBUG
			printf("LDA    addr: %04x", addr);
		#endif // Load into A direct from memory
        cpu->a = cpu->memory[addr];
        cpu->pc += 3;
        cpu->cycles += 13;
        break;
    }
    case 0x3b:
    {
        #ifdef DEBUG
			printf("DCX    SP");
		#endif // Decrement SP
        cpu->sp -= 1;
        cpu->pc += 1;
        cpu->cycles += 5;
        break;
    }
    case 0x3c:
    {
        #ifdef DEBUG
			printf("INR    A");
		#endif // Increment A
        cpu->flags.ac = ((cpu->a & 0x0F) + 1) > 0x0F;
        cpu->a += 1;
        setZSPflags(cpu, cpu->a);
        cpu->pc += 1;
        cpu->cycles += 5;
        break;
    }
    case 0x3d:
    {
        #ifdef DEBUG
			printf("DCR    A");
		#endif // Decrement A
        cpu->flags.ac = ((cpu->a & 0x0F) == 0);
        cpu->a -= 1;
        setZSPflags(cpu, cpu->a);
        cpu->pc += 1;
        cpu->cycles += 5;
        break;
    }
    case 0x3e:
    {
        #ifdef DEBUG
			printf("MVI    A, %02x", code[1]);
		#endif // Move immediate register to A
        cpu->a = code[1];
        cpu->pc += 2;
        cpu->cycles += 7;
        break;
    }
    case 0x3f:
    {
        #ifdef DEBUG
			printf("CMC");
		#endif // Complement carry: flips value of the carry flag
        if (cpu->flags.c == 0) { cpu->flags.c = 1; }
        else { cpu->flags.c = 0; }
        cpu->pc += 1;
        cpu->cycles += 4;
        break;
    }
    case 0x40:
    {
        #ifdef DEBUG
			printf("MOV    B, B");
		#endif // Move register B to register B
        cpu->b = cpu->b;
        cpu->pc += 1;
        cpu->cycles += 5;
        break;
    }
    case 0x41:
    {
        #ifdef DEBUG
			printf("MOV    B, C");
		#endif // Move register C to register B
        cpu->b = cpu->c;
        cpu->pc += 1;
        cpu->cycles += 5;
        break;
    }
    case 0x42:
    {
        #ifdef DEBUG
			printf("MOV    B, D");
		#endif // Move register D to register B
        cpu->b = cpu->d;
        cpu->pc += 1;
        cpu->cycles += 5;
        break;
    }
    case 0x43:
    {
        #ifdef DEBUG
			printf("MOV    B, E");
		#endif // Move register E to register B
        cpu->b = cpu->e;
        cpu->pc += 1;
        cpu->cycles += 5;
        break;
    }
    case 0x44:
    {
        #ifdef DEBUG
			printf("MOV    B, H");
		#endif // Move register H to register B
        cpu->b = cpu->h;
        cpu->pc += 1;
        cpu->cycles += 5;
        break;
    }
    case 0x45:
    {
        #ifdef DEBUG
			printf("MOV    B, L");
		#endif // Move register L to register B
        cpu->b = cpu->l;
        cpu->pc += 1;
        cpu->cycles += 5;
        break;
    }
    case 0x46:
    {
        #ifdef DEBUG
			printf("MOV    B, M");
		#endif // Move value at memory (HL) to register B
        cpu->b = cpu->memory[(cpu->h << 8) | cpu->l];
        cpu->pc += 1;
        cpu->cycles += 7;
        break;
    }
    case 0x47:
    {
        #ifdef DEBUG
			printf("MOV    B, A");
		#endif // Move register A to register B
        cpu->b = cpu->a;
        cpu->pc += 1;
        cpu->cycles += 5;
        break;
    }
    case 0x48:
    {
        #ifdef DEBUG
			printf("MOV    C, B");
		#endif
        cpu->c = cpu->b;
        cpu->pc += 1;
        cpu->cycles += 5;
        break;
    }
    case 0x49:
    {
        #ifdef DEBUG
			printf("MOV    C, C");
		#endif
        cpu->c = cpu->c;
        cpu->pc += 1;
        cpu->cycles += 5;
        break;
    }
    case 0x4a:
    {
        #ifdef DEBUG
			printf("MOV    C, D");
		#endif
        cpu->c = cpu->d;
        cpu->pc += 1;
        cpu->cycles += 5;
        break;
    }
    case 0x4b:
    {
        #ifdef DEBUG
			printf("MOV    C, E");
		#endif
        cpu->c = cpu->e;
        cpu->pc += 1;
        cpu->cycles += 5;
        break;
    }
    case 0x4c:
    {
        #ifdef DEBUG
			printf("MOV    C, H");
		#endif
        cpu->c = cpu->h;
        cpu->pc += 1;
        cpu->cycles += 5;
        break;
    }
    case 0x4d:
    {
        #ifdef DEBUG
			printf("MOV    C, L");
		#endif
        cpu->c = cpu->l;
        cpu->pc += 1;
        cpu->cycles += 5;
        break;
    }
    case 0x4e:
    {
        #ifdef DEBUG
			printf("MOV    C, M");
		#endif
        cpu->c = cpu->memory[(cpu->h << 8) | cpu->l];
        cpu->pc += 1;
        cpu->cycles += 7;
        break;
    }
    case 0x4f:
    {
        #ifdef DEBUG
			printf("MOV    C, A");
		#endif
        cpu->c = cpu->a;
        cpu->pc += 1;
        cpu->cycles += 5;
        break;
    }
    case 0x50:
    {
        #ifdef DEBUG
			printf("MOV    D, B");
		#endif
        cpu->d = cpu->b;
        cpu->pc += 1;
        cpu->cycles += 5;
        break;
    }
    case 0x51:
    {
        #ifdef DEBUG
			printf("MOV    D, C");
		#endif
        cpu->d = cpu->c;
        cpu->pc += 1;
        cpu->cycles += 5;
        break;
    }
    case 0x52:
    {
        #ifdef DEBUG
			printf("MOV    D, D");
		#endif
        cpu->d = cpu->d;
        cpu->pc += 1;
        cpu->cycles += 5;
        break;
    }
    case 0x53:
    {
        #ifdef DEBUG
			printf("MOV    D, E");
		#endif
        cpu->d = cpu->e;
        cpu->pc += 1;
        cpu->cycles += 5;
        break;
    }
    case 0x54:
    {
        #ifdef DEBUG
			printf("MOV    D, H");
		#endif
        cpu->d = cpu->h;
        cpu->pc += 1;
        cpu->cycles += 5;
        break;
    }
    case 0x55:
    {
        #ifdef DEBUG
			printf("MOV    D, L");
		#endif
        cpu->d = cpu->l;
        cpu->pc += 1;
        cpu->cycles += 5;
        break;
    }
    case 0x56:
    {
        #ifdef DEBUG
			printf("MOV    D, M");
		#endif
        cpu->d = cpu->memory[(cpu->h << 8) | cpu->l];
        cpu->pc += 1;
        cpu->cycles += 7;
        break;
    }
    case 0x57:
    {
        #ifdef DEBUG
			printf("MOV    D, A");
		#endif
        cpu->d = cpu->a;
        cpu->pc += 1;
        cpu->cycles += 5;
        break;
    }
    case 0x58:
    {
        #ifdef DEBUG
			printf("MOV    E, B");
		#endif
        cpu->e = cpu->b;
        cpu->pc += 1;
        cpu->cycles += 5;
        break;
    }
    case 0x59:
    {
        #ifdef DEBUG
			printf("MOV    E, C");
		#endif
        cpu->e = cpu->c;
        cpu->pc += 1;
        cpu->cycles += 5;
        break;
    }
    case 0x5a:
    {
        #ifdef DEBUG
			printf("MOV    E, D");
		#endif
        cpu->e = cpu->d;
        cpu->pc += 1;
        cpu->cycles += 5;
        break;
    }
    case 0x5b:
    {
        #ifdef DEBUG
			printf("MOV    E, E");
		#endif
        cpu->e = cpu->e;
        cpu->pc += 1;
        cpu->cycles += 5;
        break;
    }
    case 0x5c:
    {
        #ifdef DEBUG
			printf("MOV    E, H");
		#endif
        cpu->e = cpu->h;
        cpu->pc += 1;
        cpu->cycles += 5;
        break;
    }
    case 0x5d:
    {
        #ifdef DEBUG
			printf("MOV    E, L");
		#endif
        cpu->e = cpu->l;
        cpu->pc += 1;
        cpu->cycles += 5;
        break;
    }
    case 0x5e:
    {
        #ifdef DEBUG
			printf("MOV    E, M");
		#endif
        cpu->e = cpu->memory[(cpu->h << 8) | cpu->l];
        cpu->pc += 1;
        cpu->cycles += 7;
        break;
    }
    case 0x5f:
    {
        #ifdef DEBUG
			printf("MOV    E, A");
		#endif
        cpu->e = cpu->a;
        cpu->pc += 1;
        cpu->cycles += 5;
        break;
    }
    case 0x60:
    {
        #ifdef DEBUG
			printf("MOV    H, B");
		#endif
        cpu->h = cpu->b;
        cpu->pc += 1;
        cpu->cycles += 5;
        break;
    }
    case 0x61:
    {
        #ifdef DEBUG
			printf("MOV    H, C");
		#endif
        cpu->h = cpu->c;
        cpu->pc += 1;
        cpu->cycles += 5;
        break;
    }
    case 0x62:
    {
        #ifdef DEBUG
			printf("MOV    H, D");
		#endif
        cpu->h = cpu->d;
        cpu->pc += 1;
        cpu->cycles += 5;
        break;
    }
    case 0x63:
    {
        #ifdef DEBUG
			printf("MOV    H, E");
		#endif
        cpu->h = cpu->e;
        cpu->pc += 1;
        cpu->cycles += 5;
        break;
    }
    case 0x64:
    {
        #ifdef DEBUG
			printf("MOV    H, H");
		#endif
        cpu->h = cpu->h;
        cpu->pc += 1;
        cpu->cycles += 5;
        break;
    }
    case 0x65:
    {
        #ifdef DEBUG
			printf("MOV    H, L");
		#endif
        cpu->h = cpu->l;
        cpu->pc += 1;
        cpu->cycles += 5;
        break;
    }
    case 0x66:
    {
        #ifdef DEBUG
			printf("MOV    H, M");
		#endif
        cpu->h = cpu->memory[(cpu->h << 8) | cpu->l];
        cpu->pc += 1;
        cpu->cycles += 7;
        break;
    }
    case 0x67:
    {
        #ifdef DEBUG
			printf("MOV    H, A");
		#endif
        cpu->h = cpu->a;
        cpu->pc += 1;
        cpu->cycles += 5;
        break;
    }
    case 0x68:
    {
        #ifdef DEBUG
			printf("MOV    L, B");
		#endif
        cpu->l = cpu->b;
        cpu->pc += 1;
        cpu->cycles += 5;
        break;
    }
    case 0x69:
    {
        #ifdef DEBUG
			printf("MOV    L, C");
		#endif
        cpu->l = cpu->c;
        cpu->pc += 1;
        cpu->cycles += 5;
        break;
    }
    case 0x6a:
    {
        #ifdef DEBUG
			printf("MOV    L, D");
		#endif
        cpu->l = cpu->d;
        cpu->pc += 1;
        cpu->cycles += 5;
        break;
    }
    case 0x6b:
    {
        #ifdef DEBUG
			printf("MOV    L, E");
		#endif
        cpu->l = cpu->e;
        cpu->pc += 1;
        cpu->cycles += 5;
        break;
    }
    case 0x6c:
    {
        #ifdef DEBUG
			printf("MOV    L, H");
		#endif
        cpu->l = cpu->h;
        cpu->pc += 1;
        cpu->cycles += 5;
        break;
    }
    case 0x6d:
    {
        #ifdef DEBUG
			printf("MOV    L, L");
		#endif
        cpu->l = cpu->l;
        cpu->pc += 1;
        cpu->cycles += 5;
        break;
    }
    case 0x6e:
    {
        #ifdef DEBUG
			printf("MOV    L, M");
		#endif
        cpu->l = cpu->memory[(cpu->h << 8) | cpu->l];
        cpu->pc += 1;
        cpu->cycles += 7;
        break;
    }
    case 0x6f:
    {
        #ifdef DEBUG
			printf("MOV    L, A");
		#endif
        cpu->l = cpu->a;
        cpu->pc += 1;
        cpu->cycles += 5;
        break;
    }
    case 0x70:
    {
        #ifdef DEBUG
			printf("MOV    M, B");
		#endif
        cpu->memory[(cpu->h << 8) | cpu->l] = cpu->b;
        cpu->pc += 1; cpu->cycles += 7;
        break;
    }
    case 0x71:
    {
        #ifdef DEBUG
			printf("MOV    M, C");
		#endif
        cpu->memory[(cpu->h << 8) | cpu->l] = cpu->c;
        cpu->pc += 1;
        cpu->cycles += 7;
        break;
    }
    case 0x72:
    {
        #ifdef DEBUG
			printf("MOV    M, D");
		#endif
        cpu->memory[(cpu->h << 8) | cpu->l] = cpu->d;
        cpu->pc += 1;
        cpu->cycles += 7;
        break;
    }
    case 0x73:
    {
        #ifdef DEBUG
			printf("MOV    M, E");
		#endif
        cpu->memory[(cpu->h << 8) | cpu->l] = cpu->e;
        cpu->pc += 1;
        cpu->cycles += 7;
        break;
    }
    case 0x74:
    {
        #ifdef DEBUG
			printf("MOV    M, H");
		#endif
        cpu->memory[(cpu->h << 8) | cpu->l] = cpu->h;
        cpu->pc += 1;
        cpu->cycles += 7;
        break;
    }
    case 0x75:
    {
        #ifdef DEBUG
			printf("MOV    M, L");
		#endif
        cpu->memory[(cpu->h << 8) | cpu->l] = cpu->l;
        cpu->pc += 1;
        cpu->cycles += 7;
        break;
    }
    case 0x76:
    {
        /*
        tops the CPU's execution until an external interrupt or reset signal is received.
        The CPU will remain in a waiting state, essentially doing nothing, until an external event signals it to resume operation.
        */
        #ifdef DEBUG
			printf("HLT");
		#endif // Halt
        cpu->halted = true;
        cpu->pc += 1;
        cpu->cycles += 7;
        break;
    }
    case 0x77:
    {
        #ifdef DEBUG
			printf("MOV    M, A");
		#endif
        cpu->memory[(cpu->h << 8) | cpu->l] = cpu->a;
        cpu->pc += 1;
        cpu->cycles += 7;
        break;
    }
    case 0x78:
    {
        #ifdef DEBUG
			printf("MOV    A, B");
		#endif
        cpu->a = cpu->b;
        cpu->pc += 1;
        cpu->cycles += 5;
        break;
    }
    case 0x79:
    {
        #ifdef DEBUG
			printf("MOV    A, C");
		#endif
        cpu->a = cpu->c;
        cpu->pc += 1;
        cpu->cycles += 5;
        break;
    }
    case 0x7a:
    {
        #ifdef DEBUG
			printf("MOV    A, D");
		#endif
        cpu->a = cpu->d;
        cpu->pc += 1;
        cpu->cycles += 5;
        break;
    }
    case 0x7b:
    {
        #ifdef DEBUG
			printf("MOV    A, E");
		#endif
        cpu->a = cpu->e;
        cpu->pc += 1;
        cpu->cycles += 5;
        break;
    }
    case 0x7c:
    {
        #ifdef DEBUG
			printf("MOV    A, H");
		#endif
        cpu->a = cpu->h;
        cpu->pc += 1;
        cpu->cycles += 5;
        break;
    }
    case 0x7d:
    {
        #ifdef DEBUG
			printf("MOV    A, L");
		#endif
        cpu->a = cpu->l;
        cpu->pc += 1;
        cpu->cycles += 5;
        break;
    }
    case 0x7e:
    {
        #ifdef DEBUG
			printf("MOV    A, M");
		#endif
        cpu->a = cpu->memory[(cpu->h << 8) | cpu->l];
        cpu->pc += 1;
        cpu->cycles += 7;
        break;
    }
    case 0x7f:
    {
        #ifdef DEBUG
			printf("MOV    A, A");
		#endif
        cpu->a = cpu->a;
        cpu->pc += 1;
        cpu->cycles += 5;
        break;
    }
    case 0x80:
    {
        #ifdef DEBUG
			printf("ADD    B");
		#endif // Adds contents of register B to register A
        uint16_t answer = cpu->a + cpu->b;
        cpu->flags.c = (answer > 0xFF);
        cpu->flags.ac = ((cpu->a & 0x0F) + (cpu->b & 0x0F)) > 0x0F; // Auxiliary carry
        cpu->a = answer & 0xFF;
        setZSPflags(cpu, cpu->a);
        cpu->pc += 1;
        cpu->cycles += 4;
        break;
    }
    case 0x81:
    {
        #ifdef DEBUG
			printf("ADD    C");
		#endif // Adds contents of register C to register A
        uint16_t answer = cpu->a + cpu->c;
        cpu->flags.c = (answer > 0xFF);
        cpu->flags.ac = ((cpu->a & 0x0F) + (cpu->c & 0x0F)) > 0x0F; // Auxiliary carry
        cpu->a = answer & 0xFF;
        setZSPflags(cpu, cpu->a);
        cpu->pc += 1;
        cpu->cycles += 4;
        break;
    }
    case 0x82:
    {
        #ifdef DEBUG
			printf("ADD    D");
		#endif // Adds contents of register D to register A
        uint16_t answer = cpu->a + cpu->d;
        cpu->flags.c = (answer > 0xFF);
        cpu->flags.ac = ((cpu->a & 0x0F) + (cpu->d & 0x0F)) > 0x0F; // Auxiliary carry
        cpu->a = answer & 0xFF;
        setZSPflags(cpu, cpu->a);
        cpu->pc += 1;
        cpu->cycles += 4;
        break;
    }
    case 0x83:
    {
        #ifdef DEBUG
			printf("ADD    E");
		#endif // Adds contents of register E to register A
        uint16_t answer = cpu->a + cpu->e;
        cpu->flags.c = (answer > 0xFF);
        cpu->flags.ac = ((cpu->a & 0x0F) + (cpu->e & 0x0F)) > 0x0F; // Auxiliary carry
        cpu->a = answer & 0xFF;
        setZSPflags(cpu, cpu->a);
        cpu->pc += 1;
        cpu->cycles += 4;
        break;
    }
    case 0x84:
    {
        #ifdef DEBUG
			printf("ADD    H");
		#endif // Adds contents of register H to register A
        uint16_t answer = cpu->a + cpu->h;
        cpu->flags.c = (answer > 0xFF);
        cpu->flags.ac = ((cpu->a & 0x0F) + (cpu->h & 0x0F)) > 0x0F; // Auxiliary carry
        cpu->a = answer & 0xFF;
        setZSPflags(cpu, cpu->a);
        cpu->pc += 1;
        cpu->cycles += 4;
        break;
    }
    case 0x85:
    {
        #ifdef DEBUG
			printf("ADD    L");
		#endif // Adds contents of register L to register A
        uint16_t answer = cpu->a + cpu->l;
        cpu->flags.c = (answer > 0xFF);
        cpu->flags.ac = ((cpu->a & 0x0F) + (cpu->l & 0x0F)) > 0x0F; // Auxiliary carry
        cpu->a = answer & 0xFF;
        setZSPflags(cpu, cpu->a);
        cpu->pc += 1;
        cpu->cycles += 4;
        break;
    }
    case 0x86:
    {
        #ifdef DEBUG
			printf("ADD    M");
		#endif // Adds contents of register M to register A
        uint16_t addr = (cpu->h << 8) | cpu->l;
        uint8_t value = cpu->memory[addr];
        uint16_t answer = cpu->a + value;
        cpu->flags.c = (answer > 0xFF);
        cpu->flags.ac = ((cpu->a & 0x0F) + (value & 0x0F)) > 0x0F; // Auxiliary carry
        cpu->a = answer & 0xFF;
        setZSPflags(cpu, cpu->a);
        cpu->pc += 1;
        cpu->cycles += 7;
        break;
    }
    case 0x87:
    {
        #ifdef DEBUG
			printf("ADD    A");
		#endif // Adds contents of register A to register A
        uint16_t answer = cpu->a + cpu->a;
        cpu->flags.c = (answer > 0xFF);
        cpu->flags.ac = ((cpu->a & 0x0F) + (cpu->a & 0x0F)) > 0x0F; // Auxiliary carry
        cpu->a = answer & 0xFF;
        setZSPflags(cpu, cpu->a);
        cpu->pc += 1;
        cpu->cycles += 4;
        break;
    }
    case 0x88:
    {
        #ifdef DEBUG
			printf("ADC    B");
		#endif // Adds contents of register B to register A with carry
        uint16_t answer = cpu->a + cpu->b + cpu->flags.c;
        cpu->flags.c = (answer > 0xFF);
        cpu->flags.ac = ((cpu->a & 0x0F) + (cpu->b & 0x0F) + cpu->flags.c) > 0x0F; // Auxiliary carry
        cpu->a = answer & 0xFF;
        setZSPflags(cpu, cpu->a);
        cpu->pc += 1;
        cpu->cycles += 4;
        break;
    }
    case 0x89:
    {
        #ifdef DEBUG
			printf("ADC    C");
		#endif // Adds contents of register C to register A with carry
        uint16_t answer = cpu->a + cpu->c + cpu->flags.c;
        cpu->flags.c = (answer > 0xFF);
        cpu->flags.ac = ((cpu->a & 0x0F) + (cpu->c & 0x0F) + cpu->flags.c) > 0x0F; // Auxiliary carry
        cpu->a = answer & 0xFF;
        setZSPflags(cpu, cpu->a);
        cpu->pc += 1;
        cpu->cycles += 4;
        break;
    }
    case 0x8a:
    {
        #ifdef DEBUG
			printf("ADC    D");
		#endif // Adds contents of register D to register A with carry
        uint16_t answer = cpu->a + cpu->d + cpu->flags.c;
        cpu->flags.c = (answer > 0xFF);
        cpu->flags.ac = ((cpu->a & 0x0F) + (cpu->d & 0x0F) + cpu->flags.c) > 0x0F; // Auxiliary carry
        cpu->a = answer & 0xFF;
        setZSPflags(cpu, cpu->a);
        cpu->pc += 1;
        cpu->cycles += 4;
        break;
    }
    case 0x8b:
    {
        #ifdef DEBUG
			printf("ADC    E");
		#endif // Adds contents of register E to register A with carry
        uint16_t answer = cpu->a + cpu->e + cpu->flags.c;
        cpu->flags.c = (answer > 0xFF);
        cpu->flags.ac = ((cpu->a & 0x0F) + (cpu->e & 0x0F) + cpu->flags.c) > 0x0F; // Auxiliary carry
        cpu->a = answer & 0xFF;
        setZSPflags(cpu, cpu->a);
        cpu->pc += 1;
        cpu->cycles += 4;
        break;
    }
    case 0x8c:
    {
        #ifdef DEBUG
			printf("ADC    H");
		#endif // Adds contents of register H to register A with carry
        uint16_t answer = cpu->a + cpu->h + cpu->flags.c;
        cpu->flags.c = (answer > 0xFF);
        cpu->flags.ac = ((cpu->a & 0x0F) + (cpu->h & 0x0F) + cpu->flags.c) > 0x0F; // Auxiliary carry
        cpu->a = answer & 0xFF;
        setZSPflags(cpu, cpu->a);
        cpu->pc += 1;
        cpu->cycles += 4;
        break;
    }
    case 0x8d:
    {
        #ifdef DEBUG
			printf("ADC    L");
		#endif // Adds contents of register L to register A with carry
        uint16_t answer = cpu->a + cpu->l + cpu->flags.c;
        cpu->flags.c = (answer > 0xFF);
        cpu->flags.ac = ((cpu->a & 0x0F) + (cpu->l & 0x0F) + cpu->flags.c) > 0x0F; // Auxiliary carry
        cpu->a = answer & 0xFF;
        setZSPflags(cpu, cpu->a);
        cpu->pc += 1;
        cpu->cycles += 4;
        break;
    }
    case 0x8e:
    {
        #ifdef DEBUG
			printf("ADC    M");
		#endif // Adds contents of register M to register A with carry
        uint16_t addr = (cpu->h << 8) | cpu->l;
        uint8_t value = cpu->memory[addr];
        uint16_t answer = cpu->a + value + cpu->flags.c;
        cpu->flags.c = (answer > 0xFF);
        cpu->flags.ac = ((cpu->a & 0x0F) + (value & 0x0F) + cpu->flags.c) > 0x0F; // Auxiliary carry
        cpu->a = answer & 0xFF;
        setZSPflags(cpu, cpu->a);
        cpu->pc += 1;
        cpu->cycles += 7;
        break;
    }
    case 0x8f:
    {
        #ifdef DEBUG
			printf("ADC    A");
		#endif // Adds contents of register A to register A with carry
        uint16_t answer = cpu->a + cpu->a + cpu->flags.c;
        cpu->flags.c = (answer > 0xFF);
        cpu->flags.ac = ((cpu->a & 0x0F) + (cpu->a & 0x0F) + cpu->flags.c) > 0x0F; // Auxiliary carry
        cpu->a = answer & 0xFF;
        setZSPflags(cpu, cpu->a);
        cpu->pc += 1;
        cpu->cycles += 4;
        break;
    }
    case 0x90:
    {
        #ifdef DEBUG
			printf("SUB    B");
		#endif // Subtracts contents of register B from register A
        uint16_t answer = (uint16_t)cpu->a - (uint16_t)cpu->b;
        cpu->flags.ac = (cpu->a & 0x0F) < (cpu->b & 0x0F);
        cpu->flags.c = (cpu->a < cpu->b);
        cpu->a = answer & 0xFF;
        setZSPflags(cpu, cpu->a);
        cpu->pc += 1;
        cpu->cycles += 4;
        break;
    }
    case 0x91:
    {
        #ifdef DEBUG
			printf("SUB    C");
		#endif // Subtracts contents of register C from register A
        uint16_t answer = (uint16_t)cpu->a - (uint16_t)cpu->c;
        cpu->flags.ac = (cpu->a & 0x0F) < (cpu->c & 0x0F);
        cpu->flags.c = (cpu->a < cpu->c);
        cpu->a = answer & 0xFF;
        setZSPflags(cpu, cpu->a);
        cpu->pc += 1;
        cpu->cycles += 4;
        break;
    }
    case 0x92:
    {
        #ifdef DEBUG
			printf("SUB    D");
		#endif // Subtracts contents of register D from register A
        uint16_t answer = (uint16_t)cpu->a - (uint16_t)cpu->d;
        cpu->flags.ac = (cpu->a & 0x0F) < (cpu->d & 0x0F);
        cpu->flags.c = (cpu->a < cpu->d);
        cpu->a = answer & 0xFF;
        setZSPflags(cpu, cpu->a);
        cpu->pc += 1;
        cpu->cycles += 4;
        break;
    }
    case 0x93:
    {
        #ifdef DEBUG
			printf("SUB    E");
		#endif // Subtracts contents of register E from register A
        uint16_t answer = (uint16_t)cpu->a - (uint16_t)cpu->e;
        cpu->flags.ac = (cpu->a & 0x0F) < (cpu->e & 0x0F);
        cpu->flags.c = (cpu->a < cpu->e);
        cpu->a = answer & 0xFF;
        setZSPflags(cpu, cpu->a);
        cpu->pc += 1;
        cpu->cycles += 4;
        break;
    }
    case 0x94:
    {
        #ifdef DEBUG
			printf("SUB    H");
		#endif // Subtracts contents of register H from register A
        uint16_t answer = (uint16_t)cpu->a - (uint16_t)cpu->h;
        cpu->flags.ac = (cpu->a & 0x0F) < (cpu->h & 0x0F);
        cpu->flags.c = (cpu->a < cpu->h);
        cpu->a = answer & 0xFF;
        setZSPflags(cpu, cpu->a);
        cpu->pc += 1;
        cpu->cycles += 4;
        break;
    }
    case 0x95:
    {
        #ifdef DEBUG
			printf("SUB    L");
		#endif // Subtracts contents of register L from register A
        uint16_t answer = (uint16_t)cpu->a - (uint16_t)cpu->l;
        cpu->flags.ac = (cpu->a & 0x0F) < (cpu->l & 0x0F);
        cpu->flags.c = (cpu->a < cpu->l);
        cpu->a = answer & 0xFF;
        setZSPflags(cpu, cpu->a);
        cpu->pc += 1;
        cpu->cycles += 4;
        break;
    }
    case 0x96:
    {
        #ifdef DEBUG
			printf("SUB    M");
		#endif // Subtracts contents of memory[HL] from register A
        uint16_t addr = (cpu->h << 8) | cpu->l;
        uint16_t value = cpu->memory[addr];
        uint16_t answer = (uint16_t)cpu->a - value;
        cpu->flags.ac = (cpu->a & 0x0F) < (value & 0x0F);
        cpu->flags.c = (cpu->a < value);
        cpu->a = answer & 0xFF;
        setZSPflags(cpu, cpu->a);
        cpu->pc += 1;
        cpu->cycles += 7;
        break;
    }
    case 0x97:
    {
        #ifdef DEBUG
			printf("SUB    A");
		#endif // Subtracts contents of register A from register A
        uint16_t answer = (uint16_t)cpu->a - (uint16_t)cpu->a;
        cpu->flags.ac = (cpu->a & 0x0F) < (cpu->a & 0x0F);
        cpu->flags.c = (cpu->a < cpu->a);
        cpu->a = answer & 0xFF;
        setZSPflags(cpu, cpu->a);
        cpu->pc += 1;
        cpu->cycles += 4;
        break;
    }
    case 0x98:
    {
        #ifdef DEBUG
			printf("SBB    B");
		#endif // Subtracts contents of register B from register A with borrow
        uint16_t answer = (uint16_t)cpu->a - (uint16_t)cpu->b - cpu->flags.c;
        cpu->flags.ac = (cpu->a & 0x0F) < ((cpu->b + cpu->flags.c) & 0x0F);
        cpu->flags.c = (cpu->a < (cpu->b + cpu->flags.c));
        cpu->a = answer & 0xFF;
        setZSPflags(cpu, cpu->a);
        cpu->pc += 1;
        cpu->cycles += 4;
        break;
    }
    case 0x99:
    {
        #ifdef DEBUG
			printf("SBB    C");
		#endif // Subtracts contents of register C from register A with borrow
        uint16_t answer = (uint16_t)cpu->a - (uint16_t)cpu->c - cpu->flags.c;
        cpu->flags.ac = (cpu->a & 0x0F) < ((cpu->c + cpu->flags.c) & 0x0F);
        cpu->flags.c = (cpu->a < (cpu->c + cpu->flags.c));
        cpu->a = answer & 0xFF;
        setZSPflags(cpu, cpu->a);
        cpu->pc += 1;
        cpu->cycles += 4;
        break;
    }
    case 0x9a:
    {
        #ifdef DEBUG
			printf("SBB    D");
		#endif // Subtracts contents of register D from register A with borrow
        uint16_t answer = (uint16_t)cpu->a - (uint16_t)cpu->d - cpu->flags.c;
        cpu->flags.ac = (cpu->a & 0x0F) < ((cpu->d + cpu->flags.c) & 0x0F);
        cpu->flags.c = (cpu->a < (cpu->d + cpu->flags.c));
        cpu->a = answer & 0xFF;
        setZSPflags(cpu, cpu->a);
        cpu->pc += 1;
        cpu->cycles += 4;
        break;
    }
    case 0x9b:
    {
        #ifdef DEBUG
			printf("SBB    E");
		#endif // Subtracts contents of register E from register A with borrow
        uint16_t answer = (uint16_t)cpu->a - (uint16_t)cpu->e - cpu->flags.c;
        cpu->flags.ac = (cpu->a & 0x0F) < ((cpu->e + cpu->flags.c) & 0x0F);
        cpu->flags.c = (cpu->a < (cpu->e + cpu->flags.c));
        cpu->a = answer & 0xFF;
        setZSPflags(cpu, cpu->a);
        cpu->pc += 1;
        cpu->cycles += 4;
        break;
    }
    case 0x9c:
    {
        #ifdef DEBUG
			printf("SBB    H");
		#endif // Subtracts contents of register H from register A with borrow
        uint16_t answer = (uint16_t)cpu->a - (uint16_t)cpu->h - cpu->flags.c;
        cpu->flags.ac = (cpu->a & 0x0F) < ((cpu->h + cpu->flags.c) & 0x0F);
        cpu->flags.c = (cpu->a < (cpu->h + cpu->flags.c));
        cpu->a = answer & 0xFF;
        setZSPflags(cpu, cpu->a);
        cpu->pc += 1;
        cpu->cycles += 4;
        break;
    }
    case 0x9d:
    {
        #ifdef DEBUG
			printf("SBB    L");
		#endif // Subtracts contents of register L from register A with borrow
        uint16_t answer = (uint16_t)cpu->a - (uint16_t)cpu->l - cpu->flags.c;
        cpu->flags.ac = (cpu->a & 0x0F) < ((cpu->l + cpu->flags.c) & 0x0F);
        cpu->flags.c = (cpu->a < (cpu->l + cpu->flags.c));
        cpu->a = answer & 0xFF;
        setZSPflags(cpu, cpu->a);
        cpu->pc += 1;
        cpu->cycles += 4;
        break;
    }
    case 0x9e:
    {
        #ifdef DEBUG
			printf("SBB    M");
		#endif // Subtracts contents of register M from register A with borrow
        uint16_t addr = (cpu->h << 8) | cpu->l;
        uint16_t value = cpu->memory[addr];
        uint16_t answer = (uint16_t)cpu->a - value - cpu->flags.c;
        cpu->flags.ac = (cpu->a & 0x0F) < ((value + cpu->flags.c) & 0x0F);
        cpu->flags.c = (cpu->a < (value + cpu->flags.c));
        cpu->a = answer & 0xFF;
        setZSPflags(cpu, cpu->a);
        cpu->pc += 1;
        cpu->cycles += 7;
        break;
    }
    case 0x9f:
    {
        #ifdef DEBUG
			printf("SBB    A");
		#endif // Subtracts contents of register A from register A with borrow
        uint16_t answer = (uint16_t)cpu->a - (uint16_t)cpu->a - cpu->flags.c;
        cpu->flags.ac = (cpu->a & 0x0F) < ((cpu->a + cpu->flags.c) & 0x0F);
        cpu->flags.c = (cpu->a < (cpu->a + cpu->flags.c));
        cpu->a = answer & 0xFF;
        setZSPflags(cpu, cpu->a);
        cpu->pc += 1;
        cpu->cycles += 4;
        break;
    }
    case 0xa0:
    {
        #ifdef DEBUG
			printf("ANA    B");
		#endif // Register B AND register A
        cpu->a = cpu->a & cpu->b;
        cpu->flags.c = 0;
        cpu->flags.ac = ((cpu->a | cpu->b) & 0x08) != 0; // Not sure if this is correct, but space-invaders does not use auxiliary carry
        setZSPflags(cpu, cpu->a);
        cpu->pc += 1;
        cpu->cycles += 4;
        break;
    }
    case 0xa1:
    {
        #ifdef DEBUG
			printf("ANA    C");
		#endif // Register C AND register A
        cpu->a = cpu->a & cpu->c;
        cpu->flags.c = 0;
        cpu->flags.ac = ((cpu->a | cpu->c) & 0x08) != 0; // Not sure if this is correct, but space-invaders does not use auxiliary carry
        setZSPflags(cpu, cpu->a);
        cpu->pc += 1;
        cpu->cycles += 4;
        break;
    }
    case 0xa2:
    {
        #ifdef DEBUG
			printf("ANA    D");
		#endif // Register D AND register A
        cpu->a = cpu->a & cpu->d;
        cpu->flags.c = 0;
        cpu->flags.ac = ((cpu->a | cpu->d) & 0x08) != 0; // Not sure if this is correct, but space-invaders does not use auxiliary carry
        setZSPflags(cpu, cpu->a);
        cpu->pc += 1;
        cpu->cycles += 4;
        break;
    }
    case 0xa3:
    {
        #ifdef DEBUG
			printf("ANA    E");
		#endif // Register E AND register A
        cpu->a = cpu->a & cpu->e;
        cpu->flags.c = 0;
        cpu->flags.ac = ((cpu->a | cpu->e) & 0x08) != 0; // Not sure if this is correct, but space-invaders does not use auxiliary carry
        setZSPflags(cpu, cpu->a);
        cpu->pc += 1;
        cpu->cycles += 4;
        break;
    }
    case 0xa4:
    {
        #ifdef DEBUG
			printf("ANA    H");
		#endif // Register H AND register A
        cpu->a = cpu->a & cpu->h;
        cpu->flags.c = 0;
        cpu->flags.ac = ((cpu->a | cpu->h) & 0x08) != 0; // Not sure if this is correct, but space-invaders does not use auxiliary carry
        setZSPflags(cpu, cpu->a);
        cpu->pc += 1;
        cpu->cycles += 4;
        break;
    }
    case 0xa5:
    {
        #ifdef DEBUG
			printf("ANA    L");
		#endif // Register L AND register A
        cpu->a = cpu->a & cpu->l;
        cpu->flags.c = 0;
        cpu->flags.ac = ((cpu->a | cpu->l) & 0x08) != 0; // Not sure if this is correct, but space-invaders does not use auxiliary carry
        setZSPflags(cpu, cpu->a);
        cpu->pc += 1;
        cpu->cycles += 4;
        break;
    }
    case 0xa6:
    {
        #ifdef DEBUG
			printf("ANA    M");
		#endif // Register M AND register A
        uint16_t addr = (cpu->h << 8) | cpu->l;
        uint8_t value = cpu->memory[addr];
        cpu->a = cpu->a & value;
        cpu->flags.c = 0;
        cpu->flags.ac = ((cpu->a | value) & 0x08) != 0; // Not sure if this is correct, but space-invaders does not use auxiliary carry
        setZSPflags(cpu, cpu->a);
        cpu->pc += 1;
        cpu->cycles += 7;
        break;
    }
    case 0xa7:
    {
        #ifdef DEBUG
			printf("ANA    A");
		#endif // Register A AND register A
        cpu->a = cpu->a & cpu->a;
        cpu->flags.c = 0;
        cpu->flags.ac = ((cpu->a | cpu->a) & 0x08) != 0; // Not sure if this is correct, but space-invaders does not use auxiliary carry
        setZSPflags(cpu, cpu->a);
        cpu->pc += 1;
        cpu->cycles += 4;
        break;
    }
    case 0xa8:
    {
        #ifdef DEBUG
			printf("XRA    B");
		#endif // Register B OR register A (exclusive)
        cpu->a = cpu->a ^ cpu->b;
        cpu->flags.c = 0;
        cpu->flags.ac = 0;
        setZSPflags(cpu, cpu->a);
        cpu->pc += 1;
        cpu->cycles += 4;
        break;
    }
    case 0xa9:
    {
        #ifdef DEBUG
			printf("XRA    C");
		#endif // Register B OR register A (exclusive)
        cpu->a = cpu->a ^ cpu->c;
        cpu->flags.c = 0;
        cpu->flags.ac = 0;
        setZSPflags(cpu, cpu->a);
        cpu->pc += 1;
        cpu->cycles += 4;
        break;
    }
    case 0xaa:
    {
        #ifdef DEBUG
			printf("XRA    D");
		#endif // Register B OR register A (exclusive)
        cpu->a = cpu->a ^ cpu->d;
        cpu->flags.c = 0;
        cpu->flags.ac = 0;
        setZSPflags(cpu, cpu->a);
        cpu->pc += 1;
        cpu->cycles += 4;
        break;
    }
    case 0xab:
    {
        #ifdef DEBUG
			printf("XRA    E");
		#endif // Register B OR register A (exclusive)
        cpu->a = cpu->a ^ cpu->e;
        cpu->flags.c = 0;
        cpu->flags.ac = 0;
        setZSPflags(cpu, cpu->a);
        cpu->pc += 1;
        cpu->cycles += 4;
        break;
    }
    case 0xac:
    {
        #ifdef DEBUG
			printf("XRA    H");
		#endif // Register B OR register A (exclusive)
        cpu->a = cpu->a ^ cpu->h;
        cpu->flags.c = 0;
        cpu->flags.ac = 0;
        setZSPflags(cpu, cpu->a);
        cpu->pc += 1;
        cpu->cycles += 4;
        break;
    }
    case 0xad:
    {
        #ifdef DEBUG
			printf("XRA    L");
		#endif // Register B OR register A (exclusive)
        cpu->a = cpu->a ^ cpu->l;
        cpu->flags.c = 0;
        cpu->flags.ac = 0;
        setZSPflags(cpu, cpu->a);
        cpu->pc += 1;
        cpu->cycles += 4;
        break;
    }
    case 0xae:
    {
        #ifdef DEBUG
			printf("XRA    M");
		#endif // Register B OR register A (exclusive)
        uint16_t addr = (cpu->h << 8) | cpu->l;
        uint8_t value = cpu->memory[addr];
        cpu->a = cpu->a ^ value;
        cpu->flags.c = 0;
        cpu->flags.ac = 0;
        setZSPflags(cpu, cpu->a);
        cpu->pc += 1;
        cpu->cycles += 7;
        break;
    }
    case 0xaf:
    {
        #ifdef DEBUG
			printf("XRA    A");
		#endif // Register B OR register A (exclusive)
        cpu->a = cpu->a ^ cpu->a;
        cpu->flags.c = 0;
        cpu->flags.ac = 0;
        setZSPflags(cpu, cpu->a);
        cpu->pc += 1;
        cpu->cycles += 4;
        break;
    }
    case 0Xb0:
    {
        #ifdef DEBUG
			printf("ORA    B");
		#endif // Register B OR register A
        cpu->a = cpu->a | cpu->b;
        cpu->flags.c = 0;
        cpu->flags.ac = 0;
        setZSPflags(cpu, cpu->a);
        cpu->pc += 1;
        cpu->cycles += 4;
        break;
    }
    case 0Xb1:
    {
        #ifdef DEBUG
			printf("ORA    C");
		#endif // Register C OR register A
        cpu->a = cpu->a | cpu->c;
        cpu->flags.c = 0;
        cpu->flags.ac = 0;
        setZSPflags(cpu, cpu->a);
        cpu->pc += 1;
        cpu->cycles += 4;
        break;
    }
    case 0Xb2:
    {
        #ifdef DEBUG
			printf("ORA    D");
		#endif // Register D OR register A
        cpu->a = cpu->a | cpu->d;
        cpu->flags.c = 0;
        cpu->flags.ac = 0;
        setZSPflags(cpu, cpu->a);
        cpu->pc += 1;
        cpu->cycles += 4;
        break;
    }
    case 0Xb3:
    {
        #ifdef DEBUG
			printf("ORA    E");
		#endif // Register E OR register A
        cpu->a = cpu->a | cpu->e;
        cpu->flags.c = 0;
        cpu->flags.ac = 0;
        setZSPflags(cpu, cpu->a);
        cpu->pc += 1;
        cpu->cycles += 4;
        break;
    }
    case 0Xb4:
    {
        #ifdef DEBUG
			printf("ORA    H");
		#endif // Register H OR register A
        cpu->a = cpu->a | cpu->h;
        cpu->flags.c = 0;
        cpu->flags.ac = 0;
        setZSPflags(cpu, cpu->a);
        cpu->pc += 1;
        cpu->cycles += 4;
        break;
    }
    case 0Xb5:
    {
        #ifdef DEBUG
			printf("ORA    L");
		#endif // Register L OR register A
        cpu->a = cpu->a | cpu->l;
        cpu->flags.c = 0;
        cpu->flags.ac = 0;
        setZSPflags(cpu, cpu->a);
        cpu->pc += 1;
        cpu->cycles += 4;
        break;
    }
    case 0Xb6:
    {
        #ifdef DEBUG
			printf("ORA    M");
		#endif // Register M OR register A
        uint16_t addr = (cpu->h << 8) | cpu->l;
        uint8_t value = cpu->memory[addr];
        cpu->a = cpu->a | value;
        cpu->flags.c = 0;
        cpu->flags.ac = 0;
        setZSPflags(cpu, cpu->a);
        cpu->pc += 1;
        cpu->cycles += 7;
        break;
    }
    case 0Xb7:
    {
        #ifdef DEBUG
			printf("ORA    A");
		#endif // Register A OR register A
        cpu->a = cpu->a | cpu->a;
        cpu->flags.c = 0;
        cpu->flags.ac = 0;
        setZSPflags(cpu, cpu->a);
        cpu->pc += 1;
        cpu->cycles += 4;
        break;
    }
    case 0Xb8:
    {
        #ifdef DEBUG
			printf("CMP    B");
		#endif // Compare register B with register A
        uint8_t difference = cpu->a - cpu->b;
        cpu->flags.c = (cpu->a < cpu->b);
        cpu->flags.ac = (cpu->a & 0x0F) < (cpu->b & 0x0F);
        setZSPflags(cpu, difference);
        cpu->pc += 1;
        cpu->cycles += 4;
        break;
    }
    case 0Xb9:
    {
        #ifdef DEBUG
			printf("CMP    C");
		#endif // Compare register C with register A
        uint8_t difference = cpu->a - cpu->c;
        cpu->flags.c = (cpu->a < cpu->c);
        cpu->flags.ac = (cpu->a & 0x0F) < (cpu->c & 0x0F);
        setZSPflags(cpu, difference);
        cpu->pc += 1;
        cpu->cycles += 4;
        break;
    }
    case 0Xba:
    {
        #ifdef DEBUG
			printf("CMP    D");
		#endif // Compare register D with register A
        uint8_t difference = cpu->a - cpu->d;
        cpu->flags.c = (cpu->a < cpu->d);
        cpu->flags.ac = (cpu->a & 0x0F) < (cpu->d & 0x0F);
        setZSPflags(cpu, difference);
        cpu->pc += 1;
        cpu->cycles += 4;
        break;
    }
    case 0Xbb:
    {
        #ifdef DEBUG
			printf("CMP    E");
		#endif // Compare register E with register A
        uint8_t difference = cpu->a - cpu->e;
        cpu->flags.c = (cpu->a < cpu->e);
        cpu->flags.ac = (cpu->a & 0x0F) < (cpu->e & 0x0F);
        setZSPflags(cpu, difference);
        cpu->pc += 1;
        cpu->cycles += 4;
        break;
    }
    case 0Xbc:
    {
        #ifdef DEBUG
			printf("CMP    H");
		#endif // Compare register H with register A
        uint8_t difference = cpu->a - cpu->h;
        cpu->flags.c = (cpu->a < cpu->h);
        cpu->flags.ac = (cpu->a & 0x0F) < (cpu->h & 0x0F);
        setZSPflags(cpu, difference);
        cpu->pc += 1;
        cpu->cycles += 4;
        break;
    }
    case 0Xbd:
    {
        #ifdef DEBUG
			printf("CMP    L");
		#endif // Compare register L with register A
        uint8_t difference = cpu->a - cpu->l;
        cpu->flags.c = (cpu->a < cpu->l);
        cpu->flags.ac = (cpu->a & 0x0F) < (cpu->l & 0x0F);
        setZSPflags(cpu, difference);
        cpu->pc += 1;
        cpu->cycles += 4;
        break;
    }
    case 0Xbe:
    {
        #ifdef DEBUG
			printf("CMP    M");
		#endif // Compare register M with register A
        uint16_t addr = (cpu->h << 8) | cpu->l;
        uint8_t value = cpu->memory[addr];
        uint8_t difference = cpu->a - value;
        cpu->flags.c = (cpu->a < value);
        cpu->flags.ac = (cpu->a & 0x0F) < (value & 0x0F);
        setZSPflags(cpu, difference);
        cpu->pc += 1;
        cpu->cycles += 7;
        break;
    }
    case 0Xbf:
    {
        #ifdef DEBUG
			printf("CMP    A");
		#endif // Compare register A with register A
        uint8_t difference = cpu->a - cpu->a;
        cpu->flags.c = (cpu->a < cpu->a);
        cpu->flags.ac = (cpu->a & 0x0F) < (cpu->a & 0x0F);
        setZSPflags(cpu, difference);
        cpu->pc += 1;
        cpu->cycles += 4;
        break;
    }
    case 0Xc0:
    {
        #ifdef DEBUG
			printf("RNZ");
		#endif // Return on no zero: if zero flag is not set, jump to address stored on stack
        if (cpu->flags.z == 0) {
            cpu->pc = (cpu->memory[cpu->sp + 1] << 8) | cpu->memory[cpu->sp];
            cpu->sp += 2;
            cpu->cycles += 11;
        }
        else {
            cpu->pc += 1;
            cpu->cycles += 5;
        }
        break;
    }
    case 0Xc1:
    {
        #ifdef DEBUG
			printf("POP    B");
		#endif // Pop BC from stack
        cpu->c = cpu->memory[cpu->sp];
        cpu->b = cpu->memory[cpu->sp + 1];
        cpu->sp += 2;
        cpu->pc += 1;
        cpu->cycles += 10;
        break;
    }
    case 0Xc2:
    {
        uint16_t addr = (code[2] << 8) | code[1];
        #ifdef DEBUG
			printf("JNZ    addr: %04x", addr);
		#endif // Jump if zero flag is not set
        if (cpu->flags.z == 0) {
            cpu->pc = addr;
            cpu->cycles += 10;
        }
        else {
            cpu->pc += 3;
            cpu->cycles += 7;
        }
        break;
    }
    case 0Xc3:
    {
        uint16_t addr = (code[2] << 8) | code[1];
        #ifdef DEBUG
			printf("JMP    addr: %04x, H: %02x, L: %02x", addr, code[2], code[1]);
		#endif // Jump unconditional
        cpu->pc = addr;
        cpu->cycles += 16;
        break;
    }
    case 0Xc4:
    {
        uint16_t addr = (code[2] << 8) | code[1];
        #ifdef DEBUG
			printf("CNZ    addr: %04x", addr);
		#endif // Call on non-zero: jump to a new location in memory if the zero flag is not set
        cpu->pc += 3;
        if (cpu->flags.z == 0) {
            cpu->memory[cpu->sp - 1] = (cpu->pc >> 8) & 0xFF;
            cpu->memory[cpu->sp - 2] = cpu->pc & 0xFF;
            cpu->sp -= 2;
            cpu->pc = addr;
            cpu->cycles += 17;
        }
        else {
            cpu->cycles += 11;
        }
        break;
    }
    case 0Xc5:
    {
        #ifdef DEBUG
			printf("PUSH   B");
		#endif // Push contents of register BC to stack
        cpu->memory[cpu->sp - 1] = cpu->b;
        cpu->memory[cpu->sp - 2] = cpu->c;
        cpu->sp -= 2;
        cpu->pc += 1;
        cpu->cycles += 11;
        break;
    }
    case 0Xc6:
    {
        #ifdef DEBUG
			printf("ADI    %02x", code[1]);
		#endif // Add immediate to register A
        uint16_t answer = code[1] + cpu->a;
        cpu->flags.c = (answer > 0xFF);
        cpu->flags.ac = ((code[1] & 0x0F) + (cpu->a & 0x0F)) > 0x0F;
        cpu->a = answer;
        setZSPflags(cpu, cpu->a);
        cpu->pc += 2;
        cpu->cycles += 7;
        break;
    }
    case 0Xc7:
    {
        #ifdef DEBUG
			printf("RST    0");
		#endif // Restart 0
        cpu->memory[cpu->sp - 1] = (cpu->pc >> 8) & 0xFF;
        cpu->memory[cpu->sp - 2] = cpu->pc & 0xFF;
        cpu->sp -= 2;
        cpu->pc = 0x00;
        cpu->cycles += 11;
        break;
    }
    case 0Xc8:
    {
        #ifdef DEBUG
			printf("RZ");
		#endif // Return on zero: if zero flag is set, jump to address stored on stack
        if (cpu->flags.z) {
            cpu->pc = (cpu->memory[cpu->sp + 1] << 8) | cpu->memory[cpu->sp];
            cpu->sp += 2;
            cpu->cycles += 11;
        }
        else {
            cpu->pc += 1;
            cpu->cycles += 5;
        }
        break;
    }
    case 0Xc9:
    {
        #ifdef DEBUG
			printf("RET");
		#endif // Unconditional return: jump to address on stack
        cpu->pc = (cpu->memory[cpu->sp + 1] << 8) | cpu->memory[cpu->sp];
        cpu->sp += 2;
        cpu->cycles += 10;
        break;
    }
    case 0Xca:
    {
        uint16_t addr = (code[2] << 8) | code[1];
        #ifdef DEBUG
			printf("JZ     addr: %04x", addr);
		#endif // Jump if zero flag is set
        if (cpu->flags.z) { cpu->pc = addr; }
        else {
            cpu->pc += 3;
            cpu->cycles += 10;
        }
        break;
    }
    case 0Xcb:
    {
        uint16_t addr = (code[2] << 8) | code[1];
        #ifdef DEBUG
			printf("JMP    addr: %04x, H: %02x, L: %02x", addr, code[2], code[1]);
		#endif // Jump unconditional
        cpu->pc = addr;
        cpu->cycles += 10;
        break;
    }
    case 0Xcc:
    {
        uint16_t addr = (code[2] << 8) | code[1];
        #ifdef DEBUG
			printf("CZ     addr: %04x", addr);
		#endif // Call on zero: jump to new location in memory if zero flag is set
        cpu->pc += 3;
        if (cpu->flags.z) {
            cpu->memory[cpu->sp - 1] = (cpu->pc >> 8) & 0xFF;
            cpu->memory[cpu->sp - 2] = cpu->pc & 0xFF;
            cpu->sp -= 2;
            cpu->pc = addr;
            cpu->cycles += 17;
        }
        else {
            cpu->cycles += 11;
        }
        break;
    }
    case 0Xcd:
    {
        uint16_t addr = (code[2] << 8) | code[1];
        #ifdef DEBUG
			printf("CALL   addr: %04x", addr);
		#endif // Unconditional call: jump to new location in memory
        cpu->pc += 3;
        cpu->memory[cpu->sp - 1] = (cpu->pc >> 8) & 0xFF;
        cpu->memory[cpu->sp - 2] = cpu->pc & 0xFF;
        cpu->sp -= 2;
        cpu->pc = addr;
        cpu->cycles += 17;
        break;
    }
    case 0Xce:
    {
        #ifdef DEBUG
			printf("ACI    %02x", code[1]);
		#endif // Add immediate to A with carry
        uint16_t answer = code[1] + cpu->a + cpu->flags.c;
        cpu->flags.c = (answer > 0xFF);
        cpu->flags.ac = ((code[1] & 0x0F) + (cpu->a & 0x0F) + cpu->flags.c) > 0x0F;
        cpu->a = answer;
        setZSPflags(cpu, cpu->a);
        cpu->pc += 2;
        cpu->cycles += 7;
        break;
    }
    case 0Xcf:
    {
        #ifdef DEBUG
			printf("RST    1");
		#endif // Restart 1
        cpu->memory[cpu->sp - 1] = (cpu->pc >> 8) & 0xFF;
        cpu->memory[cpu->sp - 2] = cpu->pc & 0xFF;
        cpu->sp -= 2;
        cpu->pc = 0x08;
        cpu->cycles += 11;
        break;
    }
    case 0Xd0:
    {
        #ifdef DEBUG
			printf("RNC");
		#endif // Return on no carry: if carry flag is not set, jump to address stored on stack
        if (cpu->flags.c == 0) {
            cpu->pc = (cpu->memory[cpu->sp + 1] << 8) | cpu->memory[cpu->sp];
            cpu->sp += 2;
            cpu->cycles += 11;
        }
        else {
            cpu->pc += 1;
            cpu->cycles += 5;
        }
        break;
    }
    case 0Xd1:
    {
        #ifdef DEBUG
			printf("POP    D");
		#endif // Pop DE from stack
        cpu->e = cpu->memory[cpu->sp];
        cpu->d = cpu->memory[cpu->sp + 1];
        cpu->sp += 2;
        cpu->pc += 1;
        cpu->cycles += 10;
        break;
    }
    case 0Xd2:
    {
        uint16_t addr = (code[2] << 8) | code[1];
        #ifdef DEBUG
			printf("JNC    addr: %04x", addr);
		#endif // Jump if carry flag is not set
        if (cpu->flags.c == 0) { cpu->pc = addr; }
        else {
            cpu->pc += 3;
            cpu->cycles += 10;
        }
        break;
    }
    case 0Xd3:
    {
        /*
        (data) <- (A)
        The content of register A is placed on the eight bit
        bi-directional data bus for transmission to the specified port.
        */
        uint8_t port = code[1];
        #ifdef DEBUG
			printf("OUT    port: %02x", port);
		#endif // Output content from A to port address
        output_port(cpu, port, cpu->a);
        cpu->pc += 2;
        cpu->cycles += 10;
        break;
    }
    case 0Xd4:
    {
        uint16_t addr = (code[2] << 8) | code[1];
        #ifdef DEBUG
			printf("CNC    addr: %04x", addr);
		#endif // Call on non-carry: jump to a new location in memory if the carry flag is not set
        cpu->pc += 3;
        if (cpu->flags.c == 0) {
            cpu->memory[cpu->sp - 1] = (cpu->pc >> 8) & 0xFF;
            cpu->memory[cpu->sp - 2] = cpu->pc & 0xFF;
            cpu->sp -= 2;
            cpu->pc = addr;
            cpu->cycles += 17;
        }
        else {
            cpu->cycles += 11;
        }
        break;
    }
    case 0Xd5:
    {
        #ifdef DEBUG
			printf("PUSH   D");
		#endif // Push contents of register DE to stack
        cpu->memory[cpu->sp - 1] = cpu->d;
        cpu->memory[cpu->sp - 2] = cpu->e;
        cpu->sp -= 2;
        cpu->pc += 1;
        cpu->cycles += 11;
        break;
    }
    case 0Xd6:
    {
        #ifdef DEBUG
			printf("SUI    %02x", code[1]);
		#endif // Subtract immediate from register A
        uint16_t answer = (uint16_t)cpu->a - (uint16_t)code[1];
        cpu->flags.ac = ((cpu->a & 0x0F) < (code[1] & 0x0F));
        cpu->flags.c = (cpu->a < code[1]);
        cpu->a = answer & 0xFF;
        setZSPflags(cpu, cpu->a);
        cpu->pc += 2;
        cpu->cycles += 7;
        break;
    }
    case 0Xd7:
    {
        #ifdef DEBUG
			printf("RST    2");
		#endif // Restart 2
        cpu->memory[cpu->sp - 1] = (cpu->pc >> 8) & 0xFF;
        cpu->memory[cpu->sp - 2] = cpu->pc & 0xFF;
        cpu->sp -= 2;
        cpu->pc = 0x10;
        cpu->cycles += 11;
        break;
    }
    case 0Xd8:
    {
        #ifdef DEBUG
			printf("RC");
		#endif // Return on carry: if carry flag is set, jump to address stored on stack
        if (cpu->flags.c) {
            cpu->pc = (cpu->memory[cpu->sp + 1] << 8) | cpu->memory[cpu->sp];
            cpu->sp += 2;
            cpu->cycles += 11;
        }
        else {
            cpu->pc += 1;
            cpu->cycles += 5;
        }
        break;
    }
    case 0Xd9:
    {
        #ifdef DEBUG
			printf("RET");
		#endif // Unconditional return: jump to address on stack
        cpu->pc = (cpu->memory[cpu->sp + 1] << 8) | cpu->memory[cpu->sp];
        cpu->sp += 2;
        cpu->cycles += 10;
        break;
    }
    case 0Xda:
    {
        uint16_t addr = (code[2] << 8) | code[1];
        #ifdef DEBUG
			printf("JNC    addr: %04x", addr);
		#endif // Jump if carry flag is set
        if (cpu->flags.c) { cpu->pc = addr; }
        else {
            cpu->pc += 3;
            cpu->cycles += 10;
        }
        break;
    }
    case 0Xdb:
    {
        /*
        (A) <- (data)
        The data placed on the eight bit bi-directional data
        bus by the specified port is moved to register A.
        */
        uint8_t port = code[1];
        #ifdef DEBUG
			printf("IN     port: %02x", port);
		#endif // Input content from port address
        // map keyboard input to this function
        cpu->a = input_port(cpu, port);
        cpu->pc += 2;
        cpu->cycles += 10;
        break;
    }
    case 0Xdc:
    {
        uint16_t addr = (code[2] << 8) | code[1];
        #ifdef DEBUG
			printf("CC     addr: %04x", addr);
		#endif // Call on carry: jump to new location in memory if carry flag is set
        cpu->pc += 3;
        if (cpu->flags.c) {
            cpu->memory[cpu->sp - 1] = (cpu->pc >> 8) & 0xFF;
            cpu->memory[cpu->sp - 2] = cpu->pc & 0xFF;
            cpu->sp -= 2;
            cpu->pc = addr;
            cpu->cycles += 17;
        }
        else {
            cpu->cycles += 11;
        }
        break;
    }
    case 0Xdd:
    {
        uint16_t addr = (code[2] << 8) | code[1];
        #ifdef DEBUG
			printf("CALL   addr: %04x", addr);
		#endif // Unconditional call: jump to new location in memory
        cpu->pc += 3;
        cpu->memory[cpu->sp - 1] = (cpu->pc >> 8) & 0xFF;
        cpu->memory[cpu->sp - 2] = cpu->pc & 0xFF;
        cpu->sp -= 2;
        cpu->pc = addr;
        cpu->cycles += 17;
        break;
    }
    case 0Xde:
    {
        #ifdef DEBUG
			printf("SBI    %02x", code[1]);
		#endif // Subtract immediate from A with borrow
        uint16_t answer = (uint16_t)cpu->a - (uint16_t)code[1] - cpu->flags.c;
        cpu->flags.ac = ((cpu->a & 0x0F) < ((code[1] + cpu->flags.c) & 0x0F));
        cpu->flags.c = (cpu->a < (code[1] + cpu->flags.c));
        cpu->a = answer & 0xFF;
        setZSPflags(cpu, cpu->a);
        cpu->pc += 2;
        cpu->cycles += 7;
        break;
    }
    case 0Xdf:
    {
        #ifdef DEBUG
			printf("RST    3");
		#endif // Restart 3
        cpu->memory[cpu->sp - 1] = (cpu->pc >> 8) & 0xFF;
        cpu->memory[cpu->sp - 2] = cpu->pc & 0xFF;
        cpu->sp -= 2;
        cpu->pc = 0x18;
        cpu->cycles += 11;
        break;
    }
    case 0Xe0:
    {
        #ifdef DEBUG
			printf("RPO");
		#endif // Return on parity odd: if parity flag is odd, jump to address stored on stack
        if (cpu->flags.p == 0) {
            cpu->pc = (cpu->memory[cpu->sp + 1] << 8) | cpu->memory[cpu->sp];
            cpu->sp += 2;
            cpu->cycles += 11;
        }
        else {
            cpu->pc += 1;
            cpu->cycles += 5;
        }
        break;
    }
    case 0Xe1:
    {
        #ifdef DEBUG
			printf("POP    H");
		#endif // Pop HL from stack
        cpu->l = cpu->memory[cpu->sp];
        cpu->h = cpu->memory[cpu->sp + 1];
        cpu->sp += 2;
        cpu->pc += 1;
        cpu->cycles += 10;
        break;
    }
    case 0Xe2:
    {
        uint16_t addr = (code[2] << 8) | code[1];
        #ifdef DEBUG
			printf("JPO    addr: %04x", addr);
		#endif // Jump if parity flag is odd
        if (cpu->flags.p == 0) { cpu->pc = addr; }
        else {
            cpu->pc += 3;
            cpu->cycles += 10;
        }
        break;
    }
    case 0Xe3:
    {
        #ifdef DEBUG
			printf("XTHL");
		#endif // Exchange contents from HL and top of stack
        uint8_t h = cpu->h;
        uint8_t l = cpu->l;
        cpu->l = cpu->memory[cpu->sp];
        cpu->h = cpu->memory[cpu->sp + 1];
        cpu->memory[cpu->sp] = l;
        cpu->memory[cpu->sp + 1] = h;
        cpu->pc += 1;
        cpu->cycles += 18;
        break;
    }
    case 0Xe4:
    {
        uint16_t addr = (code[2] << 8) | code[1];
        #ifdef DEBUG
			printf("CPO    addr: %04x", addr);
		#endif // Call on parity-odd: jump to a new location in memory if the parity flag is odd
        cpu->pc += 3;
        if (cpu->flags.p == 0) {
            cpu->memory[cpu->sp - 1] = (cpu->pc >> 8) & 0xFF;
            cpu->memory[cpu->sp - 2] = cpu->pc & 0xFF;
            cpu->sp -= 2;
            cpu->pc = addr;
            cpu->cycles += 17;
        }
        else {
            cpu->cycles += 11;
        }
        break;
    }
    case 0Xe5:
    {
        #ifdef DEBUG
			printf("PUSH   H");
		#endif // Push contents of register HL to stack
        cpu->memory[cpu->sp - 1] = cpu->h;
        cpu->memory[cpu->sp - 2] = cpu->l;
        cpu->sp -= 2;
        cpu->pc += 1;
        cpu->cycles += 11;
        break;
    }
    case 0Xe6:
    {
        #ifdef DEBUG
			printf("ANI    %02x", code[1]);
		#endif // Add immediate to register A
        cpu->a = cpu->a & code[1];
        cpu->flags.c = 0;
        cpu->flags.ac = ((cpu->a | code[1]) & 0x08) != 0;
        setZSPflags(cpu, cpu->a);
        cpu->pc += 2;
        cpu->cycles += 11;
        break;
    }
    case 0Xe7:
    {
        #ifdef DEBUG
			printf("RST    4");
		#endif // Restart 4
        cpu->memory[cpu->sp - 1] = (cpu->pc >> 8) & 0xFF;
        cpu->memory[cpu->sp - 2] = cpu->pc & 0xFF;
        cpu->sp -= 2;
        cpu->pc = 0x20;
        cpu->cycles += 11;
        break;
    }
    case 0Xe8:
    {
        #ifdef DEBUG
			printf("RPE");
		#endif // Return on parity even: if parity flag is even, jump to address stored on stack
        if (cpu->flags.p) {
            cpu->pc = (cpu->memory[cpu->sp + 1] << 8) | cpu->memory[cpu->sp];
            cpu->sp += 2;
            cpu->cycles += 11;
        }
        else {
            cpu->pc += 1;
            cpu->cycles += 5;
        }
        break;
    }
    case 0Xe9:
    {
        #ifdef DEBUG
			printf("PCHL");
		#endif // Copies the contents from HL to the program counter
        cpu->pc = (cpu->h << 8) | cpu->l;
        cpu->cycles += 5;
        break;
    }
    case 0Xea:
    {
        uint16_t addr = (code[2] << 8) | code[1];
        #ifdef DEBUG
			printf("JPE    addr: %04x", addr);
		#endif // Jump if parity flag is even
        if (cpu->flags.p) { cpu->pc = addr; }
        else {
            cpu->pc += 3;
            cpu->cycles += 10;
        }
        break;
    }
    case 0Xeb:
    {
        #ifdef DEBUG
			printf("XCHG");
		#endif // Exchange contents of HL and DE
        uint8_t d = cpu->d;
        uint8_t e = cpu->e;
        cpu->d = cpu->h;
        cpu->e = cpu->l;
        cpu->h = d;
        cpu->l = e;
        cpu->pc += 1;
        cpu->cycles += 5;
        break;
    }
    case 0Xec:
    {
        uint16_t addr = (code[2] << 8) | code[1];
        #ifdef DEBUG
			printf("CPO    addr: %04x", addr);
		#endif // Call on parity-even: jump to a new location in memory if the parity flag is even
        cpu->pc += 3;
        if (cpu->flags.p) {
            cpu->memory[cpu->sp - 1] = (cpu->pc >> 8) & 0xFF;
            cpu->memory[cpu->sp - 2] = cpu->pc & 0xFF;
            cpu->sp -= 2;
            cpu->pc = addr;
            cpu->cycles += 17; //should this be CPE?
        }
        else {
            cpu->cycles += 11;
        }
        break;
    }
    case 0Xed:
    {
        uint16_t addr = (code[2] << 8) | code[1];
        #ifdef DEBUG
			printf("CALL   addr: %04x", addr);
		#endif // Unconditional call: jump to new location in memory
        cpu->pc += 3;
        cpu->memory[cpu->sp - 1] = (cpu->pc >> 8) & 0xFF;
        cpu->memory[cpu->sp - 2] = cpu->pc & 0xFF;
        cpu->sp -= 2;
        cpu->pc = addr;
        cpu->cycles += 17;
        break;
    }
    case 0Xee:
    {
        #ifdef DEBUG
			printf("XRI    %02x", code[1]);
		#endif // immediate OR A (exclusive)
        cpu->a = cpu->a ^ code[1];
        cpu->flags.c = 0;
        cpu->flags.ac = 0;
        setZSPflags(cpu, cpu->a);
        cpu->pc += 2;
        cpu->cycles += 7;
        break;
    }
    case 0Xef:
    {
        #ifdef DEBUG
			printf("RST    5");
		#endif // Restart 3
        cpu->memory[cpu->sp - 1] = (cpu->pc >> 8) & 0xFF;
        cpu->memory[cpu->sp - 2] = cpu->pc & 0xFF;
        cpu->sp -= 2;
        cpu->pc = 0x28;
        cpu->cycles += 11;
        break;
    }
    case 0Xf0:
    {
        #ifdef DEBUG
			printf("RP");
		#endif // Return on positive: if sign flag is not set, jump to address stored on stack
        if (cpu->flags.s == 0) {
            cpu->pc = (cpu->memory[cpu->sp + 1] << 8) | cpu->memory[cpu->sp];
            cpu->sp += 2;
            cpu->cycles += 11;
        }
        else {
            cpu->pc += 1;
            cpu->cycles += 5;
        }
        break;
    }
    case 0Xf1:
    {
        #ifdef DEBUG
			printf("POP    PSW");
		#endif // Pop A and flags from stack
        uint8_t flags = cpu->memory[cpu->sp];
        cpu->a = cpu->memory[cpu->sp + 1];

        cpu->flags.c = flags & 1;
        cpu->flags.p = (flags >> 2) & 1;
        cpu->flags.ac = (flags >> 4) & 1;
        cpu->flags.z = (flags >> 6) & 1;
        cpu->flags.s = (flags >> 7) & 1;

        cpu->sp += 2;
        cpu->pc += 1;
        cpu->cycles += 10;
        break;
    }
    case 0Xf2:
    {
        uint16_t addr = (code[2] << 8) | code[1];
        #ifdef DEBUG
			printf("JP     addr: %04x", addr);
		#endif // Jump if sign flag is not set
        if (cpu->flags.s == 0) { cpu->pc = addr; }
        else {
            cpu->pc += 3;
            cpu->cycles += 10;
        }
        break;
    }
    case 0Xf3:
    {
        #ifdef DEBUG
			printf("DI");
		#endif // Disable interupt
        cpu->interrupt_enabled = false;
        cpu->pc += 1;
        cpu->cycles += 4;
        break;
    }
    case 0Xf4:
    {
        uint16_t addr = (code[2] << 8) | code[1];
        #ifdef DEBUG
			printf("CP     addr: %04x", addr);
		#endif // Call on positive: jump to a new location in memory if the sign flag is not set
        cpu->pc += 3;
        if (cpu->flags.s == 0) {
            cpu->memory[cpu->sp - 1] = (cpu->pc >> 8) & 0xFF;
            cpu->memory[cpu->sp - 2] = cpu->pc & 0xFF;
            cpu->sp -= 2;
            cpu->pc = addr;
            cpu->cycles += 17;
        }
        else {
            cpu->cycles += 11;
        }
        break;
    }
    case 0Xf5:
    {
        #ifdef DEBUG
			printf("PUSH   PSW");
		#endif // Push contents of register A and flags to stack
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
        cpu->cycles += 11;
        break;
    }
    case 0Xf6:
    {
        #ifdef DEBUG
			printf("ORI    %02x", code[1]);
		#endif // Immediate OR register A
        cpu->a = cpu->a | code[1];
        cpu->flags.c = 0;
        cpu->flags.ac = 0;
        setZSPflags(cpu, cpu->a);
        cpu->pc += 2;
        cpu->cycles += 7;
        break;
    }
    case 0Xf7:
    {
        #ifdef DEBUG
			printf("RST    6");
		#endif // Restart 6
        cpu->memory[cpu->sp - 1] = (cpu->pc >> 8) & 0xFF;
        cpu->memory[cpu->sp - 2] = cpu->pc & 0xFF;
        cpu->sp -= 2;
        cpu->pc = 0x30;
        cpu->cycles += 11;
        break;
    }
    case 0Xf8:
    {
        #ifdef DEBUG
			printf("RM");
		#endif // Return on minus: if sign flag is set, jump to address stored on stack
        if (cpu->flags.s) {
            cpu->pc = (cpu->memory[cpu->sp + 1] << 8) | cpu->memory[cpu->sp];
            cpu->sp += 2;
            cpu->cycles += 11;
        }
        else {
            cpu->pc += 1;
            cpu->cycles += 5;
        }
        break;
    }
    case 0Xf9:
    {
        #ifdef DEBUG
			printf("SPHL");
		#endif // Copies the contents from HL to the stack pointer
        cpu->sp = (cpu->h << 8) | cpu->l;
        cpu->pc += 1;
        cpu->cycles += 5;
        break;
    }
    case 0Xfa:
    {
        uint16_t addr = (code[2] << 8) | code[1];
        #ifdef DEBUG
			printf("JM     addr: %04x", addr);
		#endif // Jump on minus: if sign flag is set
        if (cpu->flags.s) { cpu->pc = addr; }
        else {
            cpu->pc += 3;
            cpu->cycles += 11;
        }
        break;
    }
    case 0Xfb:
    {
        #ifdef DEBUG
			printf("EI");
		#endif // Enable interupts
        cpu->interrupt_enabled = true;
        cpu->pc += 1;
        cpu->cycles += 4;
        break;
    }
    case 0Xfc:
    {
        uint16_t addr = (code[2] << 8) | code[1];
        #ifdef DEBUG
			printf("CM     addr: %04x", addr);
		#endif // Call on minus: jump to new location in memory if sign flag is set
        cpu->pc += 3;
        if (cpu->flags.s == 0) {
            cpu->memory[cpu->sp - 1] = (cpu->pc >> 8) & 0xFF;
            cpu->memory[cpu->sp - 2] = cpu->pc & 0xFF;
            cpu->sp -= 2;
            cpu->pc = addr;
            cpu->cycles += 17;
        }
        else {
            cpu->cycles += 11;
        }
        break;
    }
    case 0Xfd:
    {
        uint16_t addr = (code[2] << 8) | code[1];
        #ifdef DEBUG
			printf("CALL   addr: %04x", addr);
		#endif // Unconditional call: jump to new location in memory
        cpu->pc += 3;
        cpu->memory[cpu->sp - 1] = (cpu->pc >> 8) & 0xFF;
        cpu->memory[cpu->sp - 2] = cpu->pc & 0xFF;
        cpu->sp -= 2;
        cpu->pc = addr;
        cpu->cycles += 17;
        break;
    }
    case 0Xfe:
    {
        #ifdef DEBUG
			printf("CPI    %02x", code[1]);
		#endif // Compare immediate with contents of A
        uint8_t difference = cpu->a - code[1];
        cpu->flags.c = (cpu->a < code[1]);
        cpu->flags.ac = ((cpu->a & 0x0F) < (code[1] & 0x0F));
        setZSPflags(cpu, difference);
        cpu->pc += 2;
        cpu->cycles += 7;
        break;
    }
    case 0Xff:
    {
        #ifdef DEBUG
			printf("RST    7");
		#endif // Restart 7
        cpu->memory[cpu->sp - 1] = (cpu->pc >> 8) & 0xFF;
        cpu->memory[cpu->sp - 2] = cpu->pc & 0xFF;
        cpu->sp -= 2;
        cpu->pc = 0x38;
        cpu->cycles += 11;
        break;
    }
    }

    #ifdef DEBUG
		printf("\n");
	#endif

}