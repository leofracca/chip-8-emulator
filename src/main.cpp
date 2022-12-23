#include "Chip8.h"
#include "Platform.h"

#include <chrono>
#include <iostream>

int main(int argc, char **argv)
{
    using namespace chip8;

    if (argc != 4)
    {
        std::cerr << "Usage: " << argv[0] << " <Scale> <Delay> <ROM>\n";
        std::exit(EXIT_FAILURE);
    }

    int videoScale = std::stoi(argv[1]);
    int cycleDelay = std::stoi(argv[2]);
    char const *romFilename = argv[3];

    Platform platform("CHIP-8 Emulator", SCREEN_WIDTH * videoScale, SCREEN_HEIGHT * videoScale, SCREEN_WIDTH, SCREEN_HEIGHT);

    Chip8 chip8;
    chip8.loadGame(romFilename);

    int videoPitch = sizeof(chip8.m_display[0]) * SCREEN_WIDTH;

    auto lastCycleTime = std::chrono::high_resolution_clock::now();
    bool quit = false;

    while (!quit)
    {
        quit = Platform::ProcessInput(chip8.m_keypad);

        auto currentTime = std::chrono::high_resolution_clock::now();
        auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(currentTime - lastCycleTime).count();

        if (elapsed > cycleDelay)
        {
            lastCycleTime = currentTime;
            chip8.cycle();
            platform.Update(chip8.m_display, videoPitch);
        }
    }

    return 0;
}
