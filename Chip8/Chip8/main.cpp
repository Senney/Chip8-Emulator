#include <iostream>
#include <string>

#include <SFML/Window.hpp>
#include <SFML/Graphics.hpp>

#include "Chip8.h"

#define XSIZE 640
#define YSIZE 320

void createImage(unsigned char* gfx, sf::Image& img);
unsigned short* handleKeys();

int main(int argc, char** argv) 
{
	std::cout << "Chip8 emulator - Sean Heintz - Help from http://www.multigesture.net/articles/how-to-write-an-emulator-chip-8-interpreter/" << std::endl;

	Chip8 cpu;
	bool running = true;
	std::string filename = "";

	while (true) {

		// Get the filename.
		std::cout << "Enter q or Q to quit!" << std::endl;
		std::cout << "Please input the program that you would wish to load." << std::endl;
		std::cin >> filename;

		if (filename == "q" || filename == "Q") break;

		// Initialize the cpu.
		cpu.initialize();
		if (!cpu.loadProgram(filename.c_str())) {
			std::cout << "Error: Could not find the program!" << std::endl;
			return -1;
		}
	
		sf::RenderWindow mWindow(sf::VideoMode(XSIZE, YSIZE),"Chip8 Emulator");

		// Timing
		sf::Clock timer;
		int lastTime, curTime;
		lastTime = curTime = 0;

		sf::Event mEvent;
		sf::Image img;
		img.create(64, 32, sf::Color(0, 0, 0));
		sf::Texture tex; sf::Sprite image;
		while (mWindow.isOpen())
		{
			curTime = timer.getElapsedTime().asMilliseconds();
			while (mWindow.pollEvent(mEvent))
			{
				if (mEvent.type == sf::Event::Closed) {
					mWindow.close();
				}
			}

			if (curTime - lastTime >= (1000/500)) {
				lastTime = curTime;
				cpu.setKeys(handleKeys());
				cpu.emulateCycle();

				if (cpu.drawFlag) {
					img.create(64, 32, sf::Color(0, 0, 0));
					createImage(cpu.getGraphics(), img);
					tex.loadFromImage(img);
					image.setTexture(tex);
					image.setScale(XSIZE / 64, YSIZE / 32);

					mWindow.clear();

					cpu.drawFlag = false;
				}
			}
		
			mWindow.draw(image);
			mWindow.display();
		}
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

unsigned short* handleKeys() {
	unsigned short* keys = new unsigned short[16];
	// Initialize keys to 0.
	for (int i = 0; i < 16; i++) keys[i] = 0;
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Num1))
		keys[0] = 1;
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Num2))
		keys[1] = 1;
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Num3))
		keys[2] = 1;
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Num4))
		keys[0xC] = 1;
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Q))
		keys[3] = 1;
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::W))
		keys[4] = 1;
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::E))
		keys[5] = 1;
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::R))
		keys[0xD] = 1;
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::A))
		keys[6] = 1;
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::S))
		keys[7] = 1;
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::D))
		keys[8] = 1;
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::F))
		keys[0xE] = 1;
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Z))
		keys[9] = 1;
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::X))
		keys[10] = 1;
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::C))
		keys[11] = 1;
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::V))
		keys[0xF] = 1;
	
	return keys;
}