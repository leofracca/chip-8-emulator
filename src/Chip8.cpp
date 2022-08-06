#include "Chip8.h"

#include <fstream>
#include <iostream>
#include <cstdlib>
#include <ctime>

const unsigned int FONTSET_SIZE = 80;
uint8_t chip8Fontset[FONTSET_SIZE] = { // Every charater is 4 pixels wide and 5 pixels high
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
    0xF0, 0x80, 0xF0, 0x80, 0x80  // F
}; // Chip8 fontset

const unsigned int START_ADDRESS = 0x200; // Program counter starts at 0x200
const unsigned int FONTSET_START_ADDRESS = 0x50; // Fontset starts at 0x50

Chip8::Chip8()
    : I(0),
    pc(START_ADDRESS),
    opcode(0),
    delayTimer(0),
    soundTimer(0),
    sp(0)
{
    // Load the fontset into memory
    for (unsigned int i = 0; i < FONTSET_SIZE; i++)
    {
        memory[FONTSET_START_ADDRESS + i] = chip8Fontset[i];
    }

    // Initialize random seed
    srand(time(NULL));
}

void Chip8::loadGame(char const* filename)
{
    // Open the file
    std::ifstream file(filename, std::ios::binary);
    if (!file.is_open())
    {
        std::cout << "Could not open file: " << filename << std::endl;
        return;
    }

    // Read the entire file into the memory
    file.seekg(0, std::ios::end);
    int size = file.tellg();
    file.seekg(0, std::ios::beg);
    for (int i = 0; i < size; i++)
    {
        memory[START_ADDRESS + i] = file.get();
    }

    file.close();
}

void Chip8::cycle()
{
    // Fetch the opcode
    opcode = memory[pc] << 8 | memory[pc + 1];

    // Increment the program counter
    pc += 2;

    // Debugging purposes
    std::cout << "opcode: " << std::hex << opcode << std::endl;
    std::cout << "pc: " << pc << std::endl;

    // Decode and execute the opcode
    switch (opcode & 0xF000) // First 4 bits
    {
        case 0x0000: decodeOpcode0(opcode); break;
        case 0x1000: executeOpcode1NNN(opcode); break;
        case 0x2000: executeOpcode2NNN(opcode); break;
        case 0x3000: executeOpcode3XNN(opcode); break;
        case 0x4000: executeOpcode4XNN(opcode); break;
        case 0x5000: executeOpcode5XY0(opcode); break;
        case 0x6000: executeOpcode6XNN(opcode); break;
        case 0x7000: executeOpcode7XNN(opcode); break;
        case 0x8000: decodeOpcode8(opcode); break;
        case 0x9000: executeOpcode9XY0(opcode); break;
        case 0xA000: executeOpcodeANNN(opcode); break;
        case 0xB000: executeOpcodeBNNN(opcode); break;
        case 0xC000: executeOpcodeCXNN(opcode); break;
        case 0xD000: executeOpcodeDXYN(opcode); break;
        case 0xE000: decodeOpcodeE(opcode); break;
        case 0xF000: decodeOpcodeF(opcode); break;
        default:
            std::cout << "Unknown opcode: " << std::hex << opcode << std::endl;
            break;
    }

    // Update timers
    if (delayTimer > 0)
        delayTimer--;
    if (soundTimer > 0)
        soundTimer--;
}

void Chip8::decodeOpcode0(uint16_t opcode)
{
    // Checks the last 4 bits of the opcode
    switch (opcode & 0x000F)
    {
        case 0x0000: executeOpcode00E0(opcode); break;
        case 0x000E: executeOpcode00EE(opcode); break;
        default:
            std::cout << "Unknown opcode: " << std::hex << opcode << std::endl;
            break;
    }
}

void Chip8::executeOpcode00E0(uint16_t opcode)
{
    // Clears the screen
    for (int i = 0; i < DISPLAY_SIZE; i++)
        display[i] = 0;
}

void Chip8::executeOpcode00EE(uint16_t opcode)
{
    // Returns from a subroutine
    pc = stack[--sp];
}

void Chip8::executeOpcode1NNN(uint16_t opcode)
{
    // Jumps to address NNN
    pc = opcode & 0x0FFF;
}

void Chip8::executeOpcode2NNN(uint16_t opcode)
{
    // Calls subroutine at NNN
    stack[sp++] = pc;
    pc = opcode & 0x0FFF;
}

void Chip8::executeOpcode3XNN(uint16_t opcode)
{
    // Skips the next instruction if VX equals NN
    uint8_t VX = (opcode & 0x0F00) >> 8;
    uint8_t NN = opcode & 0x00FF;

    if (registers[VX] == NN)
        pc += 2;
}

void Chip8::executeOpcode4XNN(uint16_t opcode)
{
    // Skips the next instruction if VX does not equal NN
    uint8_t VX = (opcode & 0x0F00) >> 8;
    uint8_t NN = opcode & 0x00FF;

    if (registers[VX] != NN)
        pc += 2;
}

void Chip8::executeOpcode5XY0(uint16_t opcode)
{
    // Skips the next instruction if VX equals VY
    uint8_t VX = (opcode & 0x0F00) >> 8;
    uint8_t VY = (opcode & 0x00F0) >> 4;

    if (registers[VX] == registers[VY])
        pc += 2;
}

void Chip8::executeOpcode6XNN(uint16_t opcode)
{
    // Sets VX to NN
    uint8_t VX = (opcode & 0x0F00) >> 8;
    uint8_t NN = opcode & 0x00FF;

    registers[VX] = NN;
}

void Chip8::executeOpcode7XNN(uint16_t opcode)
{
    // Adds NN to VX. (Carry flag is not changed)
    uint8_t VX = (opcode & 0x0F00) >> 8;
    uint8_t NN = opcode & 0x00FF;

    registers[VX] += NN;
}

void Chip8::decodeOpcode8(uint16_t opcode)
{
    // Checks the last 4 bits of the opcode
    switch (opcode & 0x000F)
    {
        case 0x0000: executeOpcode8XY0(opcode); break;
        case 0x0001: executeOpcode8XY1(opcode); break;
        case 0x0002: executeOpcode8XY2(opcode); break;
        case 0x0003: executeOpcode8XY3(opcode); break;
        case 0x0004: executeOpcode8XY4(opcode); break;
        case 0x0005: executeOpcode8XY5(opcode); break;
        case 0x0006: executeOpcode8XY6(opcode); break;
        case 0x0007: executeOpcode8XY7(opcode); break;
        case 0x000E: executeOpcode8XYE(opcode); break;
        default:
            std::cout << "Unknown opcode: " << std::hex << opcode << std::endl;
            break;
    }
}

void Chip8::executeOpcode8XY0(uint16_t opcode)
{
    // Sets VX to the value of VY
    uint8_t VX = (opcode & 0x0F00) >> 8;
    uint8_t VY = (opcode & 0x00F0) >> 4;

    registers[VX] = registers[VY];
}

void Chip8::executeOpcode8XY1(uint16_t opcode)
{
    // Sets VX to VX or VY. (Bitwise OR operation)
    uint8_t VX = (opcode & 0x0F00) >> 8;
    uint8_t VY = (opcode & 0x00F0) >> 4;

    registers[VX] |= registers[VY];
}

void Chip8::executeOpcode8XY2(uint16_t opcode)
{
    // Sets VX to VX and VY. (Bitwise AND operation)
    uint8_t VX = (opcode & 0x0F00) >> 8;
    uint8_t VY = (opcode & 0x00F0) >> 4;

    registers[VX] &= registers[VY];
}

void Chip8::executeOpcode8XY3(uint16_t opcode)
{
    // Sets VX to VX xor VY
    uint8_t VX = (opcode & 0x0F00) >> 8;
    uint8_t VY = (opcode & 0x00F0) >> 4;

    registers[VX] ^= registers[VY];
}

void Chip8::executeOpcode8XY4(uint16_t opcode)
{
    // Adds VY to VX. VF is set to 1 when there's a carry, and to 0 when there is not
    uint8_t VX = (opcode & 0x0F00) >> 8;
    uint8_t VY = (opcode & 0x00F0) >> 4;

    uint16_t sum = registers[VX] + registers[VY];

    if (sum > 255)
        registers[0xF] = 1;
    else
        registers[0xF] = 0;

    registers[VX] = sum & 0xFF;
}

void Chip8::executeOpcode8XY5(uint16_t opcode)
{
    // VY is subtracted from VX. VF is set to 0 when there's a borrow, and 1 when there is not
    uint8_t VX = (opcode & 0x0F00) >> 8;
    uint8_t VY = (opcode & 0x00F0) >> 4;

    if (registers[VX] > registers[VY])
        registers[0xF] = 1;
    else
        registers[0xF] = 0;

    registers[VX] -= registers[VY];
}

void Chip8::executeOpcode8XY6(uint16_t opcode)
{
    // Stores the least significant bit of VX in VF and then shifts VX to the right by 1
    uint8_t VX = (opcode & 0x0F00) >> 8;

    registers[0xF] = registers[VX] & 0x1;
    registers[VX] >>= 1;
}

void Chip8::executeOpcode8XY7(uint16_t opcode)
{
    // Sets VX to VY minus VX. VF is set to 0 when there's a borrow, and 1 when there is not
    uint8_t VX = (opcode & 0x0F00) >> 8;
    uint8_t VY = (opcode & 0x00F0) >> 4;

    if (registers[VY] > registers[VX])
        registers[0xF] = 1;
    else
        registers[0xF] = 0;

    registers[VX] = registers[VY] - registers[VX];
}

void Chip8::executeOpcode8XYE(uint16_t opcode)
{
    // Stores the most significant bit of VX in VF and then shifts VX to the left by 1
    uint8_t VX = (opcode & 0x0F00) >> 8;

    registers[0xF] = (registers[VX] >> 7) & 0x1;
    registers[VX] <<= 1;
}

void Chip8::executeOpcode9XY0(uint16_t opcode)
{
    // 	Skips the next instruction if VX does not equal VY.
    uint8_t VX = (opcode & 0x0F00) >> 8;
    uint8_t VY = (opcode & 0x00F0) >> 4;

    if (registers[VX] != registers[VY])
        pc += 2;
}

void Chip8::executeOpcodeANNN(uint16_t opcode)
{
    // Sets I to the address NNN
    I = opcode & 0x0FFF;
}

void Chip8::executeOpcodeBNNN(uint16_t opcode)
{
    // Jumps to the address NNN plus V0
    uint16_t NNN = opcode & 0x0FFF;

    pc = NNN + registers[0];
}

void Chip8::executeOpcodeCXNN(uint16_t opcode)
{
    // Sets VX to the result of a bitwise and operation on a random number (Typically: 0 to 255) and NN
    uint8_t VX = (opcode & 0x0F00) >> 8;
    uint8_t NN = (opcode & 0x00FF);

    registers[VX] = (rand() % 256) & NN;
}

void Chip8::executeOpcodeDXYN(uint16_t opcode)
{
    // Draws a sprite at coordinate (VX, VY) that has a width of 8 pixels and a height of N pixels.
    // Each row of 8 pixels is read as bit-coded starting from memory location I;
    // I value does not change after the execution of this instruction.
    // As described above, VF is set to 1 if any screen pixels are flipped from set to unset when the sprite is drawn, and to 0 if that does not happen
    uint8_t VX = (opcode & 0x0F00) >> 8;
    uint8_t VY = (opcode & 0x00F0) >> 4;
    uint8_t N = (opcode & 0x000F);

    uint8_t x = registers[VX];
    uint8_t y = registers[VY];

    uint8_t pixel;

    for (uint8_t row = 0; row < N; row++)
    {
        pixel = memory[I + row];

        for (uint8_t col = 0; col < 8; col++)
        {
            // If the pixel is set, draw it to the screen
            if ((pixel & (0x80 >> col)) != 0)
            {
                uint32_t* pixel_ptr = (uint32_t*)&display[(y + row) * SCREEN_WIDTH + (x + col)];
                // If the pixel on the screen is already set, set VF to 1, else set it to 0
                if (*pixel_ptr == 0xFFFFFFFF)
                    registers[0xF] = 1;
                else
                    registers[0xF] = 0;

                // Xor the pixel on the screen with the pixel of the sprite
                *pixel_ptr ^= 0xFFFFFFFF;
            }
        }
    }
}

void Chip8::decodeOpcodeE(uint16_t opcode)
{
    switch (opcode & 0x00FF)
    {
        case 0x009E: executeOpcodeEX9E(opcode); break;
        case 0x00A1: executeOpcodeEXA1(opcode); break;
        default:
            std::cout << "Unknown opcode: " << std::hex << opcode << std::endl;
            break;
    }
}

void Chip8::executeOpcodeEX9E(uint16_t opcode)
{
    // Skips the next instruction if the key stored in VX is pressed
    uint8_t VX = (opcode & 0x0F00) >> 8;
    uint8_t key = registers[VX];

    if (keypad[key])
        pc += 2;
}

void Chip8::executeOpcodeEXA1(uint16_t opcode)
{
    // Skips the next instruction if the key stored in VX is not pressed
    uint8_t VX = (opcode & 0x0F00) >> 8;
    uint8_t key = registers[VX];

    if (!keypad[key])
        pc += 2;
}

void Chip8::decodeOpcodeF(uint16_t opcode)
{
    switch (opcode & 0x00FF)
    {
        case 0x0007: executeOpcodeFX07(opcode); break;
        case 0x000A: executeOpcodeFX0A(opcode); break;
        case 0x0015: executeOpcodeFX15(opcode); break;
        case 0x0018: executeOpcodeFX18(opcode); break;
        case 0x001E: executeOpcodeFX1E(opcode); break;
        case 0x0029: executeOpcodeFX29(opcode); break;
        case 0x0033: executeOpcodeFX33(opcode); break;
        case 0x0055: executeOpcodeFX55(opcode); break;
        case 0x0065: executeOpcodeFX65(opcode); break;
        default:
            std::cout << "Unknown opcode: " << std::hex << opcode << std::endl;
            break;
    }
}

void Chip8::executeOpcodeFX07(uint16_t opcode)
{
    // Sets VX to the value of the delay timer
    uint8_t VX = (opcode & 0x0F00) >> 8;
    registers[VX] = delayTimer;
}

void Chip8::executeOpcodeFX0A(uint16_t opcode)
{
    // A key press is awaited, and then stored in VX.
    // (Blocking Operation. All instruction halted until next key event);
    uint8_t VX = (opcode & 0x0F00) >> 8;
    uint8_t key = -1;

    for (uint8_t i = 0; i < 16; i++)
    {
        if (keypad[i])
        {
            key = i;
            break;
        }
    }

    // If no key is pressed, pc is not incremented
    if (key != -1)
        registers[VX] = key;
    else
        pc -= 2;
}

void Chip8::executeOpcodeFX15(uint16_t opcode)
{
    // Sets the delay timer to VX
    uint8_t VX = (opcode & 0x0F00) >> 8;
    delayTimer = registers[VX];
}

void Chip8::executeOpcodeFX18(uint16_t opcode)
{
    // Sets the sound timer to VX
    uint8_t VX = (opcode & 0x0F00) >> 8;
    soundTimer = registers[VX];
}

void Chip8::executeOpcodeFX1E(uint16_t opcode)
{
    // Adds VX to I. VF is not affected
    uint8_t VX = (opcode & 0x0F00) >> 8;
    I += registers[VX];
}

void Chip8::executeOpcodeFX29(uint16_t opcode)
{
    // Sets I to the location of the sprite for the character in VX.
    // Characters 0-F (in hexadecimal) are represented by a 4x5 font.
    uint8_t VX = (opcode & 0x0F00) >> 8;
    I = FONTSET_START_ADDRESS + registers[VX] * 5;
}

void Chip8::executeOpcodeFX33(uint16_t opcode)
{
    // Stores the binary-coded decimal representation of VX,
    // with the most significant of three digits at the address in I,
    // the middle digit at I plus 1,
    // and the least significant digit at I plus 2.
    // (In other words, take the decimal representation of VX,
    // place the hundreds digit in memory at location in I,
    // the tens digit at location I+1,
    // and the ones digit at location I+2.)
    uint8_t VX = (opcode & 0x0F00) >> 8;
    uint8_t value = registers[VX];

    uint8_t hundreds = value / 100;
    uint8_t tens = (value % 100) / 10;
    uint8_t ones = value % 10;

    memory[I] = hundreds;
    memory[I + 1] = tens;
    memory[I + 2] = ones;
}

void Chip8::executeOpcodeFX55(uint16_t opcode)
{
    // Stores from V0 to VX (including VX) in memory, starting at address I.
    // The offset from I is increased by 1 for each value written,
    // but I itself is left unmodified
    uint8_t VX = (opcode & 0x0F00) >> 8;

    for (uint8_t i = 0; i <= VX; i++)
        memory[I + i] = registers[i];
}

void Chip8::executeOpcodeFX65(uint16_t opcode)
{
    // Fills from V0 to VX (including VX) with values from memory, starting at address I.
    // The offset from I is increased by 1 for each value read,
    // but I itself is left unmodified
    uint8_t VX = (opcode & 0x0F00) >> 8;

    for (uint8_t i = 0; i <= VX; i++)
        registers[i] = memory[I + i];
}
