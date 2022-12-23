#pragma once

#include <cstdint>
#include <random>
#include <string>

namespace chip8
{
constexpr int SCREEN_WIDTH = 64;
constexpr int SCREEN_HEIGHT = 32;
constexpr int DISPLAY_SIZE = SCREEN_WIDTH * SCREEN_HEIGHT; // 2048 pixels
constexpr int FONTSET_SIZE = 80;
constexpr uint8_t chip8Fontset[FONTSET_SIZE] = {
        // Every charater is 4 pixels wide and 5 pixels high
        0xF0, 0x90, 0x90, 0x90, 0xF0, // 0
        0x20, 0x60, 0x20, 0x20, 0x70, // 1
        0xF0, 0x10, 0xF0, 0x80, 0xF0, // 2
        0xF0, 0x10, 0xF0, 0x10, 0xF0, // 3
        0x90, 0x90, 0xF0, 0x10, 0x10, // 4
        0xF0, 0x80, 0xF0, 0x10, 0xF0, // 5
        0xF0, 0x80, 0xF0, 0x90, 0xF0, // 6
        0xF0, 0x10, 0x20, 0x40, 0x40, // 7
        0xF0, 0x90, 0xF0, 0x90, 0xF0, // 8
        0xF0, 0x90, 0xF0, 0x10, 0xF0, // 9
        0xF0, 0x90, 0xF0, 0x90, 0x90, // A
        0xE0, 0x90, 0xE0, 0x90, 0xE0, // B
        0xF0, 0x80, 0x80, 0x80, 0xF0, // C
        0xE0, 0x90, 0x90, 0x90, 0xE0, // D
        0xF0, 0x80, 0xF0, 0x80, 0xF0, // E
        0xF0, 0x80, 0xF0, 0x80, 0x80 // F
}; // Chip8 fontset

const uint16_t START_ADDRESS = 0x200; // Program counter starts at 0x200
const uint16_t FONTSET_START_ADDRESS = 0x50; // Fontset starts at 0x50

class Chip8
{
private:
    uint8_t m_memory[4096]{}; // 4K memory
    uint8_t m_registers[16]{}; // V0-VF
    uint16_t m_I = 0; // Index register
    uint16_t m_pc = 0x200; // Program counter
    uint16_t m_opcode = 0; // Current opcode
    uint8_t m_delayTimer = 0; // Delay timer
    uint8_t m_soundTimer = 0; // Sound timer
    uint16_t m_stack[16]{}; // Stack
    uint16_t m_sp = 0; // Stack pointer

    std::default_random_engine generator;

public:
    uint32_t m_display[DISPLAY_SIZE]{}; // Graphics array (64x32)
    uint8_t m_keypad[16]{}; // Keypad

    Chip8();

    void loadGame(char const *filename);
    void cycle();

    void decodeOpcode0(uint16_t opcode);
    void executeOpcode00E0();
    void executeOpcode00EE();
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
} // namespace chip8
