#include <iostream>
#include <string>

#include <SFML/Window.hpp>
#include <SFML/Graphics.hpp>

#include "Chip8.h"

void createImage(unsigned char* gfx, sf::Image& img);

int main(int argc, char** argv) 
{
	std::cout << "Chip8 emulator - Sean Heintz - Help from http://www.multigesture.net/articles/how-to-write-an-emulator-chip-8-interpreter/" << std::endl;

	Chip8 cpu;
	bool running = true;

	// Get the filename.
	std::cout << "Please input the program that you would wish to load." << std::endl;
	std::string filename;
	std::cin >> filename;

	// Initialize the cpu.
	cpu.initialize();
	if (!cpu.loadProgram(filename.c_str()))
		std::cout << "Error: Could not find the program!" << std::endl;
	
	sf::RenderWindow mWindow(sf::VideoMode(64, 32),"Chip8 Emulator");

	sf::Time time;
	float lastTime, curTime;
	curTime = time.asMilliseconds();
	lastTime = 0;

	sf::Event mEvent;
	sf::Image img;
	img.create(64, 32, sf::Color(255, 255, 255));
	sf::Texture tex; sf::Sprite image;
	while (mWindow.isOpen())
	{
		// Update the CPU timing.
		curTime = time.asMilliseconds();

		while (mWindow.pollEvent(mEvent))
		{
			if (mEvent.type == sf::Event::Closed) {
				mWindow.close();
			}
		}
		
		// Time to update?
		if (curTime - lastTime >= (1000 / 60)) {
			cpu.emulateCycle();
			lastTime = curTime;
		}

		if (cpu.drawFlag) {
			createImage(cpu.getGraphics(), img);
			tex.loadFromImage(img);
			image.setTexture(tex);
			mWindow.draw(image);

			cpu.drawFlag = false;
		}

		mWindow.display();
	}

	return 0;
}

void createImage(unsigned char* gfx, sf::Image& image) {
	for (int y = 0; y < 32; y++)
		for (int x = 0; x < 64; x++) {
			if (gfx[(y * 64) + x] == 1)
				image.setPixel(x, y, sf::Color(255, 255, 255, 255));
		}
}
