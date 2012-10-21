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
	if (!cpu.loadProgram(filename.c_str())) {
		std::cout << "Error: Could not find the program!" << std::endl;
		return -1;
	}
	
	sf::RenderWindow mWindow(sf::VideoMode(100, 100),"Chip8 Emulator");

	sf::Event mEvent;
	sf::Image img;
	img.create(64, 32, sf::Color(0, 0, 0));
	sf::Texture tex; sf::Sprite image;
	while (mWindow.isOpen())
	{
		while (mWindow.pollEvent(mEvent))
		{
			if (mEvent.type == sf::Event::Closed) {
				mWindow.close();
			}
		}

		cpu.emulateCycle();

		if (cpu.drawFlag) {
			createImage(cpu.getGraphics(), img);
			tex.loadFromImage(img);
			image.setTexture(tex);

			mWindow.clear();

			cpu.drawFlag = false;

			std::cout << "Drawing!" << std::endl;
		}
		
		mWindow.draw(image);
		mWindow.display();
	}

	return 0;
}

void createImage(unsigned char* gfx, sf::Image& image) {
	for (int y = 0; y < 32; y++)
		for (int x = 0; x < 64; x++) {
			if (gfx[(y * 64) + x] != 0)
				image.setPixel(x, y, sf::Color(255, 255, 255));
		}
}
