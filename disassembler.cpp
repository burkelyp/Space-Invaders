#include <stdio.h>
#include <stdlib.h>


int Disassemble8080Op(unsigned char *codebuffer, int pc) {
    /*
    *codebuffer is a valid pointer to 8080 assembly code
    pc is the current offset into the code

    returns the number of bytes of the op
   */
    unsigned char *code = &codebuffer[pc];
    int opbytes = 1;
    printf ("%04x: %02x: ", pc, *code);
    switch (*code) {
        case 0x00:
            printf("NOP");
            break;
        case 0x01:
            printf("LXI    B, %02x%02x", code[2], code[1]);
            opbytes = 3;
            break;
        case 0x02:
            printf("STAX   B");
            break;
        case 0x03:
            printf("INX    B");
            break;
        case 0x04:
            printf("INR    B");
            break;
        case 0x05:
            printf("DCR    B");
            break;
        case 0x06:
            printf("MVI    B, %02x", code[1]);
            opbytes = 2;
            break;
        case 0x07:
            printf("RLC");
            break;
        case 0x08:
            printf("NOP");
            break;
        case 0x09:
            printf("DAD    B"); // Add BC to HL
            break;
        case 0x0a:
            printf("LDAX   B"); // Load A indirect
            break;
        case 0x0b:
            printf("DCX    B"); // Decrement BC
            break;
        case 0x0c:
            printf("INR    C"); // Increment C
            break;
        case 0x0d:
            printf("DCR    C"); // Decrement C
            break;
        case 0x0e:
            printf("MVI    C, %02x", code[1]); // Move immediate register to C
            opbytes = 2;
            break;
        case 0x0f:
            printf("RRC"); // Rotate A right
            break;
        case 0x10:
            printf("NOP");
            break;
        case 0x11:
            printf("LXI    D, %02x%02x", code[2], code[1]); // Load immediate register pair
            opbytes = 3;
            break;
        case 0x12:
            printf("STAX   D"); // Store A indirect
            break;
        case 0x13:
            printf("INX    D"); // Increment DE
            break;
        case 0x14:
            printf("INR    D"); // Increment D
            break;
        case 0x15:
            printf("DCR    D"); // Decrement D
            break;
        case 0x16:
            printf("MVI    D, %02x", code[1]); // Move immediate register to D
            opbytes = 2;
            break;
        case 0x17:
            printf("RAL"); // Rotate A left through carry
            break;
        case 0x18:
            printf("NOP");
            break;
        case 0x19:
            printf("DAD    D"); // Add DE to HL
            break;
        case 0x1a:
            printf("LDAX   D"); // Load A indirect
            break;
        case 0x1b:
            printf("DCX    D"); // Decrement DE
            break;
        case 0x1c:
            printf("INR    E"); // Increment E
            break;
        case 0x1d:
            printf("DCR    E"); // Decrement E
            break;
        case 0x1e:
            printf("MVI    E, %02x", code[1]); // Move immediate register to E
            opbytes = 2;
            break;
        case 0x1f:
            /*
            "Rotate Accumulator Right through Carry" (RAR) is a bit-manipulation instruction that shifts the bits of an accumulator register to the right,
            with the least significant bit (LSB) being moved into the carry flag, and the carry flag being moved into the most significant bit (MSB)
            */
            printf("RAR"); // Rotate A right through carry
            break;
        case 0x20:
            printf("NOP");
            break;
        case 0x21:
            printf("LXI    H, %02x%02x", code[2], code[1]); // Load immediate register pair HL
            opbytes = 3;
            break;
        case 0x22:
            printf("SHLD   addr: %02x%02x", code[2], code[1]); // Store HL direct: from HL to memory
            opbytes = 3;
            break;
        case 0x23:
            printf("INX    H"); // Increment HL
            break;
        case 0x24:
            printf("INR    H"); // Increment H
            break;
        case 0x25:
            printf("DCR    H"); // Decrement H
            break;
        case 0x26:
            printf("MVI    H, %02x", code[1]); // Move immediate register to H
            opbytes = 2;
            break;
        case 0x27:
            /*
            Decimal Adjust Accumulator. It's an instruction used to convert the result of an 8-bit binary addition of two binary
            coded decimal (BCD) numbers into two valid BCD digits.
            */
            printf("DAA"); // Decimal adjust A
            break;
        case 0x28:
            printf("NOP");
            break;
        case 0x29:
            printf("DAD    H"); // Add HL to HL
            break;
        case 0x2a:
            printf("LHLD   addr: %02x%02x", code[2], code[1]); // Load HL direct: from memory to HL
            opbytes = 3;
            break;
        case 0x2b:
            printf("DCX    H"); // Decrement HL
            break;
        case 0x2c:
            printf("INR    L"); // Increment L
            break;
        case 0x2d:
            printf("DCR    L"); // Decrement L
            break;
        case 0x2e:
            printf("MVI    L, %02x", code[1]); // Move immediate register to L
            opbytes = 2;
            break;
        case 0x2f:
            printf("CMA"); // Complements the contents of A, assigns to A
            break;
        case 0x30:
            printf("NOP");
            break;
        case 0x31:
            printf("LXI    SP, %02x%02x", code[2], code[1]); // Load immediate to stack pointer
            opbytes = 3;
            break;
        case 0x32:
            printf("STA    addr: %02x%02x", code[2], code[1]); // Store A direct: from A to memory
            opbytes = 3;
            break;
        case 0x33:
            printf("INX    SP"); // Increment stack pointer
            break;
        case 0x34:
            printf("INR    M"); // Increment memory (HL)
            break;
        case 0x35:
            printf("DCR    M"); // Decrement memory (HL)
            break;
        case 0x36:
            printf("MVI    M, %02x", code[1]); // Move immediate to memory (memory designated by HL)
            opbytes = 2;
            break;
        case 0x37:
            printf("STC"); // Sets the Carry flag bit in the status register to 1
            break;
        case 0x38:
            printf("NOP");
            break;
        case 0x39:
            printf("DAD    SP"); // Add stack pointer to HL (memory)
            break;
        case 0x3a:
            printf("LDA    addr: %02x%02x", code[2], code[1]); // Load A direct from memory
            opbytes = 3;
            break;
        case 0x3b:
            printf("DCX    SP"); // Decrement SP
            break;
        case 0x3c:
            printf("INR    A"); // Increment A
            break;
        case 0x3d:
            printf("DCR    A"); // Decrement A
            break;
        case 0x3e:
            printf("MVI    A, %02x", code[1]); // Move immediate register to A
            opbytes = 2;
            break;
        case 0x3f:
            printf("CMC"); // Complement carry: flips value of the carry flag
            break;
        case 0x40:
            printf("MOV    B, B"); // Move register B to register B
            break;
        case 0x41:
            printf("MOV    B, C"); // Move register B to register C
            break;
        case 0x42:
            printf("MOV    B, D"); // Move register B to register D
            break;
        case 0x43:
            printf("MOV    B, E"); // Move register B to register E
            break;
        case 0x44:
            printf("MOV    B, H"); // Move register B to register H
            break;
        case 0x45:
            printf("MOV    B, L"); // Move register B to register L
            break;
        case 0x46:
            printf("MOV    B, M"); // Move register B to register M
            break;
        case 0x47:
            printf("MOV    B, A"); // Move register B to register A
            break;
        case 0x48:
            printf("MOV    C, B"); // Move register C to register B
            break;
        case 0x49:
            printf("MOV    C, C"); // Move register C to register C
            break;
        case 0x4a:
            printf("MOV    C, D"); // Move register C to register D
            break;
        case 0x4b:
            printf("MOV    C, E"); // Move register C to register E
            break;
        case 0x4c:
            printf("MOV    C, H"); // Move register C to register H
            break;
        case 0x4d:
            printf("MOV    C, L"); // Move register C to register L
            break;
        case 0x4e:
            printf("MOV    C, M"); // Move register C to register M
            break;
        case 0x4f:
            printf("MOV    C, A"); // Move register C to register A
            break;
        case 0x50:
            printf("MOV    D, B"); // Move register D to register B
            break;
        case 0x51:
            printf("MOV    D, C"); // Move register D to register C
            break;
        case 0x52:
            printf("MOV    D, D"); // Move register D to register D
            break;
        case 0x53:
            printf("MOV    D, E"); // Move register D to register E
            break;
        case 0x54:
            printf("MOV    D, H"); // Move register D to register H
            break;
        case 0x55:
            printf("MOV    D, L"); // Move register D to register L
            break;
        case 0x56:
            printf("MOV    D, M"); // Move register D to register M
            break;
        case 0x57:
            printf("MOV    D, A"); // Move register D to register A
            break;
        case 0x58:
            printf("MOV    E, B"); // Move register E to register B
            break;
        case 0x59:
            printf("MOV    E, C"); // Move register E to register C
            break;
        case 0x5a:
            printf("MOV    E, D"); // Move register E to register D
            break;
        case 0x5b:
            printf("MOV    E, E"); // Move register E to register E
            break;
        case 0x5c:
            printf("MOV    E, H"); // Move register E to register H
            break;
        case 0x5d:
            printf("MOV    E, L"); // Move register E to register L
            break;
        case 0x5e:
            printf("MOV    E, M"); // Move register E to register M
            break;
        case 0x5f:
            printf("MOV    E, A"); // Move register E to register A
            break;
        case 0x60:
            printf("MOV    H, B"); // Move register H to register B
            break;
        case 0x61:
            printf("MOV    H, C"); // Move register H to register C
            break;
        case 0x62:
            printf("MOV    H, D"); // Move register H to register D
            break;
        case 0x63:
            printf("MOV    H, E"); // Move register H to register E
            break;
        case 0x64:
            printf("MOV    H, H"); // Move register H to register H
            break;
        case 0x65:
            printf("MOV    H, L"); // Move register H to register L
            break;
        case 0x66:
            printf("MOV    H, M"); // Move register H to register M
            break;
        case 0x67:
            printf("MOV    H, A"); // Move register H to register A
            break;
        case 0x68:
            printf("MOV    L, B"); // Move register L to register B
            break;
        case 0x69:
            printf("MOV    L, C"); // Move register L to register C
            break;
        case 0x6a:
            printf("MOV    L, D"); // Move register L to register D
            break;
        case 0x6b:
            printf("MOV    L, E"); // Move register L to register E
            break;
        case 0x6c:
            printf("MOV    L, H"); // Move register L to register H
            break;
        case 0x6d:
            printf("MOV    L, L"); // Move register L to register L
            break;
        case 0x6e:
            printf("MOV    L, M"); // Move register L to register M
            break;
        case 0x6f:
            printf("MOV    L, A"); // Move register L to register A
            break;
        case 0x70:
            printf("MOV    M, B"); // Move register M to register B
            break;
        case 0x71:
            printf("MOV    M, C"); // Move register M to register C
            break;
        case 0x72:
            printf("MOV    M, D"); // Move register M to register D
            break;
        case 0x73:
            printf("MOV    M, E"); // Move register M to register E
            break;
        case 0x74:
            printf("MOV    M, H"); // Move register M to register H
            break;
        case 0x75:
            printf("MOV    M, L"); // Move register M to register L
            break;
        case 0x76:
            /*
            tops the CPU's execution until an external interrupt or reset signal is received.
            The CPU will remain in a waiting state, essentially doing nothing, until an external event signals it to resume operation. 
            */
            printf("HLT"); // Halt
            break;
        case 0x77:
            printf("MOV    M, A"); // Move register M to register A
            break;
        case 0x78:
            printf("MOV    A, B"); // Move register A to register B
            break;
        case 0x79:
            printf("MOV    A, C"); // Move register A to register C
            break;
        case 0x7a:
            printf("MOV    A, D"); // Move register A to register D
            break;
        case 0x7b:
            printf("MOV    A, E"); // Move register A to register E
            break;
        case 0x7c:
            printf("MOV    A, H"); // Move register A to register H
            break;
        case 0x7d:
            printf("MOV    A, L"); // Move register A to register L
            break;
        case 0x7e:
            printf("MOV    A, M"); // Move register A to register M
            break;
        case 0x7f:
            printf("MOV    A, A"); // Move register A to register A
            break;
        case 0x80:
            printf("ADD    B"); // Adds contents of register B to register A
            break;
        case 0x81:
            printf("ADD    C"); // Adds contents of register C to register A
            break;
        case 0x82:
            printf("ADD    D"); // Adds contents of register D to register A
            break;
        case 0x83:
            printf("ADD    E"); // Adds contents of register E to register A
            break;
        case 0x84:
            printf("ADD    H"); // Adds contents of register H to register A
            break;
        case 0x85:
            printf("ADD    L"); // Adds contents of register L to register A
            break;
        case 0x86:
            printf("ADD    M"); // Adds contents of register M to register A
            break;
        case 0x87:
            printf("ADD    A"); // Adds contents of register A to register A
            break;
        case 0x88:
            printf("ADC    B"); // Adds contents of register B to register A with carry
            break;
        case 0x89:
            printf("ADC    C"); // Adds contents of register C to register A with carry
            break;
        case 0x8a:
            printf("ADC    D"); // Adds contents of register D to register A with carry
            break;
        case 0x8b:
            printf("ADC    E"); // Adds contents of register E to register A with carry
            break;
        case 0x8c:
            printf("ADC    H"); // Adds contents of register H to register A with carry
            break;
        case 0x8d:
            printf("ADC    L"); // Adds contents of register L to register A with carry
            break;
        case 0x8e:
            printf("ADC    M"); // Adds contents of register M to register A with carry
            break;
        case 0x8f:
            printf("ADC    A"); // Adds contents of register A to register A with carry
            break;
        case 0x90:
            printf("SUB    B"); // Subtracts contents of register B from register A
            break;
        case 0x91:
            printf("SUB    C"); // Subtracts contents of register C from register A
            break;
        case 0x92:
            printf("SUB    D"); // Subtracts contents of register D from register A
            break;
        case 0x93:
            printf("SUB    E"); // Subtracts contents of register E from register A
            break;
        case 0x94:
            printf("SUB    H"); // Subtracts contents of register H from register A
            break;
        case 0x95:
            printf("SUB    L"); // Subtracts contents of register L from register A
            break;
        case 0x96:
            printf("SUB    M"); // Subtracts contents of register M from register A
            break;
        case 0x97:
            printf("SUB    A"); // Subtracts contents of register A from register A
            break;
        case 0x98:
            printf("SBB    B"); // Subtracts contents of register B from register A with borrow
            break;
        case 0x99:
            printf("SBB    C"); // Subtracts contents of register C from register A with borrow
            break;
        case 0x9a:
            printf("SBB    D"); // Subtracts contents of register D from register A with borrow
            break;
        case 0x9b:
            printf("SBB    E"); // Subtracts contents of register E from register A with borrow
            break;
        case 0x9c:
            printf("SBB    H"); // Subtracts contents of register H from register A with borrow
            break;
        case 0x9d:
            printf("SBB    L"); // Subtracts contents of register L from register A with borrow
            break;
        case 0x9e:
            printf("SBB    M"); // Subtracts contents of register M from register A with borrow
            break;
        case 0x9f:
            printf("SBB    A"); // Subtracts contents of register A from register A with borrow
            break;
        case 0xa0:
            printf("ANA    B"); // Register B AND register A
            break;
        case 0xa1:
            printf("ANA    C"); // Register C AND register A
            break;
        case 0xa2:
            printf("ANA    D"); // Register D AND register A
            break;
        case 0xa3:
            printf("ANA    E"); // Register E AND register A
            break;
        case 0xa4:
            printf("ANA    H"); // Register H AND register A
            break;
        case 0xa5:
            printf("ANA    L"); // Register L AND register A
            break;
        case 0xa6:
            printf("ANA    M"); // Register M AND register A
            break;
        case 0xa7:
            printf("ANA    A"); // Register A AND register A
            break;
        case 0xa8:
            printf("XRA    B"); // Register B OR register A (exclusive)
            break;
        case 0xa9:
            printf("XRA    C"); // Register B OR register A (exclusive)
            break;
        case 0xaa:
            printf("XRA    D"); // Register B OR register A (exclusive)
            break;
        case 0xab:
            printf("XRA    E"); // Register B OR register A (exclusive)
            break;
        case 0xac:
            printf("XRA    H"); // Register B OR register A (exclusive)
            break;
        case 0xad:
            printf("XRA    L"); // Register B OR register A (exclusive)
            break;
        case 0xae:
            printf("XRA    M"); // Register B OR register A (exclusive)
            break;
        case 0xaf:
            printf("XRA    A"); // Register B OR register A (exclusive)
            break;
        case 0Xb0:
            printf("ORA    B"); // Register B OR register A
            break;
        case 0Xb1:
            printf("ORA    C"); // Register C OR register A
            break;
        case 0Xb2:
            printf("ORA    D"); // Register D OR register A
            break;
        case 0Xb3:
            printf("ORA    E"); // Register E OR register A
            break;
        case 0Xb4:
            printf("ORA    H"); // Register H OR register A
            break;
        case 0Xb5:
            printf("ORA    L"); // Register L OR register A
            break;
        case 0Xb6:
            printf("ORA    M"); // Register M OR register A
            break;
        case 0Xb7:
            printf("ORA    A"); // Register A OR register A
            break;
        case 0Xb8:
            printf("CMP    B"); // Compare register B with register A
            break;
        case 0Xb9:
            printf("CMP    C"); // Compare register C with register A
            break;
        case 0Xba:
            printf("CMP    D"); // Compare register D with register A
            break;
        case 0Xbb:
            printf("CMP    E"); // Compare register E with register A
            break;
        case 0Xbc:
            printf("CMP    H"); // Compare register H with register A
            break;
        case 0Xbd:
            printf("CMP    L"); // Compare register L with register A
            break;
        case 0Xbe:
            printf("CMP    M"); // Compare register M with register A
            break;
        case 0Xbf:
            printf("CMP    A"); // Compare register A with register A
            break;
        case 0Xc0:
            printf("RNZ"); // Return on no zero: if zero flag is not set, jump to address stored on stack
            break;
        case 0Xc1:
            printf("POP    B"); // POP BC from stack
            break;
        case 0Xc2:
            printf("JNZ    addr: %02x%02x", code[2], code[1]); // Jump if zero flag is not set
            opbytes = 3;
            break;
        case 0Xc3:
            printf("JMP    addr: %02x%02x", code[2], code[1]); // Jump unconditional
            opbytes = 3;
            break;
        case 0Xc4:
            printf("CNZ    addr: %02x%02x", code[2], code[1]); // Call on non-zero: jump to a new location in memory if the zero flag is not set
            opbytes = 3;
            break;
        case 0Xc5:
            printf("PUSH   B"); // Push contents of register BC to stack
            break;
        case 0Xc6:
            printf("ADI    %02x", code[1]); // Add immediate to register A
            opbytes = 2;
            break;
        case 0Xc7:
            printf("RST    0"); // Restart 0
            break;
        case 0Xc8:
            printf("RZ"); // Return on zero: if zero flag is set, jump to address stored on stack
            break;
        case 0Xc9:
            printf("RET"); // Unconditional return: jump to address on stack
            break;
        case 0Xca:
            printf("JZ     addr: %02x%02x", code[2], code[1]); // Jump if zero flag is set
            opbytes = 3;
            break;
        case 0Xcb:
            printf("JMP    addr: %02x%02x", code[2], code[1]); // Jump unconditional
            opbytes = 3;
            break;
        case 0Xcc:
            printf("CZ     addr: %02x%02x", code[2], code[1]); // Call on zero: jump to new location in memory if zero flag is set
            opbytes = 3;
            break;
        case 0Xcd:
            printf("CALL   addr: %02x%02x", code[2], code[1]); // Unconditional call: jump to new location in memory
            opbytes = 3;
            break;
        case 0Xce:
            printf("ACI    %02x", code[1]); // Add immediate to A with carry
            opbytes = 2;
            break;
        case 0Xcf:
            printf("RST    1"); // Restart 1
            break;
        case 0Xd0:
            printf("RNC"); // Return on no carry: if carry flag is not set, jump to address stored on stack
            break;
        case 0Xd1:
            printf("POP    D"); // POP DE from stack
            break;
        case 0Xd2:
            printf("JNC    addr: %02x%02x", code[2], code[1]); // Jump if carry flag is not set
            opbytes = 3;
            break;
        case 0Xd3:
            printf("OUT    port: %02x", code[1]); // Output content from A to port address
            opbytes = 2;
            break;
        case 0Xd4:
            printf("CNC    addr: %02x%02x", code[2], code[1]); // Call on non-carry: jump to a new location in memory if the carry flag is not set
            opbytes = 3;
            break;
        case 0Xd5:
            printf("PUSH   D"); // Push contents of register DE to stack
            break;
        case 0Xd6:
            printf("SUI    %02x", code[1]); // Subtract immediate from register A
            opbytes = 2;
            break;
        case 0Xd7:
            printf("RST    2"); // Restart 2
            break;
        case 0Xd8:
            printf("RC"); // Return on carry: if carry flag is set, jump to address stored on stack
            break;
        case 0Xd9:
            printf("RET"); // Unconditional return: jump to address on stack
            break;
        case 0Xda:
            printf("JC     addr: %02x%02x", code[2], code[1]); // Jump if carry flag is set
            opbytes = 3;
            break;
        case 0Xdb:
            printf("IN     port: %02x", code[1]); // Input content from port address
            opbytes = 2;
            break;
        case 0Xdc:
            printf("CC     addr: %02x%02x",code[2],code[1]); // Call on carry: jump to new location in memory if carry flag is set
            opbytes = 3;
            break;
        case 0Xdd:
            printf("CALL   addr: %02x%02x",code[2],code[1]); // Unconditional call: jump to new location in memory
            opbytes = 3;
            break;
        case 0Xde:
            printf("SBI    %02x", code[1]); // Subtract immediate from A with borrow
            opbytes = 2;
            break;
        case 0Xdf:
            printf("RST    3"); // Restart 3
            break;
        case 0Xe0:
            printf("RPO"); // Return on parity odd: if parity flag is odd, jump to address stored on stack
            break;
        case 0Xe1:
            printf("POP    H"); // POP HL from stack
            break;
        case 0Xe2:
            printf("JPO    addr: %02x%02x", code[2], code[1]); // Jump if parity flag is odd
            opbytes = 3;
            break;
        case 0Xe3:
            printf("XTHL"); // Exchange contents from HL and top of stack
            break;
        case 0Xe4:
            printf("CPO    addr: %02x%02x", code[2], code[1]); // Call on parity-odd: jump to a new location in memory if the parity flag is odd
            opbytes = 3;
            break;
        case 0Xe5:
            printf("PUSH   H"); // Push contents of register HL to stack
            break;
        case 0Xe6:
            printf("ANI    %02x", code[1]); // Add immediate to register A
            opbytes = 2;
            break;
        case 0Xe7:
            printf("RST    4"); // Restart 4
            break;
        case 0Xe8:
            printf("RPE"); // Return on parity even: if parity flag is even, jump to address stored on stack
            break;
        case 0Xe9:
            printf("PCHL"); // Copies the contents from HL to the program counter
            break;
        case 0Xea:
            printf("JPE    addr: %02x%02x", code[2], code[1]); // Jump if parity flag is even
            opbytes = 3;
            break;
        case 0Xeb:
            printf("XCHG"); // Exchange contents of HL and DE
            break;
        case 0Xec:
            printf("CPE    addr: %02x%02x",code[2],code[1]); // Call on parity even: jump to new location in memory if parity flag is even
            opbytes = 3;
            break;
        case 0Xed:
            printf("CALL   addr: %02x%02x",code[2],code[1]); // Unconditional call: jump to new location in memory
            opbytes = 3;
            break;
        case 0Xee:
            printf("XRI    %02x", code[1]); // immediate OR A (exclusive)
            opbytes = 2;
            break;
        case 0Xef:
            printf("RST    5"); // Restart 3
            break;
        case 0Xf0:
            printf("RP"); // Return on positive: if sign flag is not set, jump to address stored on stack
            break;
        case 0Xf1:
            printf("POP    PSW"); // POP A and flags from stack
            break;
        case 0Xf2:
            printf("JP     addr: %02x%02x", code[2], code[1]); // Jump if sign flag is not set
            opbytes = 3;
            break;
        case 0Xf3:
            printf("DI"); // Disable interupt
            break;
        case 0Xf4:
            printf("CP     addr: %02x%02x", code[2], code[1]); // Call on positive: jump to a new location in memory if the sign flag is not set
            opbytes = 3;
            break;
        case 0Xf5:
            printf("PUSH   PSW"); // Push contents of register A and flags to stack
            break;
        case 0Xf6:
            printf("ORI    %02x", code[1]); // Immediate OR register A
            opbytes = 2;
            break;
        case 0Xf7:
            printf("RST    6"); // Restart 6
            break;
        case 0Xf8:
            printf("RM"); // Return on minus: if sign flag is set, jump to address stored on stack
            break;
        case 0Xf9:
            printf("SPHL"); // Copies the contents from HL to the stack pointer
            break;
        case 0Xfa:
            printf("JM     addr: %02x%02x", code[2], code[1]); // Jump on minus: if sign flag is set
            opbytes = 3;
            break;
        case 0Xfb:
            printf("EI"); // Enable interupts
            break;
        case 0Xfc:
            printf("CM     addr: %02x%02x",code[2],code[1]); // Call on minus: jump to new location in memory if sign flag is set
            opbytes = 3;
            break;
        case 0Xfd:
            printf("CALL   addr: %02x%02x",code[2],code[1]); // Unconditional call: jump to new location in memory
            opbytes = 3;
            break;
        case 0Xfe:
            printf("CPI    %02x", code[1]); // Compare immediate with contents of A
            opbytes = 2;
            break;
        case 0Xff:
            printf("RST    7"); // Restart 7
            break;
    }


    printf("\n");

    return opbytes;
};

int main (int argc, char**argv) {
    FILE *f= fopen("test/disassembler_test_file.bin", "rb");   // fopen(argv[1], "rb");
    if (f==NULL)
    {
        printf("error: Couldn't open %s\n", argv[1]);
        exit(1);
    };

    // Get the file size and read it into a memory buffer
    fseek(f, 0L, SEEK_END);
    int fsize = ftell(f);
    fseek(f, 0L, SEEK_SET);

    unsigned char *buffer = (unsigned char *) malloc(fsize);

    fread(buffer, fsize, 1, f);
    fclose(f);

    int pc = 0;

    while (pc < fsize) {
        pc += Disassemble8080Op(buffer, pc);
    }
    return 0;
};
