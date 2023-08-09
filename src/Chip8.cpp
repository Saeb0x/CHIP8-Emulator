#include "Chip8.h"

#include <iostream>
#include <fstream>

// Font data for the 16 built-in characters (0-F), each is 5 bytes long
uint8_t fontSet[16 * 5] = {
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
};

Chip8::Chip8()
{
	pc = PROGRAM_START_ADDRESS;

	// Load the fonts into memory starting at address 0x050 to 0x0A0 (80 bytes)
	for (unsigned int i = 0; i < (16*5); ++i)
	{
		memory[FONTSET_START_ADDRESS + i] = fontSet[i];
	}
}

void Chip8::loadROM(const char* romFileName)
{
	std::ifstream file(romFileName, std::ios::binary);

	if (!file)
	{
		std::cerr << "Error: Failed to open ROM file." << std::endl;
		return;
	}

	file.seekg(0, std::ios::end);
	std::streampos fileSize = file.tellg();

	if (fileSize > MEMORY_SIZE - PROGRAM_START_ADDRESS) {
		std::cerr << "Error: ROM size exceeds available memory." << std::endl;
		return;
	}
	
	file.seekg(0, std::ios::beg);
	file.read(reinterpret_cast<char*>(memory + 0x200) , fileSize);

	file.close();
	
	std::cout << "Successfully loaded ROM: " << romFileName << std::endl;
}

void Chip8::emulateCycle()
{
	// Fetch opcode
	opcode = memory[pc] << 8u | memory[pc + 1];
	
	// Increment the PC before executing anything
	pc += 2;

	// Decode and execute opcode
	switch (opcode & 0xF000u)
	{
	case 0x0000u:
		switch (opcode & 0x000Fu)
		{
		case 0x0000u:
			OP_00E0();
			break;
		case 0x000Eu:
			OP_00EE();
			break;
		default:
			std::cerr << "Invalid opcode: 0x" << std::hex << opcode << std::endl;
			break;
		}
		break;
	case 0x1000u:
		OP_1NNN();
		break;
	case 0x2000u:
		OP_2NNN();
		break;
	case 0x3000u:
		OP_3XNN();
		break;
	case 0x4000u:
		OP_4XNN();
		break;
	case 0x5000u:
		OP_5XY0();
		break;
	case 0x6000u:
		OP_6XNN();
		break;
	case 0x7000u:
		OP_7XNN();
		break;
	case 0x8000u:
		switch (opcode & 0x000Fu)
		{
		case 0x0000u:
			OP_8XY0();
			break;
		case 0x0001u:
			OP_8XY1();
			break;
		case 0x0002u:
			OP_8XY2();
			break;
		case 0x0003u:
			OP_8XY3();
			break;
		case 0x0004u:
			OP_8XY4();
			break;
		case 0x0005u:
			OP_8XY5();
			break;
		case 0x0006u:
			OP_8XY6();
			break;
		case 0x0007u:
			OP_8XY7();
			break;
		case 0x000Eu:
			OP_8XYE();
			break;
		default:
			std::cerr << "Invalid opcode: 0x" << std::hex << opcode << std::endl;
			break;
		}
		break;
	case 0x9000u:
		OP_9XY0();
		break;
	case 0xA000u:
		OP_ANNN();
		break;
	case 0xB000u:
		OP_BNNN();
		break;
	case 0xC000u:
		OP_CXNN();
		break;
	case 0xD000u:
		OP_DXYN();
		break;
	case 0xE000u:
		switch (opcode & 0x00FFu)
		{
		case 0x009Eu:
			OP_EX9E();
			break;
		case 0x00A1u:
			OP_EXA1();
			break;
		default:
			std::cerr << "Invalid opcode: 0x" << std::hex << opcode << std::endl;
			break;
		}
		break;
	case 0xF000u:
		switch (opcode & 0x00FFu)
		{
		case 0x0007u:
			OP_FX07();
			break;
		case 0x000Au:
			OP_FX0A();
			break;
		case 0x0015u:
			OP_FX15();
			break;
		case 0x0018u:
			OP_FX18();
			break;
		case 0x001Eu:
			OP_FX1E();
			break;
		case 0x0029u:
			OP_FX29();
			break;
		case 0x0033u:
			OP_FX33();
			break;
		case 0x0055u:
			OP_FX55();
			break;
		case 0x0065u:
			OP_FX65();
			break;
		default:
			std::cerr << "Invalid opcode: 0x" << std::hex << opcode << std::endl;
			break;
		}
		break;
	default:
		std::cerr << "Invalid opcode: 0x" << std::hex << opcode << std::endl;
		break;
	}

	if (delayTimer > 0)
	{
		--delayTimer;
	}
	if (soundTimer > 0)
	{
		--soundTimer;
	}
}

void Chip8::OP_00E0()
{
	memset(display, 0, sizeof(display));
}

void Chip8::OP_00EE()
{
	--sp;
	pc = stack[sp];
}

void Chip8::OP_1NNN()
{
	uint16_t address = opcode & 0x0FFFu;
	pc = address;
}

void Chip8::OP_2NNN()
{
	uint16_t address = opcode & 0x0FFFu;

	stack[sp] = pc;
	++sp;
	pc = address;
}

void Chip8::OP_3XNN()
{
	uint8_t VX = (opcode >> 8u) & 0x000Fu;
	uint8_t ValueNN = opcode & 0x00FFu;

	if (registers[VX] == ValueNN)
	{
		pc += 2;
	}
}

void Chip8::OP_4XNN()
{
	uint8_t VX = (opcode >> 8u) & 0x000Fu;
	uint8_t ValueNN = opcode & 0x00FFu;

	if (registers[VX] != ValueNN)
	{
		pc += 2;
	}
}

void Chip8::OP_5XY0()
{
	uint8_t VX = (opcode >> 8u) & 0x0Fu;
	uint8_t VY = (opcode >> 4u) & 0x0Fu;

	if (registers[VX] == registers[VY])
	{
		pc += 2;
	}
}

void Chip8::OP_6XNN()
{
	uint8_t VX = (opcode >> 8u) & 0x0Fu;
	uint8_t ValueNN = opcode & 0x00FFu;

	registers[VX] = ValueNN;
}

void Chip8::OP_7XNN()
{
	uint8_t VX = (opcode >> 8u) & 0x0Fu;
	uint8_t ValueNN = opcode & 0x00FFu;

	registers[VX] += ValueNN;
}

void Chip8::OP_8XY0()
{
	uint8_t VX = (opcode >> 8u) & 0x0Fu;
	uint8_t VY = (opcode >> 4u) & 0x0Fu;

	registers[VX] = registers[VY];
}

void Chip8::OP_8XY1()
{
	uint8_t VX = (opcode >> 8u) & 0x0Fu;
	uint8_t VY = (opcode >> 4u) & 0x0Fu;

	registers[VX] |= registers[VY];
}

void Chip8::OP_8XY2()
{
	uint8_t VX = (opcode >> 8u) & 0x0Fu;
	uint8_t VY = (opcode >> 4u) & 0x0Fu;

	registers[VX] &= registers[VY];
}

void Chip8::OP_8XY3()
{
	uint8_t VX = (opcode >> 8u) & 0x0Fu;
	uint8_t VY = (opcode >> 4u) & 0x0Fu;

	registers[VX] ^= registers[VY];
}

void Chip8::OP_8XY4()
{
	uint8_t VX = (opcode >> 8u) & 0x0Fu;
	uint8_t VY = (opcode >> 4u) & 0x0Fu;

	uint16_t sum = registers[VX] + registers[VY];

	registers[0xF] = (sum > 255) ? 1 : 0;

	registers[VX] = sum & 0xFFu;
}

void Chip8::OP_8XY5()
{
	uint8_t VX = (opcode >> 8u) & 0x0Fu;
	uint8_t VY = (opcode >> 4u) & 0x0Fu;

	registers[0xF] = (registers[VX] >= registers[VY]) ? 1 : 0;

	registers[VX] -= registers[VY];
}

void Chip8::OP_8XY6()
{
	uint8_t VX = (opcode >> 8u) & 0x0Fu;
	uint8_t VY = (opcode >> 4u) & 0x0Fu;

	registers[0xF] = registers[VY] & 0x1u;

	registers[VX] = registers[VY] >> 1;
}

void Chip8::OP_8XY7()
{
	uint8_t VX = (opcode >> 8u) & 0x0Fu;
	uint8_t VY = (opcode >> 4u) & 0x0Fu;

	if (registers[VY] > registers[VX])
	{
		registers[0xF] = 1;
	}
	else
	{
		registers[0xF] = 0;
	}

	registers[VX] = registers[VY] - registers[VX];
}

void Chip8::OP_8XYE()
{
	uint8_t VX = (opcode >> 8u) & 0x0Fu;

	registers[0xF] = (registers[VX] >> 7u) & 0x1u;

	registers[VX] <<= 1;
}

void Chip8::OP_9XY0()
{
	uint8_t VX = (opcode >> 8u) & 0x0Fu;
	uint8_t VY = (opcode >> 4u) & 0x0Fu;

	if (registers[VX] != registers[VY])
	{
		pc += 2;
	}
}

void Chip8::OP_ANNN()
{
	uint16_t address = opcode & 0x0FFFu;

	indexRegister = address;
}

void Chip8::OP_BNNN()
{
	uint16_t address = opcode & 0x0FFFu;

	pc = registers[0x0] + address;
}

void Chip8::OP_CXNN()
{
	uint8_t VX = (opcode >> 8u) & 0x0Fu;
	uint8_t ValueNN = opcode & 0x00FFu;

	uint8_t randomValue = std::rand() & 0xFFu;

	registers[VX] = randomValue & ValueNN;
}

void Chip8::OP_DXYN()
{
	uint8_t VX = registers[(opcode >> 8u) & 0x0Fu];
	uint8_t VY = registers[(opcode >> 4u) & 0x0Fu];
	uint8_t height = opcode & 0x000Fu;

	registers[0xF] = 0;

	for (uint8_t row = 0; row < height; ++row)
	{
		uint8_t spriteByte = memory[indexRegister + row];

		for (uint8_t col = 0; col < 8; ++col)
		{
			uint8_t spritePixel = spriteByte & (0x80u >> col);
			uint32_t* screenPixel = &display[(VX + col) + (VY + row) * DISPLAY_WIDTH];

			// Map the sprite pixel to the corresponding RGBA value.
			uint32_t rgbaSpritePixel = spritePixel ? 0xFFFFFFFF : 0x00000000;

			if (spritePixel && *screenPixel == 0xFFFFFFFF)
			{
				registers[0xF] = 1;
			}

			*screenPixel ^= rgbaSpritePixel;
		}
	}
}

void Chip8::OP_EX9E()
{
	uint8_t VX = (opcode >> 8u) & 0x0Fu;
	
	if (keypad[registers[VX]])
	{
		pc += 2;
	}
}

void Chip8::OP_EXA1()
{
	uint8_t VX = (opcode >> 8u) & 0x0Fu;

	if (!keypad[registers[VX]])
	{
		pc += 2;
	}
}

void Chip8::OP_FX07()
{
	uint8_t VX = (opcode >> 8u) & 0x0Fu;

	registers[VX] = delayTimer;
}

void Chip8::OP_FX0A()
{
	uint8_t VX = (opcode >> 8u) & 0x0Fu;

	bool keyPressed = false;
	for (unsigned int i = 0; i < 16; ++i)
	{
		if (keypad[i])
		{
			registers[VX] = static_cast<uint8_t>(i);
			keyPressed = true;
			break;
		}

		// If no key is pressed, repeat the instruction to wait for a key press
		if (!keyPressed)
		{
			pc -= 2;
		}
	}
}

void Chip8::OP_FX15()
{
	uint8_t VX = (opcode >> 8u) & 0x0Fu;

	delayTimer = registers[VX];
}

void Chip8::OP_FX18()
{
	uint8_t VX = (opcode >> 8u) & 0x0Fu;

	soundTimer = registers[VX];
}

void Chip8::OP_FX1E()
{
	uint8_t VX = (opcode >> 8u) & 0x0Fu;

	indexRegister += registers[VX];
}

void Chip8::OP_FX29()
{
	uint8_t VX = (opcode >> 8u) & 0x0Fu;
	uint8_t digit = registers[VX];

	indexRegister = FONTSET_START_ADDRESS + (5 * digit);
}

void Chip8::OP_FX33()
{
	uint8_t VX = (opcode >> 8u) & 0x0Fu;
	uint8_t value = registers[VX];

	uint8_t hundreds = value / 100;
	uint8_t tens = (value / 10) % 10;
	uint8_t ones = value % 10;

	// Store BCD representation in memory
	memory[indexRegister] = hundreds;
	memory[indexRegister + 1] = tens;
	memory[indexRegister + 2] = ones;
}

void Chip8::OP_FX55()
{
	uint8_t VX = (opcode >> 8u) & 0x0Fu;

	for (uint8_t i = 0; i <= VX; ++i)
	{
		memory[indexRegister + i] = registers[i];
	}
}

void Chip8::OP_FX65()
{
	uint8_t VX = (opcode >> 8u) & 0x0Fu;

	for (uint8_t i = 0; i <= VX; ++i)
	{
		registers[i] = memory[indexRegister + i];
	}
}


