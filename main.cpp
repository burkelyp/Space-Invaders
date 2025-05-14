#include <iostream>
#include <cstdint>
#include <cstdlib>
#include <cstring>
#include <SDL2/SDL.h>
#include "initcpu.h"
#include "loadrom.h"
#include "emulator.h"

#define VIDEO_MEMORY_START 0x2400
#define SCREEN_WIDTH 224
#define SCREEN_HEIGHT 256
#define CYCLES_PER_INTERRUPT 16667

const char* rom_h_path = "../../rom/space-invaders/invaders.h";
const char* rom_g_path = "../../rom/space-invaders/invaders.g";
const char* rom_f_path = "../../rom/space-invaders/invaders.f";
const char* rom_e_path = "../../rom/space-invaders/invaders.e";


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
                SDL_RenderDrawPoint(renderer, col, SCREEN_HEIGHT - 1 - (row + bit));
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
    //if (argc < 2) {
        //std::cerr << "Usage: " << argv[0] << " <filename>\n";
        //return 1;
    //}



    State8080 state;
    initCPU(&state);
    loadROM(rom_h_path, &state, 0x0000);
    loadROM(rom_g_path, &state, 0x0800);
    loadROM(rom_f_path, &state, 0x1000);
    loadROM(rom_e_path, &state, 0x1800);
    *state.ports.port1 = 0b00001001;

    SDL_Init(SDL_INIT_VIDEO);
    SDL_Window* window = SDL_CreateWindow("Space Invaders",
        SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
        SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, 0);

    bool running = true;
    SDL_Event event;
    uint64_t cycles_for_interrupt_timing = 0;
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
            }
            else if (event.type == SDL_KEYDOWN) {
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
                case SDLK_c:
                    *state.ports.port1 &= ~0x01;
                    break;
                case SDLK_1:
                    *state.ports.port1 |= 0x04;
                    break;
                case SDLK_SPACE:
                    *state.ports.port1 |= 0x10;
                    break;
                case SDLK_LEFT:
                    *state.ports.port1 |= 0x20;
                    break;
                case SDLK_RIGHT:
                    *state.ports.port1 |= 0x40;
                    break;
                default:
                    break;
                }
            }
            else if (event.type == SDL_KEYUP) {
                switch (event.key.keysym.sym) {
                case SDLK_c:
                    *state.ports.port1 |= 0x01;
                    break;
                case SDLK_1:
                    *state.ports.port1 &= ~0x04;
                    break;
                case SDLK_SPACE:
                    *state.ports.port1 &= ~0x10;
                    break;
                case SDLK_LEFT:
                    *state.ports.port1 &= ~0x20;
                    break;
                case SDLK_RIGHT:
                    *state.ports.port1 &= ~0x40;
                    break;
                    // case SDLK_2:
                    //    state.ports.port1 &=  ;
                    //    break;
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

        uint64_t cycles_before_op = state.cycles;
        Emulate8080Op(&state);
        uint64_t op_cycles = state.cycles - cycles_before_op;
        cycles_for_interrupt_timing += op_cycles;

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

        if (cycles_for_interrupt_timing >= CYCLES_PER_INTERRUPT) {
            cycles_for_interrupt_timing -= CYCLES_PER_INTERRUPT;

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

            if (state.interrupt_enabled) {
                state.interrupt_enabled = false;
                state.memory[state.sp - 1] = (state.pc >> 8) & 0xff;
                state.memory[state.sp - 2] = state.pc & 0xff;
                state.sp -= 2;

                if (interrupt_num == 1) {
                    state.pc = 0x0008;
                }
                else {
                    state.pc = 0x0010;
                }
                state.cycles += 11;
                interrupt_num ^= 1;
            }

            DrawScreen(&state, renderer);
        }

        //SDL_Delay(1);
    }

    std::free(state.memory);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    return 0;
}