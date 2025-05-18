#include <iostream>
#include <cstdint>
#include <cstdlib>
#include <cstring>
#include "initcpu.h"
#include "loadrom.h"
#include "emulator.h"
#include "access_mmap.h"
#include "sound.h"

#ifdef PLATFORM_WINDOWS
    #include <SDL.h>
#else
    #include <SDL2/SDL.h>
#endif

#define VIDEO_MEMORY_START 0x2400
#define VIDEO_MEMORY_END 0x3FFF
#define SCREEN_WIDTH 224
#define SCREEN_HEIGHT 256

const char* rom_h_path = "../../rom/space-invaders/invaders.h";
const char* rom_g_path = "../../rom/space-invaders/invaders.g";
const char* rom_f_path = "../../rom/space-invaders/invaders.f";
const char* rom_e_path = "../../rom/space-invaders/invaders.e";

void DrawScreen(State8080* state, SDL_Renderer* renderer) {
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);

    for (int addr = VIDEO_MEMORY_START; addr <= VIDEO_MEMORY_END; addr++) {
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

int main(int argc, char** argv) {
    if (!initSoundSystem()) {
        std::cerr << "Sound system failed to initialize.\n";
    }

    State8080 state;
    if (init_mmap(&state)) {
        initCPU(&state);
    } else {
        std::cerr << "Failed to initialize memory-mapped state.\n";
        return 1;
    }

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
    int interrupt_num = 1;
    int interrupts_per_frame = 0;

    bool paused = false;
    bool debug_mode = false;
    bool log_cycles = true;
    bool single_step = false;

    uint32_t interrupt_timer = SDL_GetTicks();
    uint32_t frame_timer = SDL_GetTicks();

    while (running) {
        uint32_t frame_start = SDL_GetTicks();

        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                running = false;
            } else if (event.type == SDL_KEYDOWN) {
                switch (event.key.keysym.sym) {
                    case SDLK_p: paused = !paused; std::cout << (paused ? "Paused\n" : "Resumed\n"); break;
                    case SDLK_d: debug_mode = !debug_mode; std::cout << (debug_mode ? "Debug mode ON\n" : "Debug mode OFF\n"); break;
                    case SDLK_l: log_cycles = !log_cycles; std::cout << (log_cycles ? "Logging ON\n" : "Logging OFF\n"); break;
                    case SDLK_n: if (paused) { single_step = true; std::cout << "Single step requested\n"; } break;
                    case SDLK_c: *state.ports.port1 &= ~0x01; break;
                    case SDLK_1: *state.ports.port1 |= 0x04; break;
                    case SDLK_SPACE: *state.ports.port1 |= 0x10; break;
                    case SDLK_LEFT: *state.ports.port1 |= 0x20; break;
                    case SDLK_RIGHT: *state.ports.port1 |= 0x40; break;
                    default: break;
                }
            } else if (event.type == SDL_KEYUP) {
                switch (event.key.keysym.sym) {
                    case SDLK_c: *state.ports.port1 |= 0x01; break;
                    case SDLK_1: *state.ports.port1 &= ~0x04; break;
                    case SDLK_SPACE: *state.ports.port1 &= ~0x10; break;
                    case SDLK_LEFT: *state.ports.port1 &= ~0x20; break;
                    case SDLK_RIGHT: *state.ports.port1 &= ~0x40; break;
                    default: break;
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

        for (int interrupt = 0; interrupt < 2; ++interrupt) {
            uint64_t cycles_this_interrupt = state.cycles;
            while ((state.cycles - cycles_this_interrupt) < 16666) {
                Emulate8080Op(&state);
            }

            if (state.interrupt_enabled) {
                state.interrupt_enabled = false;
                state.memory[state.sp - 1] = (state.pc >> 8) & 0xff;
                state.memory[state.sp - 2] = state.pc & 0xff;
                state.sp -= 2;
                state.pc = (interrupt_num == 1) ? 0x0008 : 0x0010;
                state.cycles += 11;
                interrupt_num ^= 1;
            }
        }

        DrawScreen(&state, renderer);

        uint32_t frame_end = SDL_GetTicks();
        uint32_t elapsed = frame_end - frame_start;
        if (elapsed < 16) SDL_Delay(16 - elapsed);

        if (log_cycles) {
            std::cout << "Time for one full frame: " << (SDL_GetTicks() - frame_start) << " ms\n";
        }
    }

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    shutdownSoundSystem();
    return 0;
}
