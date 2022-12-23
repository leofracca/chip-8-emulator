#pragma once

#include <SDL2/SDL.h>
#include <cstdint>

namespace chip8
{
class Platform
{
public:
    Platform(char const *title, int windowWidth, int windowHeight, int textureWidth, int textureHeight);
    ~Platform();
    void Update(void const *buffer, int pitch);
    static bool ProcessInput(uint8_t *keys);

private:
    SDL_Window *window{};
    SDL_Renderer *renderer{};
    SDL_Texture *texture{};
};
} // namespace chip8
