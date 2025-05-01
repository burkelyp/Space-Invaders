#include <cstdint> // Needed for uint8_t, uint16_t

// Put the State8080 struct definition here...
struct State8080 {
    // 8-bit registers
    uint8_t a = 0;
    uint8_t b = 0;
    uint8_t c = 0;
    uint8_t d = 0;
    uint8_t e = 0;
    uint8_t h = 0;
    uint8_t l = 0;

    // Stack Pointer and Program Counter
    uint16_t sp = 0;
    uint16_t pc = 0;
    uint32_t cycles = 0;

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

    // Halt capability
    uint8_t halted = false;

    // Memory (64KB)
    uint8_t memory[0x10000]; // 65536 bytes
};

// Function Declarations
void setZSPflags(State8080* cpu, uint8_t result);
uint8_t input_port(uint8_t a, uint8_t port); // Or maybe void input_port(State8080* cpu, uint8_t port); ?
void output_port(uint8_t port, uint8_t a);  // Or maybe void output_port(State8080* cpu, uint8_t port); ?
void Emulate8080Op(State8080* cpu, bool debug = false);
void initialize_emulator(State8080* state);