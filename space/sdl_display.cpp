#include "sdl_display.h"
#include <SDL2/SDL.h>
#include <cstdio>

static SDL_Window* window = nullptr;
static SDL_Renderer* renderer = nullptr;
static SDL_Texture* texture = nullptr;

const int VIDEO_WIDTH = 224;
const int VIDEO_HEIGHT = 256;

void init_display() {
    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        fprintf(stderr, "SDL_Init failed: %s\n", SDL_GetError());
        exit(1);
    }

    window = SDL_CreateWindow("Space Invaders Emulator",
                              SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                              VIDEO_WIDTH * 2, VIDEO_HEIGHT * 2,
                              SDL_WINDOW_SHOWN);
    if (!window) {
        fprintf(stderr, "SDL_CreateWindow failed: %s\n", SDL_GetError());
        exit(1);
    }

    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_ARGB8888,
                                SDL_TEXTUREACCESS_STREAMING,
                                VIDEO_WIDTH, VIDEO_HEIGHT);
}

void draw_screen(const uint8_t* memory) {
    // Video memory is from 0x2400 to 0x3FFF (0x1C00 bytes = 7168 bytes)
    const uint8_t* video_mem = &memory[0x2400];
    uint32_t pixels[VIDEO_WIDTH * VIDEO_HEIGHT];

    for (int y = 0; y < VIDEO_HEIGHT; ++y) {
        for (int x = 0; x < VIDEO_WIDTH; ++x) {
            int flipped_y = VIDEO_HEIGHT - 1 - y;  // Flip vertically
            int byte_index = (x * VIDEO_HEIGHT + flipped_y) / 8;
            int bit_index  = flipped_y % 8;
            bool pixel_on = (video_mem[byte_index] >> bit_index) & 1;
            pixels[y * VIDEO_WIDTH + x] = pixel_on ? 0xFFFFFFFF : 0xFF000000;
        }
    }
    

    SDL_UpdateTexture(texture, nullptr, pixels, VIDEO_WIDTH * sizeof(uint32_t));
    SDL_RenderClear(renderer);
    SDL_RenderCopy(renderer, texture, nullptr, nullptr);
    SDL_RenderPresent(renderer);
}

void shutdown_display() {
    if (texture) SDL_DestroyTexture(texture);
    if (renderer) SDL_DestroyRenderer(renderer);
    if (window) SDL_DestroyWindow(window);
    SDL_Quit();
}
