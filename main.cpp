#include <iostream>
#include <cstdint>
#include <cstdlib>
#include <cstring>
#include <SDL3/SDL.h>
//#include "initcpu.h"

#include "loadrom.h"
#include "emulator.h"

#define VIDEO_MEMORY_START 0x2400
#define SCREEN_WIDTH 224
#define SCREEN_HEIGHT 256
#define CYCLES_PER_INTERRUPT 8333




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
                SDL_RenderPoint(renderer, SCREEN_WIDTH - col, row + bit);

            }
        }
    }
    SDL_RenderPresent(renderer);
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



    State8080 state;
    if (init_mmap(&state)) {
        initCPU(&state);
    }

    loadROM(argv[1], &state);

    SDL_Init(SDL_INIT_VIDEO);
    SDL_Window* window = SDL_CreateWindow("Space Invaders",
        SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_RESIZABLE);
    SDL_Renderer* renderer = SDL_CreateRenderer(window, NULL);


    bool running = true;
    SDL_Event event;
    uint64_t cycles_for_interrupt_timing = 0;
    int interrupt_num = 1;

    bool paused = false;
    bool debug_mode = true;

    bool log_cycles = true;
    bool single_step = false;

    uint32_t last_interrupt_time = SDL_GetTicks();
    uint32_t last_frame_time = SDL_GetTicks();


    while (running) {
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_EVENT_QUIT) {
                running = false;
            }
            else if (event.type == SDL_EVENT_KEY_DOWN) {
                switch (event.key.key) {
                case SDLK_P:
                    paused = !paused;
                    std::cout << (paused ? "Paused\n" : "Resumed\n");
                    break;
                case SDLK_D:
                    debug_mode = !debug_mode;
                    std::cout << (debug_mode ? "Debug mode ON\n" : "Debug mode OFF\n");
                    break;
                case SDLK_L:
                    log_cycles = !log_cycles;
                    std::cout << (log_cycles ? "Logging ON\n" : "Logging OFF\n");
                    break;
                case SDLK_N:

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

        uint64_t cycles_before_op = state.cycles;
        Emulate8080Op(&state, debug_mode);
        uint64_t op_cycles = state.cycles - cycles_before_op;
        cycles_for_interrupt_timing += op_cycles;

        if (!debug_mode) {

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