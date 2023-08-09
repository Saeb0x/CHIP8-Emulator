#pragma once

#include <cstdint>

const unsigned int MEMORY_SIZE = 0x1000;
const unsigned int PROGRAM_START_ADDRESS = 0x200;
const unsigned int FONTSET_START_ADDRESS = 0x050;
const unsigned int DISPLAY_WIDTH = 64;
const unsigned int DISPLAY_HEIGHT = 32;

class Chip8
{
public:
	Chip8();
	void loadROM(const char* romFileName);
	void emulateCycle();
private:
	void OP_00E0();
	void OP_00EE();
	void OP_1NNN();
	void OP_2NNN();
	void OP_3XNN();
	void OP_4XNN();
	void OP_5XY0();
	void OP_6XNN();
	void OP_7XNN();
	void OP_8XY0();
	void OP_8XY1();
	void OP_8XY2();
	void OP_8XY3();
	void OP_8XY4();
	void OP_8XY5();
	void OP_8XY6();
	void OP_8XY7();
	void OP_8XYE();
	void OP_9XY0();
	void OP_ANNN();
	void OP_BNNN();
	void OP_CXNN();
	void OP_DXYN();
	void OP_EX9E();
	void OP_EXA1();
	void OP_FX07();
	void OP_FX0A();
	void OP_FX15();
	void OP_FX18();
	void OP_FX1E();
	void OP_FX29();
	void OP_FX33();
	void OP_FX55();
	void OP_FX65();
public:
	uint8_t registers[16]{};
	uint8_t memory[MEMORY_SIZE]{};
	uint16_t indexRegister{};
	uint16_t pc{};
	uint16_t stack[16]{};
	uint8_t sp{};
	uint16_t opcode{};
	uint8_t delayTimer{};
	uint8_t soundTimer{};
	uint8_t keypad[16]{};
	uint32_t display[DISPLAY_WIDTH * DISPLAY_HEIGHT]{};
};