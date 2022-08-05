#pragma once

#include <cstdint>
#include <string>

const int SCREEN_WIDTH = 64;
const int SCREEN_HEIGHT = 32;
const int DISPLAY_SIZE = SCREEN_WIDTH * SCREEN_HEIGHT; // 2048 pixels

class Chip8
{
private:
    uint8_t memory[4096]; // 4K memory
    uint8_t registers[16]; // V0-VF
    uint16_t I; // Index register
    uint16_t pc; // Program counter
    uint16_t opcode; // Current opcode
    uint8_t delayTimer; // Delay timer
    uint8_t soundTimer; // Sound timer
    uint16_t stack[16]; // Stack
    uint16_t sp; // Stack pointer
public:
    uint32_t display[DISPLAY_SIZE]{}; // Graphics array (64x32)
    uint8_t keypad[16]{}; // Keypad

    Chip8();

    void loadGame(char const* filename);
    void cycle();

    void decodeOpcode0(uint16_t opcode);
    void executeOpcode00E0(uint16_t opcode);
    void executeOpcode00EE(uint16_t opcode);
    void executeOpcode1NNN(uint16_t opcode);
    void executeOpcode2NNN(uint16_t opcode);
    void executeOpcode3XNN(uint16_t opcode);
    void executeOpcode4XNN(uint16_t opcode);
    void executeOpcode5XY0(uint16_t opcode);
    void executeOpcode6XNN(uint16_t opcode);
    void executeOpcode7XNN(uint16_t opcode);
    void decodeOpcode8(uint16_t opcode);
    void executeOpcode8XY0(uint16_t opcode);
    void executeOpcode8XY1(uint16_t opcode);
    void executeOpcode8XY2(uint16_t opcode);
    void executeOpcode8XY3(uint16_t opcode);
    void executeOpcode8XY4(uint16_t opcode);
    void executeOpcode8XY5(uint16_t opcode);
    void executeOpcode8XY6(uint16_t opcode);
    void executeOpcode8XY7(uint16_t opcode);
    void executeOpcode8XYE(uint16_t opcode);
    void executeOpcode9XY0(uint16_t opcode);
    void executeOpcodeANNN(uint16_t opcode);
    void executeOpcodeBNNN(uint16_t opcode);
    void executeOpcodeCXNN(uint16_t opcode);
    void executeOpcodeDXYN(uint16_t opcode);
    void decodeOpcodeE(uint16_t opcode);
    void executeOpcodeEX9E(uint16_t opcode);
    void executeOpcodeEXA1(uint16_t opcode);
    void decodeOpcodeF(uint16_t opcode);
    void executeOpcodeFX07(uint16_t opcode);
    void executeOpcodeFX0A(uint16_t opcode);
    void executeOpcodeFX15(uint16_t opcode);
    void executeOpcodeFX18(uint16_t opcode);
    void executeOpcodeFX1E(uint16_t opcode);
    void executeOpcodeFX29(uint16_t opcode);
    void executeOpcodeFX33(uint16_t opcode);
    void executeOpcodeFX55(uint16_t opcode);
    void executeOpcodeFX65(uint16_t opcode);
};
