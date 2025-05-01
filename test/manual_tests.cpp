#include "../emulator.h" 
#include <stdio.h>      
#include <stdlib.h>     
#include <cassert>      

// Helper function to report errors
void test_failed(const char* test_name, const char* message) {
    printf("[-] TEST FAILED: %s (%s)\n", test_name, message);
}

void test_passed(const char* test_name) {
    printf("[+] TEST PASSED: %s\n", test_name);
}


void test_op_nop() {
    const char* test_name = "0x00 NOP";
    State8080 state;
    initialize_emulator(&state); // Reset state

    state.pc = 0x0100; // Start PC at some address
    state.memory[state.pc] = 0x00; // Put NOP opcode in memory

    Emulate8080Op(&state, false);

    // Verify results
    if (state.pc != 0x0101) {
        test_failed(test_name, "PC should increment by 1");
        printf("    Expected PC: 0x0101, Got: 0x%04X\n", state.pc);
        return; 
    }

    test_passed(test_name);
}

void test_op_mvi_b() {
     const char* test_name = "0x06 MVI B";
     State8080 state;
     initialize_emulator(&state);

     state.pc = 0x0200;
     state.memory[state.pc] = 0x06; 
     state.memory[state.pc + 1] = 0xAB;

     Emulate8080Op(&state, false);

     if (state.b != 0xAB) {
         test_failed(test_name, "Register B should be 0xAB");
         printf("    Expected B: 0xAB, Got: 0x%02X\n", state.b);
         return;
     }
     if (state.pc != 0x0202) {
          test_failed(test_name, "PC should increment by 2");
         printf("    Expected PC: 0x0202, Got: 0x%04X\n", state.pc);
         return;
     }


     test_passed(test_name);
}

void test_op_lxi_b(){
    const char* test_name = "0x01 LXI B,D16";
    State8080 state;
    initialize_emulator(&state);
    
    state.pc = 0x300;
    state.memory[state.pc] = 0x01;
    state.memory[state.pc +1] = 0x34;
    state.memory[state.pc +2] = 0x12;
    
    Emulate8080Op(&state, false);
    
    if (state.b != 0x12 || state.c !=0x34){
        test_failed(test_name, "Register b should be 0x12");
        printf(" Expected c:34, got : 0x%4x\n", state.c);
        return;
    }
    
    if (state.pc != 0x303){
        test_failed(test_name, "PC should increment by 3");
        printf(" Expected 0x303, but got 0x%2x\n", state.pc);
        return;
    }
    test_passed(test_name);
    
}

void test_op_dcr_b(){
    const char* test_name = "0x05 DRC B";
    State8080 state;
    initialize_emulator(&state);
    
    state.pc = 0x400;
    state.b = 0x01;
    state.flags.c = 1;
    state.memory[state.pc] = 0x05;
    
    Emulate8080Op(&state, false);
    
    if(state.b != 0x00){
        test_failed(test_name, "Register B should have decremented");
        printf("Register B is currently 0x%2x\n", state.b);
        return;
    }

    if (!state.flags.z){
        test_failed(test_name, "Z flag should be set");
        return;
    }
    
    if(state.flags.s){
        test_failed(test_name, "Sign flag should not be set");
        return;
    }
    
    if(!state.flags.p){
        test_failed(test_name, "parity flag should be set");
        return;
    }
    
    if(state.flags.ac){
        test_failed(test_name, "ac flag should not be modified");
        return;
    }
    
    if(state.pc != 0x401){
        test_failed(test_name, "pc did not increment properly");
        printf(" current PC is 0x%4x\n", state.pc);
    }
    
    test_passed(test_name);
}

void test_op_dad_b(){
    const char* test_name = "0x09 DAD B";
    State8080 state;
    initialize_emulator(&state);
    
    state.h = 0x12;
    state.l = 0x34;
    state.b = 0x11;
    state.c = 0x11;
    state.pc = 0x100;
    state.memory[state.pc] = 0x09;
    
    Emulate8080Op(&state, false);
    
    uint16_t hl = (state.h <<8) | state.l;
    if (hl != 0x2345){
        test_failed(test_name, "hl should be 0x2345");
        printf("hl is 0x%4x\n", hl);
        return;
    }
    
    if(state.flags.c != 0){
        test_failed(test_name, "C should be clear");
        return;
    }
    
    if(state.pc != 0x101){
        test_failed(test_name, "PC address should be 0x102");
        printf("PC address is 0x%4x\n", state.pc);
        return;
    }
    
    test_passed(test_name);
}

void test_op_dcr_c(){
    const char* test_name = "0x0d DCR C";
    State8080 state;
    initialize_emulator(&state);
    
    state.pc = 0x200;
    state.c = 0x02;
    state.flags.c = 1;
    state.memory[state.pc] = 0x0d;
    
    Emulate8080Op(&state, false);
    
    if (state.c != 0x01){
        test_failed(test_name, "Register C should be 0x01");
        printf("Register c is showing 0x%2x\n", state.c);
        return;
    }
    
    if (state.flags.z){
        test_failed(test_name, "Zero flag should not be set.");
        return;
    }
    
    if (state.flags.s){
        test_failed(test_name, "Sign flag should not be set"); //not sure if test is incorrect or function is incorrect.
        return;
    }
    
    if (state.flags.p){
        test_failed(test_name, "Parity flag should not be set");
        return;
    }
    
    if(state.flags.ac){
        test_failed(test_name, "ac flag should not be modified");
        return;
    }
    
    if (state.pc != 0x201){
        test_failed(test_name," pc should be 0x201");
        printf("state pc is showing as 0x%2x\n", state.pc);
    }
    
    test_passed(test_name);
}

void test_op_mvi_c(){
    
    const char* test_name = "0x0e MVI C, D8";
    State8080 state;
    initialize_emulator(&state);
    
    state.pc = 0x0200;
    state.c = 0x01;
    state.flags.c = 1;
    state.memory[state.pc] = 0x0e;
    
    Emulate8080Op(&state, false);
    
    if(state.c != 0x00){
        test_failed(test_name, "Register C should be 0x00");
        printf("Register C is 0x%2x\n", state.c);
        return;
    }
    
    if (!state.flags.z){
        test_failed(test_name, "Zero flag should be set");
        printf("Zero flag is %d\n", state.flags.z);
        return;
    }
    
    if(state.flags.s){
        test_failed(test_name, "Sign flag should not be set");
        return;
    }
    
    if(state.flags.p){
        test_failed(test_name, "Parity flag should be set");
        return;
    }
    
    if(!state.flags.ac){
        test_failed(test_name, "Auxiliary cary flag should be set when borrowing from bit 4");
        return;
    }
    
    if(!state.flags.c){
        test_failed(test_name,"Carry flags should be unchanged");
        return;
    }
    
    if(state.pc!=0x0201){
        test_failed(test_name, "PC should be 0x0201");
        printf("state.pc is showing as 0x%4x\n", state.pc);
        return;
    }
                    
    
    test_passed(test_name);
}

void test_op_rrc(){
    const char* test_name = "0x0f RRC";
    State8080 state;
    initialize_emulator(&state);
    
    state.pc = 0x0100;
    state.a = 0x93;
    state.flags.c = 0;
    state.memory[state.pc] = 0x0f;
    
    Emulate8080Op(&state, false);
    
    if(state.a != 0xC9){
        test_failed(test_name, "Register A should be 0xc9");
        printf("register a is 0x%2x\n", state.a);
        return;
    }
    
    if(state.flags.c != 1){
        test_failed(test_name, "Carry flag should be 1");
        return;
    }
    
    if(state.pc != 0x0101){
        test_failed(test_name, "PC should be 0x0101");
        printf("actual value at state pc is 0x%4x\n", state.pc);
        return;
    }
    
    test_passed(test_name);
}

void test_op_lxi_d(){
    
    const char* test_name = "0x11 LXI D,D16";
    State8080 state;
    initialize_emulator(&state);
    
    state.pc = 0x0100;
    state.memory[state.pc] = 0x11;
    state.memory[state.pc + 1] = 0x34;
    state.memory[state.pc + 2] = 0x12;
    
    Emulate8080Op(&state, false);
    
    if(state.d != 0x12){
        test_failed(test_name,"Register D should be 0x12");
        printf("Register d is currently 0x%2d\n", state.d);
        return;
    }
    
    if(state.e != 0x34){
        test_failed(test_name,"Register E should be 0x12");
        printf("Register d is currently 0x%2d\n", state.e);
        return;
    }
    
    if(state.pc != 0x0103){
        test_failed(test_name,"Pc should have been 0x0103");
        printf("State PC was actually 0x%4x\n", state.pc);
        return;
    }
    
    test_passed(test_name);
}

void test_op_inx_d(){
    const char* test_name = "0x13 INX D";
    State8080 state;
    initialize_emulator(&state);
    
    state.d = 0x12;
    state.e = 0x33;
    
    state.pc = 0x40f;
    state.memory[state.pc] = 0x13;

    Emulate8080Op(&state, false);
    
    uint16_t de = (state.d << 8) | state.e;
    
    if(de != 0x1234){
        test_failed(test_name, "Register DE should be 0x1234");
        printf("Register DE is currently 0x%4x\n", de);
        return;
    }
    
    if(state.pc != 0x410){
        test_failed(test_name," PC should be 0x410");
        printf("Actual value of PC is 0x%3x\n", state.pc);
        return;
    }
    test_passed(test_name);
}

void test_op_dad_d(){
    const char* test_name = "0x19 DAD D";
    State8080 state;
    initialize_emulator(&state);
    
    state.h = 0x12;
    state.l = 0x34;
    state.d = 0x11;
    state.e = 0x12;
    
    state.pc = 0x0200;
    state.memory[state.pc] = 0x19;
    
    Emulate8080Op(&state, false);
    
    uint16_t hl = (state.h << 8) | state.l;
    uint16_t expected_hl = 0x1234 + 0x1112;
    
    if(hl != expected_hl){
        test_failed(test_name, "Expected result should be HL + DE");
        printf("actual result is 0x%4x\n", hl);
        return;
    }
    
    if (state.flags.c != 0){
        test_failed(test_name, "Carry flag should be 0");
        return;
    }
    
    if (state.pc != 0x0201){
        test_failed(test_name,"PC should be 0x0201");
        printf("actual value of pc is 0x%4x\n", state.pc);
        return;
    }
    
    test_passed(test_name);
}

void test_op_ldax_d(){
    const char* test_name = "0x1a LDAX D";
    State8080 state;
    initialize_emulator(&state);
    
    state.d = 0x20;
    state.e = 0x10;
    state.memory[0x2010] = 0xab;
    state.pc = 0x0300;
    state.memory[state.pc] = 0x1a;
    Emulate8080Op(&state, false);
    
    if (state.a!= 0xab){
        test_failed(test_name, "Register A should be been loaded from DE");
        printf("Actual value of A is 0x%02x\n", state.a);
        return;
    }
    
    if(state.pc != 0x0301){
        test_failed(test_name, "PC should be 0x0301");
        printf("The actual value of pc is 0x%4x\n", state.pc);
        return;
    }
    test_passed(test_name);
}

void test_op_lxi_h() {
    const char* test_name = "0x21 LXI H";
    State8080 state;
    initialize_emulator(&state);

    state.pc = 0x500;
    state.memory[state.pc] = 0x21;
    state.memory[state.pc + 1] = 0x78;
    state.memory[state.pc + 2] = 0x56;

    Emulate8080Op(&state, false);

    if (state.h != 0x56 || state.l != 0x78) {
        test_failed(test_name, "h should be 0x56 and l should be 0x78");
        printf("h: 0x%02x, l: 0x%02x\n", state.h, state.l);
        return;
    }

    if (state.pc != 0x503) {
        test_failed(test_name, "pc should increment by 3");
        printf("pc: 0x%04x\n", state.pc);
        return;
    }

    test_passed(test_name);
}


void test_op_inx_h() {
    const char* test_name = "0x23 INX H";
    State8080 state;
    initialize_emulator(&state);

    state.h = 0x12;
    state.l = 0xFF;
    state.pc = 0x600;
    state.memory[state.pc] = 0x23;

    Emulate8080Op(&state, false);

    if (state.h != 0x13 || state.l != 0x00) {
        test_failed(test_name, "hl should have incremented correctly");
        printf("h: 0x%02x, l: 0x%02x\n", state.h, state.l);
        return;
    }

    if (state.pc != 0x601) {
        test_failed(test_name, "pc should increment by 1");
        printf("pc: 0x%04x\n", state.pc);
        return;
    }

    test_passed(test_name);
}

void test_op_mvi_h() {
    const char* test_name = "0x26 MVI H";
    State8080 state;
    initialize_emulator(&state);

    state.pc = 0x700;
    state.memory[state.pc] = 0x26;
    state.memory[state.pc + 1] = 0xAB;

    Emulate8080Op(&state, false);

    if (state.h != 0xAB) {
        test_failed(test_name, "h should be 0xAB");
        printf("h: 0x%02x\n", state.h);
        return;
    }

    if (state.pc != 0x702) {
        test_failed(test_name, "pc should increment by 2");
        printf("pc: 0x%04x\n", state.pc);
        return;
    }

    test_passed(test_name);
}

void test_op_dad_h() {
    const char* test_name = "0x29 DAD H";
    State8080 state;
    initialize_emulator(&state);

    state.h = 0x12;
    state.l = 0x34;
    state.h = 0x56;
    state.l = 0x78;
    state.pc = 0x800;
    state.memory[state.pc] = 0x29;

    Emulate8080Op(&state, false);

    uint16_t hl = (state.h << 8) | state.l;
    if (hl != 0xACF0) {
        test_failed(test_name, "hl should be 0xACF0");
        printf("hl: 0x%04x\n", hl);
        return;
    }

    if (state.pc != 0x801) {
        test_failed(test_name, "pc should increment by 1");
        printf("pc: 0x%04x\n", state.pc);
        return;
    }

    test_passed(test_name);
}

void test_op_lxi_sp() {
    const char* test_name = "0x31 LXI SP";
    State8080 state;
    initialize_emulator(&state);

    state.pc = 0x900;
    state.memory[state.pc] = 0x31;
    state.memory[state.pc + 1] = 0x34;
    state.memory[state.pc + 2] = 0x12;

    Emulate8080Op(&state, false);

    if (state.sp != 0x1234) {
        test_failed(test_name, "sp should be 0x1234");
        printf("sp: 0x%04x\n", state.sp);
        return;
    }

    if (state.pc != 0x903) {
        test_failed(test_name, "pc should increment by 3");
        printf("pc: 0x%04x\n", state.pc);
        return;
    }

    test_passed(test_name);
}

void test_op_sta() {
    const char* test_name = "0x32 STA";
    State8080 state;
    initialize_emulator(&state);

    state.a = 0x56;
    state.pc = 0xA00;
    state.memory[state.pc] = 0x32;
    state.memory[state.pc + 1] = 0x00;
    state.memory[state.pc + 2] = 0x20;

    Emulate8080Op(&state, false);

    if (state.memory[0x2000] != 0x56) {
        test_failed(test_name, "memory at 0x2000 should be 0x56");
        printf("memory[0x2000]: 0x%02x\n", state.memory[0x2000]);
        return;
    }

    if (state.pc != 0xA03) {
        test_failed(test_name, "pc should increment by 3");
        printf("pc: 0x%04x\n", state.pc);
        return;
    }

    test_passed(test_name);
}

void test_op_mvi_m() {
    const char* test_name = "0x36 MVI M";
    State8080 state;
    initialize_emulator(&state);

    state.h = 0x20;
    state.l = 0x00;
    state.pc = 0xB00;
    state.memory[state.pc] = 0x36;
    state.memory[state.pc + 1] = 0x78;

    Emulate8080Op(&state, false);

    if (state.memory[0x2000] != 0x78) {
        test_failed(test_name, "memory at 0x2000 should be 0x78");
        printf("memory[0x2000]: 0x%02x\n", state.memory[0x2000]);
        return;
    }

    if (state.pc != 0xB02) {
        test_failed(test_name, "pc should increment by 2");
        printf("pc: 0x%04x\n", state.pc);
        return;
    }

    test_passed(test_name);
}

void test_op_mov_d() {
    const char* test_name = "0x42 MOV D,M";
    State8080 state;
    initialize_emulator(&state);

    state.h = 0x20;
    state.l = 0x10;
    state.memory[0x2010] = 0x99;
    state.pc = 0xC00;
    state.memory[state.pc] = 0x56;

    Emulate8080Op(&state, false);

    if (state.d != 0x99) {
        test_failed(test_name, "d should be 0x99");
        printf("d: 0x%02x\n", state.d);
        return;
    }

    if (state.pc != 0xC01) {
        test_failed(test_name, "pc should increment by 1");
        printf("pc: 0x%04x\n", state.pc);
        return;
    }

    test_passed(test_name);
}

void test_op_mov_e() {
    const char* test_name = "0x5B MOV E,E";
    State8080 state;
    initialize_emulator(&state);

    state.e = 0x77;
    state.pc = 0xD00;
    state.memory[state.pc] = 0x5B;

    Emulate8080Op(&state, false);

    if (state.e != 0x77) {
        test_failed(test_name, "e should remain 0x77");
        printf("e: 0x%02x\n", state.e);
        return;
    }

    if (state.pc != 0xD01) {
        test_failed(test_name, "pc should increment by 1");
        printf("pc: 0x%04x\n", state.pc);
        return;
    }

    test_passed(test_name);
}

void test_op_mov_h() {
    const char* test_name = "0x64 MOV H,H";
    State8080 state;
    initialize_emulator(&state);

    state.h = 0x88;
    state.pc = 0xD10;
    state.memory[state.pc] = 0x64;

    Emulate8080Op(&state, false);

    if (state.h != 0x88) {
        test_failed(test_name, "h should remain 0x88");
        printf("h: 0x%02x\n", state.h);
        return;
    }

    if (state.pc != 0xD11) {
        test_failed(test_name, "pc should increment by 1");
        printf("pc: 0x%04x\n", state.pc);
        return;
    }

    test_passed(test_name);
}

void test_op_mov_l() {
    const char* test_name = "0x6D MOV L,L";
    State8080 state;
    initialize_emulator(&state);

    state.l = 0x99;
    state.pc = 0xD20;
    state.memory[state.pc] = 0x6D;

    Emulate8080Op(&state, false);

    if (state.l != 0x99) {
        test_failed(test_name, "l should remain 0x99");
        printf("l: 0x%02x\n", state.l);
        return;
    }

    if (state.pc != 0xD21) {
        test_failed(test_name, "pc should increment by 1");
        printf("pc: 0x%04x\n", state.pc);
        return;
    }

    test_passed(test_name);
}

void test_op_mov_m() {
    const char* test_name = "0x77 MOV M,A";
    State8080 state;
    initialize_emulator(&state);

    state.h = 0x20;
    state.l = 0x30;
    state.a = 0xAB;
    state.pc = 0xD30;
    state.memory[state.pc] = 0x77;

    Emulate8080Op(&state, false);

    if (state.memory[0x2030] != 0xAB) {
        test_failed(test_name, "memory at 0x2030 should be 0xAB");
        printf("memory[0x2030]: 0x%02x\n", state.memory[0x2030]);
        return;
    }

    if (state.pc != 0xD31) {
        test_failed(test_name, "pc should increment by 1");
        printf("pc: 0x%04x\n", state.pc);
        return;
    }

    test_passed(test_name);
}

void test_op_mov_a_d() {
    const char* test_name = "0x7A MOV A,D";
    State8080 state;
    initialize_emulator(&state);

    state.d = 0x44;
    state.pc = 0xD40;
    state.memory[state.pc] = 0x7A;

    Emulate8080Op(&state, false);

    if (state.a != 0x44) {
        test_failed(test_name, "a should be 0x44");
        printf("a: 0x%02x\n", state.a);
        return;
    }

    if (state.pc != 0xD41) {
        test_failed(test_name, "pc should increment by 1");
        printf("pc: 0x%04x\n", state.pc);
        return;
    }

    test_passed(test_name);
}

void test_op_mov_a_e() {
    const char* test_name = "0x7B MOV A,E";
    State8080 state;
    initialize_emulator(&state);

    state.e = 0x55;
    state.pc = 0xD50;
    state.memory[state.pc] = 0x7B;

    Emulate8080Op(&state, false);

    if (state.a != 0x55) {
        test_failed(test_name, "a should be 0x55");
        printf("a: 0x%02x\n", state.a);
        return;
    }

    if (state.pc != 0xD51) {
        test_failed(test_name, "pc should increment by 1");
        printf("pc: 0x%04x\n", state.pc);
        return;
    }

    test_passed(test_name);
}

void test_op_mov_a_h() {
    const char* test_name = "0x7C MOV A,H";
    State8080 state;
    initialize_emulator(&state);

    state.h = 0x66;
    state.pc = 0xD60;
    state.memory[state.pc] = 0x7C;

    Emulate8080Op(&state, false);

    if (state.a != 0x66) {
        test_failed(test_name, "a should be 0x66");
        printf("a: 0x%02x\n", state.a);
        return;
    }

    if (state.pc != 0xD61) {
        test_failed(test_name, "pc should increment by 1");
        printf("pc: 0x%04x\n", state.pc);
        return;
    }

    test_passed(test_name);
}

void test_op_mov_a_m() {
    const char* test_name = "0x7E MOV A,M";
    State8080 state;
    initialize_emulator(&state);

    state.h = 0x20;
    state.l = 0x40;
    state.memory[0x2040] = 0x77;
    state.pc = 0xD70;
    state.memory[state.pc] = 0x7E;

    Emulate8080Op(&state, false);

    if (state.a != 0x77) {
        test_failed(test_name, "a should be 0x77");
        printf("a: 0x%02x\n", state.a);
        return;
    }

    if (state.pc != 0xD71) {
        test_failed(test_name, "pc should increment by 1");
        printf("pc: 0x%04x\n", state.pc);
        return;
    }

    test_passed(test_name);
}

void test_op_ana_b(){
    const char* test_name = "0xa0 ANA B";
    State8080 state;
    initialize_emulator(&state);
    
    state.a = 0x12;
    state.b = 0x11;
    state.pc = 0x0100;
    state.memory[state.pc] = 0xa0;
    uint8_t expected_a = state.a & state.b;
    
    Emulate8080Op(&state, false);
    
    if(state.a != expected_a){
        test_failed(test_name, "Expected value of Register A incorrect.");
        printf("current value in register a is 0x%2x\n", state.a);
        return;
    }
    
    if (!state.flags.z) {
        test_failed(test_name, "zero flag should be set");
        return;
    }

    if (state.flags.s) {
        test_failed(test_name, "sign flag should not be set");
        return;
    }

    if (!state.flags.p) {
        test_failed(test_name, "parity flag should be set");
        return;
    }

    if (state.flags.c) {
        test_failed(test_name, "carry flag should not be set");
        return;
    }

    if (!state.flags.ac) {
        test_failed(test_name, "auxiliary carry flag should be set");
        return;
    }
    
    if(state.pc != 0x0101){
        test_failed(test_name, "PC value was expected to be 0x0101");
        printf(" Actual value of PC was 0x%4x\n", state.pc);
        return;
    }
    test_passed(test_name);
}

void test_op_xra_b() {
    const char* test_name = "0xA8 XRA B";
    State8080 state;
    initialize_emulator(&state);

    state.a = 0xFF;
    state.b = 0xFF;
    state.pc = 0x600;
    state.memory[state.pc] = 0xA8;

    Emulate8080Op(&state, false);

    if (state.a != 0x00) {
        test_failed(test_name, "a should be 0x00");
        printf("a: 0x%02x\n", state.a);
        return;
    }

    if (!state.flags.z) {
        test_failed(test_name, "zero flag should be set");
        return;
    }

    if (state.flags.s) {
        test_failed(test_name, "sign flag should not be set");
        return;
    }

    if (!state.flags.p) {
        test_failed(test_name, "parity flag should be set");
        return;
    }

    if (state.flags.c) {
        test_failed(test_name, "carry flag should not be set");
        return;
    }

    if (state.flags.ac) {
        test_failed(test_name, "auxiliary carry flag should not be set");
        return;
    }

    if (state.pc != 0x601) {
        test_failed(test_name, "pc should increment by 1");
        printf("pc: 0x%04x\n", state.pc);
        return;
    }

    test_passed(test_name);
}

void test_op_pop_b(){
    const char* test_name = "0xc1 POP B";
    State8080 state;
    initialize_emulator(&state);
    
    state.sp = 0x1000;
    state.memory[state.sp] = 0x34;
    state.memory[state.sp + 1] = 0x12;
    state.memory[0x0200] = 0xc1;
    state.pc = 0x0200;
    
    Emulate8080Op(&state, false);
    
    if (state.b != 0x12){
        test_failed(test_name, "Register B should be 0x12");
        printf("The value of register b is 0x%2x\n", state.b);
        return;
    }
    
    if(state.c != 0x34){
        test_failed(test_name, "Register C ahould be 0x34");
        printf("The current value of register c is 0x%2x\n", state.c);
        return;
    }
    
    if(state.sp != 0x1002){
        test_failed(test_name, "SP should be equal to 1002");
        printf("Current value of the stack pointer is 0x%4x\n", state.sp);
        return;
    }
    
    if(state.pc != 0x0201){
        test_failed(test_name, "PC should be 0x0201");
        printf("Current value of pc is 0x%4x\n", state.pc);
        return;
    }
    
    test_passed(test_name);
}

void test_op_jnz_adr() {
    const char* test_name = "0xC2 JNZ adr";
    State8080 state;
    initialize_emulator(&state);

    state.pc = 0x300;
    state.memory[state.pc] = 0xc2;
    state.memory[state.pc + 1] = 0x78;
    state.memory[state.pc + 2] = 0x56;
    state.flags.z = 0;

    Emulate8080Op(&state, false);

    if (state.pc != 0x5678) {
        test_failed(test_name, "PC should jump to 0x5678 when zero flag is not set");
        printf("Actual PC: 0x%04x\n", state.pc);
        return;
    }

    test_passed(test_name);
}

void test_op_jmp_adr() {
    const char* test_name = "0xC3 JMP adr";
    State8080 state;
    initialize_emulator(&state);

    state.pc = 0x400;
    state.memory[state.pc] = 0xc3;
    state.memory[state.pc + 1] = 0x00;
    state.memory[state.pc + 2] = 0x20;

    Emulate8080Op(&state, false);

    if (state.pc != 0x2000) {
        test_failed(test_name, "PC should jump to 0x2000");
        printf("Actual PC: 0x%04x\n", state.pc);
        return;
    }

    test_passed(test_name);
}

void test_op_push_b() {
    const char* test_name = "0xC5 PUSH B";
    State8080 state;
    initialize_emulator(&state);

    state.pc = 0x500;
    state.sp = 0x3000;
    state.b = 0x12;
    state.c = 0x34;
    state.memory[state.pc] = 0xc5;

    Emulate8080Op(&state, false);

    if (state.memory[state.sp + 1] != 0x12 || state.memory[state.sp] != 0x34) {
        test_failed(test_name, "Memory at SP should contain C and B after PUSH");
        printf("Memory at SP: 0x%02x 0x%02x\n", state.memory[state.sp], state.memory[state.sp + 1]);
        return;
    }

    if (state.sp != 0x2ffe) {
        test_failed(test_name, "SP should decrease by 2 after PUSH");
        printf("Actual SP: 0x%04x\n", state.sp);
        return;
    }

    if (state.pc != 0x501) {
        test_failed(test_name, "PC should move by 1");
        printf("Actual PC: 0x%04x\n", state.pc);
        return;
    }

    test_passed(test_name);
}

void test_op_adi_d8() {
    const char* test_name = "0xC6 ADI d8";
    State8080 state;
    initialize_emulator(&state);

    state.pc = 0x600;
    state.a = 0x10;
    state.memory[state.pc] = 0xc6;
    state.memory[state.pc + 1] = 0x22;

    Emulate8080Op(&state, false);

    if (state.a != 0x32) {
        test_failed(test_name, "A should be 0x32 after addition");
        printf("Actual A: 0x%02x\n", state.a);
        return;
    }

    if (state.pc != 0x602) {
        test_failed(test_name, "PC should move by 2");
        printf("Actual PC: 0x%04x\n", state.pc);
        return;
    }

    test_passed(test_name);
}

void test_op_ret() {
    const char* test_name = "0xC9 RET";
    State8080 state;
    initialize_emulator(&state);

    state.sp = 0x4000;
    state.memory[state.sp] = 0x78;
    state.memory[state.sp + 1] = 0x56;
    state.memory[0x200] = 0xc9;
    state.pc = 0x200;

    Emulate8080Op(&state, false);

    if (state.pc != 0x5678) {
        test_failed(test_name, "PC should be set to address popped from stack");
        printf("Actual PC: 0x%04x\n", state.pc);
        return;
    }

    if (state.sp != 0x4002) {
        test_failed(test_name, "SP should increase by 2 after RET");
        printf("Actual SP: 0x%04x\n", state.sp);
        return;
    }

    test_passed(test_name);
}

void test_op_call_adr() {
    const char* test_name = "0xCD CALL adr";
    State8080 state;
    initialize_emulator(&state);

    state.sp = 0x5000;
    state.pc = 0x300;
    state.memory[state.pc] = 0xcd;
    state.memory[state.pc + 1] = 0x00;
    state.memory[state.pc + 2] = 0x20;

    Emulate8080Op(&state, false);

    if (state.pc != 0x2000) {
        test_failed(test_name, "PC should jump to 0x2000");
        printf("Actual PC: 0x%04x\n", state.pc);
        return;
    }

    if (state.memory[state.sp] != 0x03 || state.memory[state.sp + 1] != 0x03) {
        test_failed(test_name, "Return address should be pushed onto stack");
        printf("Memory at SP: 0x%02x 0x%02x\n", state.memory[state.sp], state.memory[state.sp + 1]);
        return;
    }

    if (state.sp != 0x4ffe) {
        test_failed(test_name, "SP should decrease by 2 after CALL");
        printf("Actual SP: 0x%04x\n", state.sp);
        return;
    }

    test_passed(test_name);
}

void test_op_pop_d() {
    const char* test_name = "0xD1 POP D";
    State8080 state;
    initialize_emulator(&state);

    state.sp = 0x4000;
    state.memory[state.sp] = 0x34;
    state.memory[state.sp + 1] = 0x12;
    state.memory[0x300] = 0xd1;
    state.pc = 0x300;

    Emulate8080Op(&state, false);

    if (state.e != 0x34 || state.d != 0x12) {
        test_failed(test_name, "d and e should load values from stack");
        printf("d: 0x%02x e: 0x%02x\n", state.d, state.e);
        return;
    }

    if (state.sp != 0x4002) {
        test_failed(test_name, "sp should increase by 2 after pop");
        printf("Actual sp: 0x%04x\n", state.sp);
        return;
    }

    test_passed(test_name);
}

void test_op_out() {
    const char* test_name = "0xD3 OUT d8";
    State8080 state;
    initialize_emulator(&state);

    state.pc = 0x500;
    state.memory[state.pc] = 0xd3;
    state.memory[state.pc + 1] = 0x42;
    state.a = 0xab;

    Emulate8080Op(&state, false);

    if (state.pc != 0x502) {
        test_failed(test_name, "pc should move forward by 2 after out");
        printf("Actual pc: 0x%04x\n", state.pc);
        return;
    }

    // no real "out" happens unless you emulate hardware ports, so mainly check pc
    test_passed(test_name);
}

void test_op_push_d() {
    const char* test_name = "0xD5 PUSH D";
    State8080 state;
    initialize_emulator(&state);

    state.sp = 0x3000;
    state.d = 0x12;
    state.e = 0x34;
    state.pc = 0x400;
    state.memory[state.pc] = 0xd5;

    Emulate8080Op(&state, false);

    if (state.memory[state.sp] != 0x34 || state.memory[state.sp + 1] != 0x12) {
        test_failed(test_name, "stack should contain e then d after push");
        printf("memory[sp]: 0x%02x memory[sp+1]: 0x%02x\n", state.memory[state.sp], state.memory[state.sp + 1]);
        return;
    }

    if (state.sp != 0x2ffe) {
        test_failed(test_name, "sp should decrease by 2 after push");
        printf("Actual sp: 0x%04x\n", state.sp);
        return;
    }

    test_passed(test_name);
}

void test_op_pop_h() {
    const char* test_name = "0xE1 POP H";
    State8080 state;
    initialize_emulator(&state);

    state.sp = 0x5000;
    state.memory[state.sp] = 0x56;
    state.memory[state.sp + 1] = 0x78;
    state.memory[0x600] = 0xe1;
    state.pc = 0x600;

    Emulate8080Op(&state, false);

    if (state.l != 0x56 || state.h != 0x78) {
        test_failed(test_name, "h and l should load values from stack");
        printf("h: 0x%02x l: 0x%02x\n", state.h, state.l);
        return;
    }

    if (state.sp != 0x5002) {
        test_failed(test_name, "sp should increase by 2 after pop");
        printf("Actual sp: 0x%04x\n", state.sp);
        return;
    }

    test_passed(test_name);
}

void test_op_push_h() {
    const char* test_name = "0xE5 PUSH H";
    State8080 state;
    initialize_emulator(&state);

    state.sp = 0x6000;
    state.h = 0x9a;
    state.l = 0xbc;
    state.pc = 0x700;
    state.memory[state.pc] = 0xe5;

    Emulate8080Op(&state, false);

    if (state.memory[state.sp] != 0xbc || state.memory[state.sp + 1] != 0x9a) {
        test_failed(test_name, "stack should contain l then h after push");
        printf("memory[sp]: 0x%02x memory[sp+1]: 0x%02x\n", state.memory[state.sp], state.memory[state.sp + 1]);
        return;
    }

    if (state.sp != 0x5ffe) {
        test_failed(test_name, "sp should decrease by 2 after push");
        printf("Actual sp: 0x%04x\n", state.sp);
        return;
    }

    test_passed(test_name);
}

void test_op_ani() {
    const char* test_name = "0xE6 ANI d8";
    State8080 state;
    initialize_emulator(&state);

    state.pc = 0x800;
    state.a = 0xf0;
    state.memory[state.pc] = 0xe6;
    state.memory[state.pc + 1] = 0x0f;

    Emulate8080Op(&state, false);

    if (state.a != 0x00) {
        test_failed(test_name, "a should be 0x00 after ani");
        printf("Actual a: 0x%02x\n", state.a);
        return;
    }

    if (state.flags.z != 1) {
        test_failed(test_name, "zero flag should be set after ani resulting 0");
        return;
    }

    if (state.pc != 0x802) {
        test_failed(test_name, "pc should move forward by 2 after ani");
        printf("Actual pc: 0x%04x\n", state.pc);
        return;
    }

    test_passed(test_name);
}

void test_op_xchg() {
    const char* test_name = "0xEB XCHG";
    State8080 state;
    initialize_emulator(&state);

    state.h = 0x12;
    state.l = 0x34;
    state.d = 0x56;
    state.e = 0x78;
    state.pc = 0x100;
    state.memory[state.pc] = 0xeb;

    Emulate8080Op(&state, false);

    if (state.h != 0x56 || state.l != 0x78 || state.d != 0x12 || state.e != 0x34) {
        test_failed(test_name, "h/l and d/e should be swapped");
        printf("h: 0x%02x l: 0x%02x d: 0x%02x e: 0x%02x\n", state.h, state.l, state.d, state.e);
        return;
    }

    test_passed(test_name);
}

void test_op_pop_psw() {
    const char* test_name = "0xF1 POP PSW";
    State8080 state;
    initialize_emulator(&state);

    state.sp = 0x4000;
    state.memory[state.sp] = 0x02; // fake flags
    state.memory[state.sp + 1] = 0xab; // accumulator value
    state.pc = 0x200;
    state.memory[state.pc] = 0xf1;

    Emulate8080Op(&state, false);

    if (state.a != 0xab) {
        test_failed(test_name, "accumulator should be loaded from stack");
        printf("a: 0x%02x\n", state.a);
        return;
    }

    if (state.flags.z != (0x02 >> 6 & 1)) {
        test_failed(test_name, "flags should be set from stack");
        return;
    }

    if (state.sp != 0x4002) {
        test_failed(test_name, "sp should increase by 2 after pop psw");
        printf("Actual sp: 0x%04x\n", state.sp);
        return;
    }

    test_passed(test_name);
}

void test_op_push_psw() {
    const char* test_name = "0xF5 PUSH PSW";
    State8080 state;
    initialize_emulator(&state);

    state.sp = 0x5000;
    state.a = 0xcd;
    state.flags.z = 1;
    state.pc = 0x300;
    state.memory[state.pc] = 0xf5;

    Emulate8080Op(&state, false);

    uint8_t flags_packed = (state.flags.s << 7) | (state.flags.z << 6) | (state.flags.ac << 4) |
                            (state.flags.p << 2) | (state.flags.c) | 0x02;

    if (state.memory[state.sp] != flags_packed || state.memory[state.sp + 1] != 0xcd) {
        test_failed(test_name, "stack should contain flags and accumulator after push psw");
        printf("memory[sp]: 0x%02x memory[sp+1]: 0x%02x\n", state.memory[state.sp], state.memory[state.sp + 1]);
        return;
    }

    if (state.sp != 0x4ffe) {
        test_failed(test_name, "sp should decrease by 2 after push psw");
        printf("Actual sp: 0x%04x\n", state.sp);
        return;
    }

    test_passed(test_name);
}

void test_op_ei() {
    const char* test_name = "0xFB EI";
    State8080 state;
    initialize_emulator(&state);

    state.pc = 0x600;
    state.memory[state.pc] = 0xfb;

    Emulate8080Op(&state, false);

    if (state.interrupt_enabled != 1) {
        test_failed(test_name, "interrupt_enable should be set to 1 after ei");
        printf("interrupt_enable: %d\n", state.interrupt_enabled);
        return;
    }

    test_passed(test_name);
}

void test_op_cpi_d8() {
    const char* test_name = "0xFE CPI d8";
    State8080 state;
    initialize_emulator(&state);

    state.pc = 0x700;
    state.a = 0x20;
    state.memory[state.pc] = 0xfe;
    state.memory[state.pc + 1] = 0x20;

    Emulate8080Op(&state, false);

    if (state.flags.z != 1) {
        test_failed(test_name, "zero flag should be set when a == immediate");
        return;
    }

    if (state.pc != 0x702) {
        test_failed(test_name, "pc should move forward by 2 after cpi");
        printf("Actual pc: 0x%04x\n", state.pc);
        return;
    }

    test_passed(test_name);
}


// --- Main Test Runner ---

int main() {
    printf("Running Manual Emulator Tests...\n");
    int failed_count = 0;

    // Call your test functions
    test_op_nop();
    test_op_mvi_b();
    test_op_lxi_b();
    test_op_dcr_b();
    test_op_dad_b();
    test_op_dcr_c();
    test_op_mvi_c();
    test_op_rrc();
    test_op_lxi_d();
    test_op_inx_d();
    test_op_dad_d();
    test_op_ldax_d();
    test_op_lxi_h();
    test_op_inx_h();
    test_op_mvi_h();
    test_op_dad_h();
    test_op_lxi_sp();
    test_op_sta();
    test_op_mvi_m();
    test_op_mov_d();
    test_op_mov_e();
    test_op_mov_h();
    test_op_mov_l();
    test_op_mov_m();
    test_op_mov_a_d();
    test_op_mov_a_e();
    test_op_mov_a_h();
    test_op_mov_a_m();
    test_op_ana_b();
    test_op_xra_b();
    test_op_pop_b();
    test_op_jnz_adr();
    test_op_jmp_adr();
    test_op_push_b();
    test_op_adi_d8();
    test_op_ret();
    test_op_call_adr();
    test_op_pop_d();
    test_op_out();
    test_op_push_d();
    test_op_pop_h();
    test_op_push_h();
    test_op_ani();
    test_op_xchg();
    test_op_pop_psw();
    test_op_push_psw();
    test_op_ei();
    test_op_cpi_d8();
    
    printf("Manual Tests Complete.\n");

    return 0;
}
