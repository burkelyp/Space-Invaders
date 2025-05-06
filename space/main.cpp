#include "emulator.h"
#include "sdl_display.h"
#include "input.h"
#include <SDL2/SDL.h>
#include <cstdint>
#include <cstdio>
#include <cstdlib>

void load_rom_chunk(const char* filename, uint8_t* memory, size_t offset) {
    FILE* file = fopen(filename, "rb");
    if (!file) {
        fprintf(stderr, "Failed to open ROM file: %s\n", filename);
        exit(1);
    }
    fread(&memory[offset], 1, 0x0800, file);
    fclose(file);
}

int main(int argc, char** argv) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s rom_folder\n", argv[0]);
        return EXIT_FAILURE;
    }

    uint8_t memory[0x10000] = {};

    // Load ROM segments
    char filepath[256];
    snprintf(filepath, sizeof(filepath), "%s/invaders.h", argv[1]);
    load_rom_chunk(filepath, memory, 0x0000);
    snprintf(filepath, sizeof(filepath), "%s/invaders.g", argv[1]);
    load_rom_chunk(filepath, memory, 0x0800);
    snprintf(filepath, sizeof(filepath), "%s/invaders.f", argv[1]);
    load_rom_chunk(filepath, memory, 0x1000);
    snprintf(filepath, sizeof(filepath), "%s/invaders.e", argv[1]);
    load_rom_chunk(filepath, memory, 0x1800);

    // Initialize emulator state
    State8080 state = {};
    state.memory = memory;
    state.sp = 0x2400;
    state.pc = 0;
    state.int_enable = 1;

    init_display();
    init_input();

    uint32_t last_interrupt_time = SDL_GetTicks();
    uint32_t last_draw_time = SDL_GetTicks();
    int which_interrupt = 1;
    bool running = true;

    while (running) {
        // Handle SDL events
        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT ||
                (event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_ESCAPE)) {
                running = false;
            }
        }

        update_input_ports();

        // Run instructions between interrupts
        for (int i = 0; i < 500; ++i) {
            Emulate8080Op(&state);
        }

        // Fire interrupts ~ every 8ms
        uint32_t now = SDL_GetTicks();
        if (now - last_interrupt_time >= 8) {
            if (state.int_enable) {
                generate_interrupt(&state, which_interrupt ? 2 : 1);
                which_interrupt ^= 1;
            }
            last_interrupt_time = now;
        }

        // Update screen at ~60 FPS (every ~16ms)
        if (now - last_draw_time >= 16) {
            draw_screen(state.memory);
            last_draw_time = now;
        }

        SDL_Delay(1);
    }

    SDL_Quit();
    return EXIT_SUCCESS;
}
