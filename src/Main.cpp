#include <chrono>

#include "Chip8.h"
#include "Window.h"

const int TIMER_RATE = 60;
const auto TIMER_PERIOD = std::chrono::microseconds(1000000 / TIMER_RATE);
const int  DISPLAY_SIZE = DISPLAY_WIDTH * DISPLAY_HEIGHT;

int main(int argc, char* argv[])
{
	if (argc != 4)
	{
		std::cerr << "Usage: " << argv[0] << " <Video Scale> <Cycle Rate> <ROM>\n";
		return 1;
	}

	int videoScale = std::atoi(argv[1]);
	int cycleRate = std::atoi(argv[2]);
	char const* romFilename = argv[3];

	const auto CHIP8_CYCLE_PERIOD = std::chrono::microseconds(1000000 / cycleRate);

	Chip8 myChip8;
	Window window(DISPLAY_WIDTH * videoScale, DISPLAY_HEIGHT * videoScale, "CHIP-8 Emulator", &myChip8);

	myChip8.loadROM(romFilename);

	auto lastCycleTime = std::chrono::high_resolution_clock::now();

	uint32_t* flippedDisplay = new uint32_t[DISPLAY_SIZE];

	auto lastTimerTime = lastCycleTime;
	const int texture = *(window.getTexture());
	const int vao = *(window.getVAO());

	while (!window.shouldClose())
	{
		auto now = std::chrono::high_resolution_clock::now();
		while (now - lastCycleTime > CHIP8_CYCLE_PERIOD)
		{
			myChip8.emulateCycle();
			lastCycleTime += CHIP8_CYCLE_PERIOD;
		}

		window.clear();
		glBindTexture(GL_TEXTURE_2D, texture);

		// Flipping the display vertically
		for (int y = 0; y < 32; y++)
		{
			for (int x = 0; x < 64; x++)
			{
				flippedDisplay[x + (31 - y) * 64] = myChip8.display[x + y * 64];
			}
		}

		glViewport(0, 0, DISPLAY_WIDTH * videoScale - 300.0f, DISPLAY_HEIGHT * videoScale);
		glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, DISPLAY_WIDTH, DISPLAY_HEIGHT, GL_RGBA, GL_UNSIGNED_BYTE, flippedDisplay);

		glBindVertexArray(vao);
		glDrawArrays(GL_TRIANGLES, 0, 6);
		glBindVertexArray(0);
		glBindTexture(GL_TEXTURE_2D, 0);

		window.renderImGui();
		window.update();
	}

	delete[] flippedDisplay;
	return 0;
}