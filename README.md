# 4-Bit-RAM
4-bit RAM and a simple instruction set that runs on a single tape turing machine.

Emulating_4bit_RAM.h defines a 4-bit RAM that runs on the Turing machine (defined in ComM.h). The model contains a random access memory with 4 registers that the LOAD and STORE commands can use. The model also uses memory location 0 as a program counter (hence it is modifiable during runtime). Program_example.cpp contains an example program that runs showing the 4-bit RAM running a program pre-written into memory.   The 4-bit machine "language" is defined as follows:    

 
 Each command is given in 8 (2 by 4) bits:

  - first 2-Bits - Indicates command type    
    - 11 -> End program (and Print contents of registers)  
    - 01 -> Store (i.e from a regiser into a memory location)  
    - 10 -> load (i.e from a memory location into a regiser)  
    - 00 -> Unused, currently will throw error  

  - second 2-bits - Indicates what register we are referring to.

  - The next 4 bits will indicate what memory location we are referring to. 
