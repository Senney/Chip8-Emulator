#ifndef CHIP_8_HEADER
#define CHIP_8_HEADER

class Chip8
{
public:
	Chip8();
	~Chip8();

	void initialize();
	void emulateCycle();

	bool loadProgram(const char* filename);

	
	
	// Flag to indicate whether a new frame should be drawn.
	bool drawFlag;
	unsigned char* getGraphics();
	
	// Hard-coded char set.
	static unsigned char chip8_fontset[80];
private:
	void clearScreen();

	// Current opcode.
	unsigned short opcode;

	// Chip 8 has 4K of memory.
	unsigned char memory[4096];

	// Registers - 16 8 bit registers.
	// Ranges from V0 -> VF.
	unsigned char V[16];

	// Index register
	unsigned short I;
	// Program counter
	unsigned short pc;

	// Graphics memory
	unsigned char graphics[64 * 32];

	// Two timers
	unsigned char delay_timer;
	unsigned char sound_timer;

	// Stack
	unsigned short stack[16];
	unsigned short sp;

	// Key press - 16 keys.
	unsigned short key[16];

	// Hack for wait-on-keypress.
	bool waiting_key;
	bool key_pressed;
	short lastKey;

};

#endif