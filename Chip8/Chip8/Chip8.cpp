#include <fstream>
#include <string>
#include <cstdio>

#include "Chip8.h"

unsigned char Chip8::chip8_fontset[] = { 
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

Chip8::Chip8() { 
	waiting_key = false; key_pressed = false; lastKey = -1;
}
Chip8::~Chip8() { }

void Chip8::initialize()
{
	// Initialize all registers to 0.
	unsigned int i;
	for (i = 0; i < 16; i++) {
		this->V[i] = 0;
	}
	
	// Initialize the memory to 0.
	for (i = 0; i < 4096; i++) {
		this->memory[i] = 0;
	}

	// Clear graphics memory
	this->clearScreen();

	// Clear stack
	for (i = 0; i < 16; i++)
		this->stack[i] = 0;

	// Program counter starts at 0x200 in memory.
	this->pc = 0x200;
	
	// Clear storage.
	this->opcode = 0;
	this->I = 0;
	this->sp = 0;

	// Load the fontset. 
	for (i = 0; i < 80; i++)
		this->memory[0x50 + i] = Chip8::chip8_fontset[i]; // Replace with fontset reference.
}

void Chip8::emulateCycle()
{
	// Get the next opcode.
	// Opcodes are stored in 2 spaces in memory, as they are 2 bytes long.
	this->opcode = this->memory[pc] << 8 | this->memory[pc + 1];

	// Switch based on the first element of the opcode.
	switch (opcode & 0xF000) {
	case 0x0000:
		// handle 2-3 cases
		switch(opcode & 0x00FF) {
		case 0x00E0:
			// Clear the screen.
			clearScreen();
			pc += 2;
			break;
		case 0x00EE:
			// Return from subroutine.
			pc = this->stack[sp--];
			break;
		default:
			pc = 0x200 + (opcode & 0x0FFF);
			printf("Jumping to: %X\n", 0x200 + (opcode & 0x0FFF));
			break;
		}
		break;
	case 0x1000:
		// Handle jump.
		pc = opcode & 0x0FFF;
		break;
	case 0x2000:
		// Subroutine call.

		// Set the return address.
		stack[sp] = pc;
		++sp;

		// Set the program counter to the srt address.
		pc = opcode & 0x0FFF;
		break;
	case 0x3000:
		// Skip next if VX = NN
		if (this->V[(opcode & 0x0F00) >> 8] == (opcode & 0x00FF))
			this->pc += 4; // Skip forward to the next opcode;
		else
			this->pc += 2; // Do the next instruction.
		break;
	case 0x4000:
		// Skip if VX != NN.
		if (this->V[(opcode & 0x0F00) >> 8] != (opcode & 0x00FF))
			this->pc += 4;
		else
			this->pc += 2;

		break;
	case 0x5000:
		if (this->V[(opcode & 0x0F00) >> 8] == this->V[(opcode & 0x00F0) >> 4])
			this->pc += 4;
		else
			this->pc += 2;
		break;
	case 0x6000:
		// Set VX to NN.
		this->V[(opcode & 0x0F00) >> 8] = (opcode & 0x00FF);
		pc += 2;
		break;
	case 0x7000:
		this->V[(opcode & 0x0F00) >> 8] += (opcode & 0x00FF);
		pc += 2;
		break;
	case 0x8000:
		// 8 bitwise cases.
		switch (opcode & 0x000F) {
		case 0x0000:
			// Set
			this->V[(opcode & 0x0F00) >> 8] = this->V[(opcode & 0x00F0) >> 4];
			break;
		case 0x0001:
			// Bitwise or.
			this->V[(opcode & 0x0F00) >> 8] |= this->V[(opcode & 0x00F0) >> 4];
			break;
		case 0x0002:
			// bitwise and.
			this->V[(opcode & 0x0F00) >> 8] &= this->V[(opcode & 0x00F0) >> 4];
			break;
		case 0x0003:
			// bitwise xor.
			this->V[(opcode & 0x0F00) >> 8] ^= this->V[(opcode & 0x00F0) >> 4];
			break;
		case 0x0004:
			// Add two registers.
			if (this->V[(opcode & 0x00F0) >> 4] > (0xFF - this->V[(opcode & 0x0F00) >> 8]))
				this->V[0xF] = 1; // Set the carry flag.
			else
				this->V[0xF] = 0; // Clear the carry flag.
			break;

			this->V[(opcode & 0x0F00) >> 8] += this->V[(opcode & 0x00F0) >> 4];
			pc += 2;
		case 0x0005:
			// Subtract two registers.
			if (this->V[(opcode & 0x00F0) >> 4] > this->V[(opcode & 0x0F00) >> 8])
				this->V[0xF] = 1;
			else
				this->V[0xF] = 0;

			this->V[(opcode & 0x0F00) >> 8] -= this->V[(opcode & 0x00F0) >> 4];
			break;
		case 0x0006:
			// Set VF to the least significant bit.
			this->V[0xF] = (opcode & 0x0001);

			// Shift right.
			this->V[(opcode & 0x0F00) >> 8] >>= 1;
			break;
		case 0x0007:
			// VX = VY - VX
			if (this->V[(opcode & 0x0F00) >> 8] > this->V[(opcode & 0x00F0) >> 4])
				this->V[0xF] = 1;
			else
				this->V[0xF] = 0;

			this->V[(opcode & 0x0F00) >> 8] = this->V[(opcode & 0x00F0) >> 4] - this->V[(opcode & 0x0F00) >> 8];
			break;
		case 0x000E:
			// Set VF to the least significant bit.
			this->V[0xF] = (opcode & 0x0001);

			// Shift right.
			this->V[(opcode & 0x0F00) >> 8] <<= 1;
			break;
		}
		pc += 2;
		break;
	case 0x9000:
		// Skip next if VX != VY
		if (V[(opcode & 0x0F00) >> 8] == V[(opcode & 0x00F0) >> 4])
			this->pc += 4;
		else
			this->pc += 2;
		break;
	case 0xA000:
		// Set I to NNN
		this->I = (opcode & 0x0FFF);
		this->pc += 2;
		break;
	case 0xB000:
		// Jump to NNN + V0
		this->pc = (opcode & 0x0FFF) + this->V[0];
		break;
	case 0xC000:
		// Set VX to rand() & NN
		this->V[(opcode & 0x0F00) >> 8] = rand() & (opcode & 0x00FF);
		pc += 2;
		break;
	case 0xD000:
		{
			// Draw sprite to screen.
			unsigned short x = this->V[(opcode & 0x0F00) >> 8];
			unsigned short y = this->V[(opcode & 0x00F0) >> 4];
			unsigned short h = opcode & 0x000F;
			unsigned short pixel;

			this->V[0xF] = 0;
			for (int yl = 0; yl < h; yl++) {
				pixel = this->memory[I + yl];
				for (int xl = 0; xl < 8; xl++) {
					if ((pixel & (0x80 >> xl)) != 0)
					{
						if (this->graphics[(x + xl + ((y + yl) * 64))] == 1)
							V[0xF] = 1;
						this->graphics[(x + xl + ((y + yl) * 64))] ^= 1;
					}
				}
			}
			drawFlag = true;
			pc += 2;
		}
		break;
	case 0xE000:
		// 2 cases
		// Skip based on keypress.
		char k;
		switch (opcode & 0x00FF) {
		case 0x009E:
			k = this->V[(opcode & 0x0F00) >> 8];
			if (k > 16)
				break;
			if (this->key[k] == 1) pc += 2;
			break;
		case 0x00A1:
			k = this->V[(opcode & 0x0F00) >> 8];
			if (k > 16)
				break;
			if (this->key[k] == 0) pc += 2;
			break;
		}
		pc += 2;
		break;
	case 0xF000:
		// 9 timer cases
		switch (opcode & 0x00FF) {
		case 0x0007:
			this->V[(opcode & 0x0F00) >> 8] = this->delay_timer;
			pc += 2;
			break;
		case 0x000A:
			// Wait for keypress.
			if (waiting_key) {
				if (key_pressed && lastKey != -1) {
					this->V[(opcode & 0x0F00) >> 8] = (unsigned char)key[lastKey];

					key_pressed = false;
					lastKey = -1;
					pc += 2;
				}
			} else {
				waiting_key = true;
			}
			break;
		case 0x0015:
			// Set delay timer to VX.
			this->delay_timer = this->V[(opcode & 0x0F00) >> 8];
			pc += 2;
			break;
		case 0x0018:
			// Set sound timer to VX
			this->sound_timer = this->V[(opcode & 0x0F00) >> 8];
			pc += 2;
			break;
		case 0x001E:
			// Add VX to I.
			this->I += this->V[(opcode & 0x0F00) >> 8];
			pc += 2;
			break;
		case 0x0029:
			this->I = 0x50 + this->V[(opcode & 0x0F00) >> 8];
			pc += 2;
			break;
		case 0x0033:
			this->memory[this->I]		= this->V[(opcode & 0x0F00) >> 8] / 100;
			this->memory[this->I + 1]	= (this->V[(opcode & 0x0F00) >> 8] / 10) % 10;
			this->memory[this->I + 2]	= (this->V[(opcode & 0x0F00) >> 8] % 100) % 10;
			pc += 2;
			break;
		case 0x0055:
			for (char i = 0; i < ((opcode & 0x0F00) >> 8); i++) 
				this->memory[this->I + i] = this->V[i];
			pc += 2;
			break;
		case 0x0065:
			for (char i = 0; i < ((opcode & 0x0F00) >> 8); i++) 
				this->V[i] = this->memory[this->I + i];
			pc += 2;
			break;
		default:
			printf("%x opcode not supported!", opcode);
			pc+=2;
			break;
		}
		break;
	}
	
	if (this->delay_timer > 0) this->delay_timer--;
	if (this->sound_timer > 0) {
		if (this->sound_timer == 1)
			printf("Beep!\n");
		this->sound_timer--;
	}
}

bool Chip8::loadProgram(const char* filename) {
	// Load the file in binary mode.
	std::ifstream in(filename, std::ios_base::binary);
	if (in.fail())
		return false;

	std::string buffer;

	// Store the file data into the string buffer.
	char ch;
	while (in.get(ch))
		buffer.push_back(ch);

	// Close the input stream.
	in.close();

	// Check if the total file size is greater than available program space.
	if (buffer.length() > (4096 - 0x200))
		return false;

	for (unsigned int i = 0; i < buffer.length(); i++)
		this->memory[i + 0x200] = (char)buffer.at(i);

	return true;
}

void Chip8::clearScreen() {
	// Clear graphics memory
	for (unsigned int i = 0; i < (64*32); i++)
		this->graphics[i] = 0;

	this->drawFlag = true;

	return;
}

unsigned char* Chip8::getGraphics() {
	return graphics;
}