#include"Emulating_4bit_RAM.h"
#include<vector>

int main()
{
	// 4-bit "Language" Specification
	// 
	// Each comand is given in 8 (2 by 4) bits:
	// 
	// first 2-Bits - Indicates what comand is this 
	// 
	// 11 -> End program 
	// 01 -> Store
	// 10 -> Load
	// 00 -> Unused (currently produces Error)
	// 
	// second 2-bits - Indicates what register we are refering to
	// 
	// The Next 4 bits will indicate what memory location we are refering to 

	std::vector<int> program
	{
		0, 0, 0, 1,//memory location #0000 acts as the Program counter 

			1, 0, 1, 1,//Load into register 4 (#11),
			1, 1, 1, 1,//the contents stored at #1111.
			1, 0, 0, 1,//Load into register 2 (#01),
			0, 0, 1, 1,//the contents stored at #0010.
			0, 1, 0, 1,//Store from register 2 (#01),
			0, 0, 0, 0,//into memory location #0000 (The PC)
			0, 0, 0, 0,
			0, 0, 0, 0,
			1, 1, 0, 0,//End program
			0, 0, 0, 0,
			0, 0, 0, 0,
			0, 0, 0, 0,
			0, 0, 0, 0,
			0, 0, 0, 0,
			1, 1, 1, 0,
	};

	RAM_4bit::run(program);
}
