#pragma once

#include"ComM.h"
#include<iostream>
#include<vector>


namespace RAM_4bit
{
    enum states :size_t
    {
        //Base states 
        end,
        NA1,
        NA2,

        go,
        return_to_start,


        //error states
        error,
        pc_8bit_instruction_overflow,

        //Core states
        reset,
        get,
        read_Q,
        pc_inc,
        read_2bits,

        //Command states
        clear_register,
        clear,
        load,

        mark_mem_location,
        read_R,
        store,

        //Adjustments
        last_LOC,
        pc_read,

        //4 - Bit Read and write states
        //By using states to read, store & write the machine becomes significantly smaller and more intuiative (though at the cost of having significantly more states).
        Q1XXX, L1XXX, IL1XXX, RXXX1,
        Q0XXX, L0XXX, IL0XXX, RXXX0,

        Q10XX, L10XX, IL10XX, RXX11,
        Q01XX, L01XX, IL01XX, RXX01,
        Q11XX, L11XX, IL11XX, RXX10,
        Q00XX, L00XX, IL00XX, RXX00,

        Q111X, L111X, IL111X, RX111,
        Q110X, L110X, IL110X, RX101,
        Q101X, L101X, IL101X, RX011,
        Q100X, L100X, IL100X, RX001,
        Q011X, L011X, IL011X, RX110,
        Q010X, L010X, IL010X, RX100,
        Q001X, L001X, IL001X, RX010,
        Q000X, L000X, IL000X, RX000,

        Q1110, L1110, IL1110, R1110,
        Q1100, L1100, IL1100, R1100,
        Q1010, L1010, IL1010, R1010,
        Q1000, L1000, IL1000, R1000,
        Q0110, L0110, IL0110, R0110,
        Q0100, L0100, IL0100, R0100,
        Q0010, L0010, IL0010, R0010,
        Q0000, L0000, IL0000, R0000,
        Q1111, L1111, IL1111, R1111,
        Q1101, L1101, IL1101, R1101,
        Q1011, L1011, IL1011, R1011,
        Q1001, L1001, IL1001, R1001,
        Q0111, L0111, IL0111, R0111,
        Q0101, L0101, IL0101, R0101,
        Q0011, L0011, IL0011, R0011,
        Q0001, L0001, IL0001, R0001,

        //2 - Bit Read and write states
        RX0,
        RX1,

        R00,
        R10,
        R01,
        R11,
    };

    enum symbols :size_t
    {
        //Base Markers
        Blank,
        Start,
        Zero,
        One,
        Error,

        //Core Instructions
        Reset,
        Get_command,
        PC_inc1,
        Get_memory,
        PC_inc2,
        Prep_register,
        Read_command,
        Waiting,

        //Command Instructions
        Load,
        Load_from_mem,

        Store,
        Send_register,

        End,

        //Memory Management
        LOC,
        PASS,
        MARK,
        PC,
        SEND_BACK,
        PC_INC,
        PC_WRITE,

        //Register Management
        REGISTER,
        ACTIVE_REGISTER,

    };

    inline void add_ruleset(Turing::Rules& rules)
    {
        //DEFINE GROUPS 
        std::vector<size_t> Q_Segments_full
        {
            Q1110,Q1100,Q1010,Q1000,Q0110,Q0100,Q0010,Q0000,Q1111,Q1101,Q1011,Q1001,Q0111,Q0101,Q0011,Q0001,//reading states 
        };

        std::vector<size_t> L_Write_Segments_full
        {
            L1110,L1100,L1010,L1000,L0110,L0100,L0010,L0000,L1111,L1101,L1011,L1001,L0111,L0101,L0011,L0001,//return and write state LEFTward
        };

        std::vector<size_t> R_Write_Segments_full
        {
            R1110,R1100,R1010,R1000,R0110,R0100,R0010,R0000,R1111,R1101,R1011,R1001,R0111,R0101,R0011,R0001,//send and write state RIGHTward
        };

        std::vector<size_t> IL_Write_Segments_full
        {
            IL1110,IL1100,IL1010,IL1000,IL0110,IL0100,IL0010,IL0000,IL1111,IL1101,IL1011,IL1001,IL0111,IL0101,IL0011,IL0001,//instant write states (used in PC increment)
        };

        std::vector<size_t> Register_Segments_full
        {
            R00,R10,R01,R11,//2-bit Register & Command read states
        };

        //Core cycle
        rules.add({ go, Start }, { go, Start , Turing::RIGHT });

        rules.add({ go, Reset }, { reset, Get_command , Turing::LEFT }); //Reset the Core command and memory registers

        rules.add({ go, Get_command }, { get, PC_inc1, Turing::RIGHT }); //Fill Core command register

        rules.add({ go, PC_inc1 }, { pc_inc, Get_memory, Turing::RIGHT }); //Increment the PC 

        rules.add({ go, Get_memory }, { get, PC_inc2, Turing::RIGHT }); //Fill Core Memory register

        rules.add({ go, PC_inc2 }, { pc_inc, Prep_register, Turing::RIGHT }); //Increment the PC 

        rules.add({ go, Prep_register }, { read_2bits, Read_command, Turing::LEFT }); //read the register that will be used in executing the command and them mark it

        rules.add({ go, Read_command }, { read_2bits, Waiting, Turing::LEFT }); // Read the command to be executed




        //Load Command
        rules.add({ go, Load }, { clear_register, Load_from_mem , Turing::RIGHT });
        rules.add({ go, Load_from_mem }, { load, Reset , Turing::LEFT });

        //Store Command
        rules.add({ go, Store }, { mark_mem_location, Send_register , Turing::LEFT });
        rules.add({ go, Send_register }, { store, Reset , Turing::RIGHT });

        //End Command
        rules.add({ go, End }, { go, End , Turing::RIGHT });// allowing go to pass the core will mean that go hits PC and the program will end

        //Define store
        rules.add({ store, REGISTER }, { store, REGISTER , Turing::RIGHT });//Skip
        rules.add({ store, Zero }, { store, Zero , Turing::RIGHT });
        rules.add({ store, One }, { store, One , Turing::RIGHT });

        rules.add({ store, ACTIVE_REGISTER }, { read_Q, REGISTER , Turing::RIGHT });//Read Memory location (will transform to r-write)

        //Define mark memory location
        rules.add({ mark_mem_location, Blank }, { mark_mem_location, Blank , Turing::LEFT });//Skip back
        rules.add({ mark_mem_location, Zero }, { mark_mem_location, Zero , Turing::LEFT });
        rules.add({ mark_mem_location, One }, { mark_mem_location, One , Turing::LEFT });

        rules.add({ mark_mem_location, Start }, { read_Q, Start , Turing::RIGHT });// Read Memory location (will tranform to r-write)

        //Define R write (1st call. prepare for write)
        rules.add(R_Write_Segments_full, Zero, R_Write_Segments_full, Zero, Turing::RIGHT); //Skip 
        rules.add(R_Write_Segments_full, One, R_Write_Segments_full, One, Turing::RIGHT);
        rules.add(R_Write_Segments_full, REGISTER, R_Write_Segments_full, REGISTER, Turing::RIGHT);
        rules.add(R_Write_Segments_full, ACTIVE_REGISTER, R_Write_Segments_full, ACTIVE_REGISTER, Turing::RIGHT);
        rules.add(R_Write_Segments_full, PC, R_Write_Segments_full, PC, Turing::RIGHT);

        rules.add({ R1111, LOC }, { R1110, PASS , Turing::RIGHT }); //count down
        rules.add({ R1110, LOC }, { R1101, PASS , Turing::RIGHT });
        rules.add({ R1101, LOC }, { R1100, PASS , Turing::RIGHT });
        rules.add({ R1100, LOC }, { R1011, PASS , Turing::RIGHT });
        rules.add({ R1011, LOC }, { R1010, PASS , Turing::RIGHT });
        rules.add({ R1010, LOC }, { R1001, PASS , Turing::RIGHT });
        rules.add({ R1001, LOC }, { R1000, PASS , Turing::RIGHT });
        rules.add({ R1000, LOC }, { R0111, PASS , Turing::RIGHT });
        rules.add({ R0111, LOC }, { R0110, PASS , Turing::RIGHT });
        rules.add({ R0110, LOC }, { R0101, PASS , Turing::RIGHT });
        rules.add({ R0101, LOC }, { R0100, PASS , Turing::RIGHT });
        rules.add({ R0100, LOC }, { R0011, PASS , Turing::RIGHT });
        rules.add({ R0011, LOC }, { R0010, PASS , Turing::RIGHT });
        rules.add({ R0010, LOC }, { R0001, PASS , Turing::RIGHT });
        rules.add({ R0001, LOC }, { R0000, PASS , Turing::RIGHT });

        rules.add({ R0000, LOC }, { return_to_start, MARK , Turing::LEFT }); //reached destination (now prepared for write)

        rules.add(R_Write_Segments_full, PASS, R_Write_Segments_full, LOC, Turing::RIGHT);//Write case (will only work if preparation has occurred)
        rules.add(R_Write_Segments_full, MARK, IL_Write_Segments_full, LOC, Turing::LEFT);


        //Define load
        rules.add({ load, Blank }, { load, Blank , Turing::LEFT });//Skip back
        rules.add({ load, Zero }, { load, Zero , Turing::LEFT });
        rules.add({ load, One }, { load, One , Turing::LEFT });

        rules.add({ load, Start }, { read_Q, Start , Turing::RIGHT });//Read memory location (Will be transformed into R-write command)

        //Define clear register
        rules.add({ clear_register, Zero }, { clear_register, Zero, Turing::RIGHT }); // Skip to active register
        rules.add({ clear_register, One }, { clear_register, One, Turing::RIGHT });
        rules.add({ clear_register, REGISTER }, { clear_register, REGISTER, Turing::RIGHT });

        rules.add({ clear_register, ACTIVE_REGISTER }, { clear, REGISTER, Turing::RIGHT });//clear
        rules.add({ clear, One }, { clear, Blank, Turing::RIGHT });
        rules.add({ clear, Zero }, { clear, Blank, Turing::RIGHT });

        rules.add({ clear, REGISTER }, { return_to_start, REGISTER, Turing::LEFT });//Until
        rules.add({ clear, PC }, { return_to_start, PC, Turing::LEFT });

        //Define Read read_2bits
        rules.add({ read_2bits, Blank }, { read_2bits, Blank , Turing::LEFT });
        rules.add({ read_2bits, Zero }, { RX0, Blank , Turing::LEFT });//Read
        rules.add({ read_2bits, One }, { RX1, Blank , Turing::LEFT });

        rules.add({ RX0, Zero }, { R00, Blank , Turing::LEFT });
        rules.add({ RX0, One }, { R10, Blank , Turing::LEFT });
        rules.add({ RX1, Zero }, { R01, Blank , Turing::LEFT });
        rules.add({ RX1, One }, { R11, Blank , Turing::LEFT });

        rules.add({ R00, Waiting }, { return_to_start, Error , Turing::LEFT });// Turing into command
        rules.add({ R10, Waiting }, { return_to_start, Load , Turing::LEFT });
        rules.add({ R01, Waiting }, { return_to_start, Store , Turing::LEFT });
        rules.add({ R11, Waiting }, { return_to_start, End , Turing::LEFT });

        rules.add(Register_Segments_full, Zero, Register_Segments_full, Zero, Turing::RIGHT); //or Skip 
        rules.add(Register_Segments_full, One, Register_Segments_full, One, Turing::RIGHT);
        rules.add(Register_Segments_full, Blank, Register_Segments_full, Blank, Turing::RIGHT);
        rules.add(Register_Segments_full, Read_command, Register_Segments_full, Read_command, Turing::RIGHT);

        rules.add({ R10, REGISTER }, { R01, REGISTER , Turing::RIGHT });//Count Down
        rules.add({ R01, REGISTER }, { R00, REGISTER , Turing::RIGHT });
        rules.add({ R11, REGISTER }, { R10, REGISTER , Turing::RIGHT });

        rules.add({ R00, REGISTER }, { return_to_start, ACTIVE_REGISTER , Turing::LEFT });//Mark register

        //Define go
        rules.add({ go, Zero }, { go, Zero , Turing::RIGHT });//skip
        rules.add({ go, One }, { go, One , Turing::RIGHT });
        rules.add({ go, Blank }, { go, Blank , Turing::RIGHT });

        rules.add({ go, REGISTER }, { go, REGISTER , Turing::RIGHT });
        rules.add({ go, ACTIVE_REGISTER }, { go, ACTIVE_REGISTER , Turing::RIGHT });
        rules.add({ go, PC }, { end, PC , Turing::RIGHT });//End Case

        //Define return to start
        rules.add({ return_to_start, Get_memory }, { return_to_start, Get_memory, Turing::LEFT }); //skip
        rules.add({ return_to_start, Prep_register }, { return_to_start, Prep_register, Turing::LEFT });
        rules.add({ return_to_start, Read_command }, { return_to_start, Read_command, Turing::LEFT });
        rules.add({ return_to_start, Load_from_mem }, { return_to_start, Load_from_mem, Turing::LEFT });
        rules.add({ return_to_start, Send_register }, { return_to_start, Send_register, Turing::LEFT });
        rules.add({ return_to_start, Reset }, { return_to_start, Reset, Turing::LEFT });
        rules.add({ return_to_start, Zero }, { return_to_start, Zero , Turing::LEFT });
        rules.add({ return_to_start, One }, { return_to_start, One , Turing::LEFT });
        rules.add({ return_to_start, PC }, { return_to_start, PC , Turing::LEFT });
        rules.add({ return_to_start, Blank }, { return_to_start, Blank , Turing::LEFT });
        rules.add({ return_to_start, REGISTER }, { return_to_start, REGISTER , Turing::LEFT });
        rules.add({ return_to_start, ACTIVE_REGISTER }, { return_to_start, ACTIVE_REGISTER , Turing::LEFT });
        rules.add({ return_to_start, PASS }, { return_to_start, PASS , Turing::LEFT });
        rules.add({ return_to_start, MARK }, { return_to_start, MARK , Turing::LEFT });
        rules.add({ return_to_start, LOC }, { return_to_start, LOC , Turing::LEFT });

        rules.add({ return_to_start, Start }, { go, Start , Turing::RIGHT });//Done 

        //Define get
        rules.add({ get, Zero }, { get, Zero , Turing::RIGHT }); //Skip 
        rules.add({ get, One }, { get, One , Turing::RIGHT });
        rules.add({ get, REGISTER }, { get, REGISTER , Turing::RIGHT });

        rules.add({ get, PC }, { read_Q, PC , Turing::RIGHT });//Read PC and return result stored at that location (see read_Q)

        //Define reset
        rules.add({ reset, Zero }, { reset, Blank , Turing::LEFT }); //Set Core Registers to Blank
        rules.add({ reset, One }, { reset, Blank , Turing::LEFT });
        rules.add({ reset, Blank }, { reset, Blank , Turing::LEFT });
        rules.add({ reset, Start }, { go, Start , Turing::RIGHT }); //Until start

        //Define Q-Read ~ read 4 bits (overload. then go to memory location specified and read the 4 bits there & then return the 4-bits read)
        rules.add({ read_Q, One }, { Q1XXX, One , Turing::RIGHT });
        rules.add({ read_Q, Zero }, { Q0XXX, Zero , Turing::RIGHT });

        rules.add({ Q1XXX, One }, { Q11XX, One , Turing::RIGHT });
        rules.add({ Q1XXX, Zero }, { Q10XX, Zero , Turing::RIGHT });
        rules.add({ Q0XXX, One }, { Q01XX, One , Turing::RIGHT });
        rules.add({ Q0XXX, Zero }, { Q00XX, Zero , Turing::RIGHT });


        rules.add({ Q11XX, One }, { Q111X, One , Turing::RIGHT });
        rules.add({ Q11XX, Zero }, { Q110X, Zero , Turing::RIGHT });
        rules.add({ Q10XX, One }, { Q101X, One , Turing::RIGHT });
        rules.add({ Q10XX, Zero }, { Q100X, Zero , Turing::RIGHT });
        rules.add({ Q01XX, One }, { Q011X, One , Turing::RIGHT });
        rules.add({ Q01XX, Zero }, { Q010X, Zero , Turing::RIGHT });
        rules.add({ Q00XX, One }, { Q001X, One , Turing::RIGHT });
        rules.add({ Q00XX, Zero }, { Q000X, Zero , Turing::RIGHT });

        rules.add({ Q111X, One }, { Q1111, One , Turing::RIGHT });
        rules.add({ Q111X, Zero }, { Q1110, Zero , Turing::RIGHT });
        rules.add({ Q110X, One }, { Q1101, One , Turing::RIGHT });
        rules.add({ Q110X, Zero }, { Q1100, Zero , Turing::RIGHT });
        rules.add({ Q101X, One }, { Q1011, One , Turing::RIGHT });
        rules.add({ Q101X, Zero }, { Q1010, Zero , Turing::RIGHT });
        rules.add({ Q100X, One }, { Q1001, One , Turing::RIGHT });
        rules.add({ Q100X, Zero }, { Q1000, Zero , Turing::RIGHT });
        rules.add({ Q011X, One }, { Q0111, One , Turing::RIGHT });
        rules.add({ Q011X, Zero }, { Q0110, Zero , Turing::RIGHT });
        rules.add({ Q010X, One }, { Q0101, One , Turing::RIGHT });
        rules.add({ Q010X, Zero }, { Q0100, Zero , Turing::RIGHT });
        rules.add({ Q001X, One }, { Q0011, One , Turing::RIGHT });
        rules.add({ Q001X, Zero }, { Q0010, Zero , Turing::RIGHT });
        rules.add({ Q000X, One }, { Q0001, One , Turing::RIGHT });
        rules.add({ Q000X, Zero }, { Q0000, Zero , Turing::RIGHT });

        rules.add(Q_Segments_full, Zero, Q_Segments_full, Zero, Turing::RIGHT); //Skip 
        rules.add(Q_Segments_full, One, Q_Segments_full, One, Turing::RIGHT);
        rules.add(Q_Segments_full, Reset, Q_Segments_full, Reset, Turing::RIGHT);
        rules.add(Q_Segments_full, REGISTER, Q_Segments_full, REGISTER, Turing::RIGHT);
        rules.add(Q_Segments_full, PC, Q_Segments_full, PC, Turing::RIGHT);
        rules.add(Q_Segments_full, Blank, Q_Segments_full, Blank, Turing::RIGHT);

        rules.add(Q_Segments_full, Send_register, R_Write_Segments_full, Send_register, Turing::RIGHT);//special cases transform into r-write command
        rules.add(Q_Segments_full, PASS, R_Write_Segments_full, PASS, Turing::STAY);
        rules.add(Q_Segments_full, MARK, R_Write_Segments_full, MARK, Turing::STAY);

        rules.add({ Q1111, LOC }, { Q1110, LOC , Turing::RIGHT }); //count down
        rules.add({ Q1110, LOC }, { Q1101, LOC , Turing::RIGHT });
        rules.add({ Q1101, LOC }, { Q1100, LOC , Turing::RIGHT });
        rules.add({ Q1100, LOC }, { Q1011, LOC , Turing::RIGHT });
        rules.add({ Q1011, LOC }, { Q1010, LOC , Turing::RIGHT });
        rules.add({ Q1010, LOC }, { Q1001, LOC , Turing::RIGHT });
        rules.add({ Q1001, LOC }, { Q1000, LOC , Turing::RIGHT });
        rules.add({ Q1000, LOC }, { Q0111, LOC , Turing::RIGHT });
        rules.add({ Q0111, LOC }, { Q0110, LOC , Turing::RIGHT });
        rules.add({ Q0110, LOC }, { Q0101, LOC , Turing::RIGHT });
        rules.add({ Q0101, LOC }, { Q0100, LOC , Turing::RIGHT });
        rules.add({ Q0100, LOC }, { Q0011, LOC , Turing::RIGHT });
        rules.add({ Q0011, LOC }, { Q0010, LOC , Turing::RIGHT });
        rules.add({ Q0010, LOC }, { Q0001, LOC , Turing::RIGHT });
        rules.add({ Q0001, LOC }, { Q0000, LOC , Turing::RIGHT });

        rules.add({ Q0000, LOC }, { last_LOC, SEND_BACK , Turing::LEFT }); //reached destination

        rules.add({ last_LOC, Zero }, { last_LOC, Zero , Turing::LEFT }); //Skip back to last LOC (or PC)
        rules.add({ last_LOC, One }, { last_LOC, One , Turing::LEFT });
        rules.add({ last_LOC, LOC }, { read_Q, LOC , Turing::RIGHT }); //Read contents
        rules.add({ last_LOC, PC }, { read_Q, PC , Turing::RIGHT }); //Read contents
        rules.add(Q_Segments_full, SEND_BACK, L_Write_Segments_full, LOC, Turing::LEFT); // return and write


        //L_Write ~ Move LEFT until blank space is found then write 
        rules.add(L_Write_Segments_full, Zero, L_Write_Segments_full, Zero, Turing::LEFT); //Skip back Until a Blank is found
        rules.add(L_Write_Segments_full, One, L_Write_Segments_full, One, Turing::LEFT);
        rules.add(L_Write_Segments_full, LOC, L_Write_Segments_full, LOC, Turing::LEFT);
        rules.add(L_Write_Segments_full, REGISTER, L_Write_Segments_full, REGISTER, Turing::LEFT);
        rules.add(L_Write_Segments_full, PC, L_Write_Segments_full, PC, Turing::LEFT);
        rules.add(L_Write_Segments_full, PC, L_Write_Segments_full, PC, Turing::LEFT);
        rules.add(L_Write_Segments_full, PC_inc1, L_Write_Segments_full, PC_inc1, Turing::LEFT);
        rules.add(L_Write_Segments_full, PC_inc2, L_Write_Segments_full, PC_inc2, Turing::LEFT);


        rules.add({ L1111, Blank }, { L111X, One , Turing::LEFT });//Write
        rules.add({ L1110, Blank }, { L111X, Zero , Turing::LEFT });
        rules.add({ L1101, Blank }, { L110X, One , Turing::LEFT });
        rules.add({ L1100, Blank }, { L110X, Zero , Turing::LEFT });
        rules.add({ L1011, Blank }, { L101X, One , Turing::LEFT });
        rules.add({ L1010, Blank }, { L101X, Zero , Turing::LEFT });
        rules.add({ L1001, Blank }, { L100X, One , Turing::LEFT });
        rules.add({ L1000, Blank }, { L100X, Zero , Turing::LEFT });
        rules.add({ L0111, Blank }, { L011X, One , Turing::LEFT });
        rules.add({ L0110, Blank }, { L011X, Zero , Turing::LEFT });
        rules.add({ L0101, Blank }, { L010X, One , Turing::LEFT });
        rules.add({ L0100, Blank }, { L010X, Zero , Turing::LEFT });
        rules.add({ L0011, Blank }, { L001X, One , Turing::LEFT });
        rules.add({ L0010, Blank }, { L001X, Zero , Turing::LEFT });
        rules.add({ L0001, Blank }, { L000X, One , Turing::LEFT });
        rules.add({ L0000, Blank }, { L000X, Zero , Turing::LEFT });

        rules.add({ L111X, Blank }, { L11XX, One , Turing::LEFT });
        rules.add({ L110X, Blank }, { L11XX, Zero , Turing::LEFT });
        rules.add({ L101X, Blank }, { L10XX, One , Turing::LEFT });
        rules.add({ L100X, Blank }, { L10XX, Zero , Turing::LEFT });
        rules.add({ L011X, Blank }, { L01XX, One , Turing::LEFT });
        rules.add({ L010X, Blank }, { L01XX, Zero , Turing::LEFT });
        rules.add({ L001X, Blank }, { L00XX, One , Turing::LEFT });
        rules.add({ L000X, Blank }, { L00XX, Zero , Turing::LEFT });

        rules.add({ L11XX, Blank }, { L1XXX, One , Turing::LEFT });
        rules.add({ L10XX, Blank }, { L1XXX, Zero , Turing::LEFT });
        rules.add({ L01XX, Blank }, { L0XXX, One , Turing::LEFT });
        rules.add({ L00XX, Blank }, { L0XXX, Zero , Turing::LEFT });

        rules.add({ L1XXX, Blank }, { return_to_start, One , Turing::LEFT });
        rules.add({ L0XXX, Blank }, { return_to_start, Zero , Turing::LEFT }); //now return to go

        //Define Instant Write 

        rules.add({ IL1111, One }, { IL111X, One , Turing::LEFT });//Write ("one" case)
        rules.add({ IL1110, One }, { IL111X, Zero , Turing::LEFT });
        rules.add({ IL1101, One }, { IL110X, One , Turing::LEFT });
        rules.add({ IL1100, One }, { IL110X, Zero , Turing::LEFT });
        rules.add({ IL1011, One }, { IL101X, One , Turing::LEFT });
        rules.add({ IL1010, One }, { IL101X, Zero , Turing::LEFT });
        rules.add({ IL1001, One }, { IL100X, One , Turing::LEFT });
        rules.add({ IL1000, One }, { IL100X, Zero , Turing::LEFT });
        rules.add({ IL0111, One }, { IL011X, One , Turing::LEFT });
        rules.add({ IL0110, One }, { IL011X, Zero , Turing::LEFT });
        rules.add({ IL0101, One }, { IL010X, One , Turing::LEFT });
        rules.add({ IL0100, One }, { IL010X, Zero , Turing::LEFT });
        rules.add({ IL0011, One }, { IL001X, One , Turing::LEFT });
        rules.add({ IL0010, One }, { IL001X, Zero , Turing::LEFT });
        rules.add({ IL0001, One }, { IL000X, One , Turing::LEFT });
        rules.add({ IL0000, One }, { IL000X, Zero , Turing::LEFT });

        rules.add({ IL111X, One }, { IL11XX, One , Turing::LEFT });
        rules.add({ IL110X, One }, { IL11XX, Zero , Turing::LEFT });
        rules.add({ IL101X, One }, { IL10XX, One , Turing::LEFT });
        rules.add({ IL100X, One }, { IL10XX, Zero , Turing::LEFT });
        rules.add({ IL011X, One }, { IL01XX, One , Turing::LEFT });
        rules.add({ IL010X, One }, { IL01XX, Zero , Turing::LEFT });
        rules.add({ IL001X, One }, { IL00XX, One , Turing::LEFT });
        rules.add({ IL000X, One }, { IL00XX, Zero , Turing::LEFT });

        rules.add({ IL11XX, One }, { IL1XXX, One , Turing::LEFT });
        rules.add({ IL10XX, One }, { IL1XXX, Zero , Turing::LEFT });
        rules.add({ IL01XX, One }, { IL0XXX, One , Turing::LEFT });
        rules.add({ IL00XX, One }, { IL0XXX, Zero , Turing::LEFT });

        rules.add({ IL1XXX, One }, { return_to_start, One , Turing::LEFT });
        rules.add({ IL0XXX, One }, { return_to_start, Zero , Turing::LEFT }); //now return to go

        rules.add({ IL1111, Zero }, { IL111X, One , Turing::LEFT });//Write ("zero" case)
        rules.add({ IL1110, Zero }, { IL111X, Zero , Turing::LEFT });
        rules.add({ IL1101, Zero }, { IL110X, One , Turing::LEFT });
        rules.add({ IL1100, Zero }, { IL110X, Zero , Turing::LEFT });
        rules.add({ IL1011, Zero }, { IL101X, One , Turing::LEFT });
        rules.add({ IL1010, Zero }, { IL101X, Zero , Turing::LEFT });
        rules.add({ IL1001, Zero }, { IL100X, One , Turing::LEFT });
        rules.add({ IL1000, Zero }, { IL100X, Zero , Turing::LEFT });
        rules.add({ IL0111, Zero }, { IL011X, One , Turing::LEFT });
        rules.add({ IL0110, Zero }, { IL011X, Zero , Turing::LEFT });
        rules.add({ IL0101, Zero }, { IL010X, One , Turing::LEFT });
        rules.add({ IL0100, Zero }, { IL010X, Zero , Turing::LEFT });
        rules.add({ IL0011, Zero }, { IL001X, One , Turing::LEFT });
        rules.add({ IL0010, Zero }, { IL001X, Zero , Turing::LEFT });
        rules.add({ IL0001, Zero }, { IL000X, One , Turing::LEFT });
        rules.add({ IL0000, Zero }, { IL000X, Zero , Turing::LEFT });

        rules.add({ IL111X, Zero }, { IL11XX, One , Turing::LEFT });
        rules.add({ IL110X, Zero }, { IL11XX, Zero , Turing::LEFT });
        rules.add({ IL101X, Zero }, { IL10XX, One , Turing::LEFT });
        rules.add({ IL100X, Zero }, { IL10XX, Zero , Turing::LEFT });
        rules.add({ IL011X, Zero }, { IL01XX, One , Turing::LEFT });
        rules.add({ IL010X, Zero }, { IL01XX, Zero , Turing::LEFT });
        rules.add({ IL001X, Zero }, { IL00XX, One , Turing::LEFT });
        rules.add({ IL000X, Zero }, { IL00XX, Zero , Turing::LEFT });

        rules.add({ IL11XX, Zero }, { IL1XXX, One , Turing::LEFT });
        rules.add({ IL10XX, Zero }, { IL1XXX, Zero , Turing::LEFT });
        rules.add({ IL01XX, Zero }, { IL0XXX, One , Turing::LEFT });
        rules.add({ IL00XX, Zero }, { IL0XXX, Zero , Turing::LEFT });

        rules.add({ IL1XXX, Zero }, { return_to_start, One , Turing::LEFT });
        rules.add({ IL0XXX, Zero }, { return_to_start, Zero , Turing::LEFT }); //now return to start

        //Define a PC increment by 1
        rules.add({ pc_inc, Zero }, { pc_inc, Zero , Turing::RIGHT });//skip to PC reg
        rules.add({ pc_inc, One }, { pc_inc, One , Turing::RIGHT });
        rules.add({ pc_inc, REGISTER }, { pc_inc, REGISTER , Turing::RIGHT });
        rules.add({ pc_inc, PC }, { pc_inc, PC , Turing::RIGHT });
        rules.add({ pc_inc, LOC }, { pc_read, PC_INC , Turing::LEFT });

        rules.add({ pc_read, Zero }, { pc_read, Zero , Turing::LEFT });// go back and then read PC register 
        rules.add({ pc_read, One }, { pc_read, One , Turing::LEFT });
        rules.add({ pc_read, PC }, { read_Q, PC , Turing::RIGHT });

        rules.add({ Q1111, PC_INC }, { pc_8bit_instruction_overflow, PC_WRITE , Turing::STAY }); //count up
        rules.add({ Q1110, PC_INC }, { Q1111, PC_WRITE , Turing::STAY });
        rules.add({ Q1101, PC_INC }, { Q1110, PC_WRITE , Turing::STAY });
        rules.add({ Q1100, PC_INC }, { Q1101, PC_WRITE , Turing::STAY });
        rules.add({ Q1011, PC_INC }, { Q1100, PC_WRITE , Turing::STAY });
        rules.add({ Q1010, PC_INC }, { Q1011, PC_WRITE , Turing::STAY });
        rules.add({ Q1001, PC_INC }, { Q1010, PC_WRITE , Turing::STAY });
        rules.add({ Q1000, PC_INC }, { Q1001, PC_WRITE , Turing::STAY });
        rules.add({ Q0111, PC_INC }, { Q1000, PC_WRITE , Turing::STAY });
        rules.add({ Q0110, PC_INC }, { Q0111, PC_WRITE , Turing::STAY });
        rules.add({ Q0101, PC_INC }, { Q0110, PC_WRITE , Turing::STAY });
        rules.add({ Q0100, PC_INC }, { Q0101, PC_WRITE , Turing::STAY });
        rules.add({ Q0011, PC_INC }, { Q0100, PC_WRITE , Turing::STAY });
        rules.add({ Q0010, PC_INC }, { Q0011, PC_WRITE , Turing::STAY });
        rules.add({ Q0001, PC_INC }, { Q0010, PC_WRITE , Turing::STAY });
        rules.add({ Q0000, PC_INC }, { Q0001, PC_WRITE , Turing::STAY });

        rules.add(Q_Segments_full, PC_WRITE, IL_Write_Segments_full, LOC, Turing::LEFT);//Instant write
    }

    symbols encode(int input)
    {
        switch (input)
        {
        case 0:
            return symbols::Zero;
        case 1:
            return symbols::One;
        default:
            throw std::runtime_error("Program should be in binary");
        }
    }

    int decode(symbols input)
    {
        switch (input)
        {
        case symbols::Zero:
            return 0;
        case symbols::One:
            return 1;
        default:
            throw std::runtime_error("Non Binary Code");
        }
    }

    inline void run(std::vector<int>& program)
    {


        // 4-bit "Language" Specification
        // 
        // Each command is given in 8 (2 by 4) bits:
        // 
        // first 2-Bits - Indicates what command is this 
        // 
        // 11 -> End program 
        // 01 -> Store
        // 10 -> Load
        // 00 -> Unused (currently produces Error)
        // 
        // second 2-bits - Indicates what register we are referring to
        // 
        // The Next 4 bits will indicate what memory location we are referring to 
        //

        std::vector<size_t> input
        {
            //Core
            Start,
            Zero,Zero,Zero,Zero, //Memory register    
            Zero,Zero,Zero,Zero, //Command register
            Reset,

            //Registers
            REGISTER,
            Zero,Zero,Zero,Zero, //r1
            REGISTER,
            Zero,Zero,Zero,Zero, //r2
            REGISTER,
            Zero,Zero,Zero,Zero, //r3
            REGISTER,
            Zero,Zero,Zero,Zero, //r4

            PC,//#0000 //Also Acts as the Program Counter
            encode(program[0]),encode(program[1]),encode(program[2]),encode(program[3]),

            LOC,//#0001
            encode(program[4]),encode(program[5]),encode(program[6]),encode(program[7]),

            LOC,//#0010
            encode(program[8]),encode(program[9]),encode(program[10]),encode(program[11]),

            LOC,//#0011
            encode(program[12]),encode(program[13]),encode(program[14]),encode(program[15]),

            LOC,//#0100
            encode(program[16]),encode(program[17]),encode(program[18]),encode(program[19]),

            LOC,//#0101
            encode(program[20]),encode(program[21]),encode(program[22]),encode(program[23]),

            LOC,//#0110
            encode(program[24]),encode(program[25]),encode(program[26]),encode(program[27]),

            LOC,//#0111
            encode(program[28]),encode(program[29]),encode(program[30]),encode(program[31]),

            LOC,//#1000
            encode(program[32]),encode(program[33]),encode(program[34]),encode(program[35]),

            LOC,//#1001
            encode(program[36]),encode(program[37]),encode(program[38]),encode(program[39]),

            LOC,//#1010
            encode(program[40]),encode(program[41]),encode(program[42]),encode(program[43]),

            LOC,//#1011
            encode(program[44]),encode(program[45]),encode(program[46]),encode(program[47]),

            LOC,//#1100
            encode(program[48]),encode(program[49]),encode(program[50]),encode(program[51]),

            LOC,//#1101
            encode(program[52]),encode(program[53]),encode(program[54]),encode(program[55]),

            LOC,//#1110
            encode(program[56]),encode(program[57]),encode(program[58]),encode(program[59]),

            LOC,//#1111
            encode(program[60]),encode(program[61]),encode(program[62]),encode(program[63]),

            LOC,

        };


        Turing::Rules rules{};
        add_ruleset(rules);
        Turing::Turing_Machine machine(rules,go); //Create a Machine with that uses the rule set defined above and starts with state go

        machine.boot_up(input);
        machine.run();

        int contents{};
        int reg_count{ 1 };
        for (size_t code : *machine.get_tape())//Print contents of the registers
        {
            symbols item{static_cast<symbols>(code) };

            if (item == REGISTER or item == ACTIVE_REGISTER)
            {
                std::cout << "\n" << "REGISTER" << reg_count << " -> ";
                contents = contents + 4;
                ++reg_count;
            }
            else if (contents > 0)
            {
                std::cout << decode(item);
                --contents;
            }
        }
    }
}