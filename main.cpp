#include <iostream>
#include <cstdint>
#include <cstdlib>
#include <cstring>
#include <SDL2/SDL.h>

#define VIDEO_MEMORY_START 0x2400
#define SCREEN_WIDTH 224
#define SCREEN_HEIGHT 256
#define CYCLES_PER_INTERRUPT 8333

/**
   CPU state structure for Intel 8080
*/
struct ConditionCodes {
    uint8_t z : 1;
    uint8_t s : 1;
    uint8_t p : 1;
    uint8_t cy : 1;
    uint8_t ac : 1;
    uint8_t pad : 3;
};

struct State8080 {
    uint8_t a, b, c, d, e, h, l;
    uint16_t sp;
    uint16_t pc;
    uint8_t* memory;
    ConditionCodes cc;
    uint8_t int_enable;
};

/**
   Placeholder for CPU emulation step (to be implemented)

   @param state - pointer to 8080 state
   @return number of cycles used by instruction
*/
int Emulate8080Op(State8080* state) {
    // Placeholder - implement opcode execution logic
    return 4; // assume each instruction takes 4 cycles
}

/**
   Render video memory to SDL window

   @param state - 8080 CPU state
   @param renderer - SDL renderer
*/
void DrawScreen(State8080* state, SDL_Renderer* renderer) {
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);

    for (int addr = VIDEO_MEMORY_START; addr <= 0x3FFF; addr++) {
        uint8_t byte = state->memory[addr];
        int offset = addr - VIDEO_MEMORY_START;
        int col = offset / 32;
        int row = (offset % 32) * 8;

        for (int bit = 0; bit < 8; bit++) {
            if ((byte >> bit) & 1) {
                SDL_RenderDrawPoint(renderer, SCREEN_WIDTH - col, row + bit);
            }
        }
    }
    SDL_RenderPresent(renderer);
}

/**
   Disassembles a single 8080 instruction from the code buffer and prints its assembly equivalent

   @param codebuffer - pointer to the start of the 8080 program code
   @param pc - current program counter (offset into codebuffer)
   @return number of bytes the current instruction occupies
*/
int Disassemble8080Op(uint8_t* codebuffer, int pc) {
    uint8_t* code = &codebuffer[pc];
    int opbytes = 1;
    std::printf("%04x ", pc);
    switch (*code) {
        case 0x00: std::printf("NOP"); break;
        case 0x01: std::printf("LXI    B,#$%02x%02x", code[2], code[1]); opbytes = 3; break;
        case 0x02: std::printf("STAX   B"); break;
        case 0x03: std::printf("INX    B"); break;
        case 0x04: std::printf("INR    B"); break;
        case 0x05: std::printf("DCR    B"); break;
        case 0x06: std::printf("MVI    B,#$%02x", code[1]); opbytes = 2; break;
        case 0x07: std::printf("RLC"); break;
        default: std::printf("UNKNOWN %02x", *code); break;
    }
    std::printf("\n");
    return opbytes;
}

/**
   Entry point for emulator. Loads ROM and executes main loop

   @param argc - argument count
   @param argv - argument vector (expects the ROM file path as argv[1])
   @return 0 on success, 1 on failure
*/
int main(int argc, char** argv) {
    if (argc < 2) {
        std::cerr << "Usage: " << argv[0] << " <filename>\n";
        return 1;
    }

    FILE* f = std::fopen(argv[1], "rb");
    if (!f) {
        std::cerr << "Cannot open file: " << argv[1] << "\n";
        return 1;
    }

    std::fseek(f, 0, SEEK_END);
    int fsize = std::ftell(f);
    std::fseek(f, 0, SEEK_SET);

    State8080 state{};
    state.memory = static_cast<uint8_t*>(std::malloc(0x10000)); // 64KB
    std::memset(state.memory, 0, 0x10000);
    std::fread(state.memory, fsize, 1, f);
    std::fclose(f);

    SDL_Init(SDL_INIT_VIDEO);
    SDL_Window* window = SDL_CreateWindow("Space Invaders",
        SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
        SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, 0);

    bool running = true;
    SDL_Event event;
    uint64_t cycles = 0;
    int interrupt_num = 1;

    bool paused = false;
    bool debug_mode = false;
    bool log_cycles = true;
    bool single_step = false;

    uint32_t last_interrupt_time = SDL_GetTicks();
    uint32_t last_frame_time = SDL_GetTicks();

    while (running) {
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                running = false;
            } else if (event.type == SDL_KEYDOWN) {
                switch (event.key.keysym.sym) {
                    case SDLK_p:
                        paused = !paused;
                        std::cout << (paused ? "Paused\n" : "Resumed\n");
                        break;
                    case SDLK_d:
                        debug_mode = !debug_mode;
                        std::cout << (debug_mode ? "Debug mode ON\n" : "Debug mode OFF\n");
                        break;
                    case SDLK_l:
                        log_cycles = !log_cycles;
                        std::cout << (log_cycles ? "Logging ON\n" : "Logging OFF\n");
                        break;
                    case SDLK_n:
                        if (paused) {
                            single_step = true;
                            std::cout << "Single step requested\n";
                        }
                        break;
                    default:
                        break;
                }
            }
        }

        if (paused && !single_step) {
            SDL_Delay(1);
            continue;
        }

        if (paused && single_step) {
            single_step = false;
        }

        cycles += Emulate8080Op(&state);

        if (debug_mode) {
            std::cout << std::hex;
            std::cout << "[DEBUG] PC: " << state.pc
                      << " SP: " << state.sp
                      << " A: " << (int)state.a
                      << " B: " << (int)state.b
                      << " C: " << (int)state.c
                      << " D: " << (int)state.d
                      << " E: " << (int)state.e
                      << " H: " << (int)state.h
                      << " L: " << (int)state.l
                      << std::dec << "\n";
        }

        if (cycles >= CYCLES_PER_INTERRUPT) {
            cycles = 0;

            uint32_t now = SDL_GetTicks();
            uint32_t elapsed_interrupt = now - last_interrupt_time;
            last_interrupt_time = now;

            if (log_cycles) {
                std::cout << "Time between interrupts: " << elapsed_interrupt << " ms\n";
            }

            if (interrupt_num == 0) {
                uint32_t elapsed_frame = now - last_frame_time;
                last_frame_time = now;

                if (log_cycles) {
                    std::cout << "Time for one full frame: " << elapsed_frame << " ms\n";
                }
            }

            if (state.int_enable) {
                uint8_t rst_opcode = interrupt_num ? 0xd7 : 0xcf;
                interrupt_num ^= 1;
            }

            DrawScreen(&state, renderer);
        }

        SDL_Delay(1);
    }

    std::free(state.memory);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    return 0;
}
