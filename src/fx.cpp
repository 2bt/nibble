#include <SDL.h>
#include "fx.hpp"
#include "app/app.hpp"


namespace fx {

uint8_t pixels[SCREEN_W * SCREEN_H];
uint8_t button_bits;

namespace {

uint32_t const PALETTE[16] = {
    0x000000,
    0x1D2B53,
    0x7E2553,
    0x008751,
    0xAB5236,
    0x5F574F,
    0xC2C3C7,
    0xFFF1E8,
    0xFF004D,
    0xFFA300,
    0xFFEC27,
    0x00E436,
    0x29ADFF,
    0x83769C,
    0xFF77A8,
    0xFFCCAA,
};

uint32_t pixels32[SCREEN_W * SCREEN_H];


int main() {
    SDL_Init(SDL_INIT_VIDEO);
    SDL_Window* window = SDL_CreateWindow("Nibble",
                                          SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
                                          SCREEN_W * 3, SCREEN_H * 3, SDL_WINDOW_RESIZABLE);

    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_PRESENTVSYNC);
    SDL_RenderSetLogicalSize(renderer, SCREEN_W, SCREEN_H);
    SDL_Texture* texture = SDL_CreateTexture(renderer,
                                             SDL_PIXELFORMAT_ARGB8888,
                                             SDL_TEXTUREACCESS_STREAMING,
                                             SCREEN_W, SCREEN_H);

    app::init();

    bool running = true;
    while (running) {
        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            switch (event.type) {
            default: break;
            case SDL_QUIT:
                running = false;
                break;
            case SDL_KEYDOWN:
                if (event.key.keysym.scancode == SDL_SCANCODE_ESCAPE) running = false;
                break;
            }
        }
        const Uint8* ks = SDL_GetKeyboardState(nullptr);

        button_bits = 0;
        button_bits |= !!ks[SDL_SCANCODE_LEFT  ] << BTN_LEFT;
        button_bits |= !!ks[SDL_SCANCODE_RIGHT ] << BTN_RIGHT;
        button_bits |= !!ks[SDL_SCANCODE_UP    ] << BTN_UP;
        button_bits |= !!ks[SDL_SCANCODE_DOWN  ] << BTN_DOWN;
        button_bits |= !!ks[SDL_SCANCODE_Z     ] << BTN_A;
        button_bits |= !!ks[SDL_SCANCODE_C     ] << BTN_A;
        button_bits |= !!ks[SDL_SCANCODE_X     ] << BTN_B;
        button_bits |= !!ks[SDL_SCANCODE_RETURN] << BTN_C;

        app::update();

        for (int i = 0; i < SCREEN_W * SCREEN_H; ++i) pixels32[i] = PALETTE[pixels[i]];

        SDL_UpdateTexture(texture, nullptr, pixels32, SCREEN_W * sizeof(uint32_t));
        SDL_RenderClear(renderer);
        SDL_RenderCopy(renderer, texture, nullptr, nullptr);
        SDL_RenderPresent(renderer);
    }

    SDL_DestroyTexture(texture);
    SDL_Quit();
    return 0;
}

} // namespace

} // namespace fx

int main() { return fx::main(); }
