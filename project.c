// Kevin Zheng, Cassandra Morales, Matthew Arroyave
// Group Project
// CDA 3103
// 4/21/24
// Dr. Ozdag

#include "spimcore.h"

/* ALU */
/* 10 Points */
// Implement the operations on input parameters A and B according to ALU controls
void ALU(unsigned A,unsigned B,char ALUControl,unsigned *ALUresult,char *Zero)
{
    if (ALUControl == 0x0)
    {
        *ALUresult = A + B;
    }
    else if (ALUControl == 0x1)
    {
        *ALUresult = A - B;
    }
    else if (ALUControl == 0x2)
    {
        if ((int)A < (int)B)
        {
            *ALUresult = 1;
        }
        else
        {
            *ALUresult = 0;
        }
    }
    else if (ALUControl == 0x3)
    {
        if (A < B)
        {
            *ALUresult = 1;
        }
        else
        {
            *ALUresult = 0;
        }
    }
    else if (ALUControl == 0x4)
    {
        *ALUresult = A & B;
    }
    else if (ALUControl == 0x5)
    {
        *ALUresult = A | B;
    }
    else if (ALUControl == 0x6)
    {
        *ALUresult = B << 16;
    }
    else if (ALUControl == 0x7)
    {
        *ALUresult = ~A;
    }
    
    if (*ALUresult == 0)
    {
        *Zero = 1;
    }
    else
    {
        *Zero = 0;
    }
}

/* instruction fetch */
/* 10 Points */
// grabs info from PC and writes it to instruction
int instruction_fetch(unsigned PC,unsigned *Mem,unsigned *instruction)
{
// checks if PC is 4-byte
   if(PC % 4 != 0)
    {
        return 1;
    }
    else
    {
// shifts the PC by 2 bits
        *instruction = Mem[PC >> 2];
        return 0;
    }
}


/* instruction partition */
/* 10 Points */
// masks bits and shifts them into position
void instruction_partition(unsigned instruction, unsigned *op, unsigned *r1,unsigned *r2, unsigned *r3, unsigned *funct, unsigned *offset, unsigned *jsec)
{
// shift by 26 bits
   *op = (instruction & 0xfc000000) >> 26;

// shift by 21 bits
    *r1 = (instruction & 0x03e00000) >> 21;

// shift by 16 bits
    *r2 = (instruction & 0x001f0000) >> 16;

// shift by 11
    *r3 = (instruction & 0x0000f800) >> 11;

// masking
    *funct = instruction & 0x0000003f;
	
// masking
    *offset = instruction & 0x0000ffff;

// masking
    *jsec = instruction & 0x03ffffff;
}



/* instruction decode */
/* 15 Points */
// assigns control signals to variables
int instruction_decode(unsigned op,struct_controls *controls)
{
// starts all the controls at 0
   	controls->RegDst = 0;
	controls->Jump = 0;
	controls->Branch = 0;
	controls->MemRead = 0;
	controls->MemtoReg = 0;
	controls->ALUOp = 0;
	controls->MemWrite = 0;
	controls->ALUSrc = 0;
	controls->RegWrite = 0;

// changes control values according to op value
    if (op == 0x0)
    {
        controls->RegDst = 1;
        controls->ALUOp = 7;
        controls->RegWrite = 1;
    }

    else if (op == 0x8)
    {    
        controls->RegWrite = 1;
        controls->ALUSrc = 1;
    }
	
    else if (op == 0x23)
    {
        controls->RegWrite = 1;
        controls->MemRead = 1;
        controls->MemtoReg = 1;
        controls->ALUSrc = 1;
    }

    else if (op == 0x2b)
    {
        controls->MemWrite = 1;
        controls->RegDst = 2;
        controls->MemtoReg = 2; 
        controls->ALUSrc = 1;
    }

    else if(op == 0xf)
    {
        controls->RegWrite = 1;
        controls->ALUOp = 6;
        controls->ALUSrc = 1;
    }

    else if (op == 0x4)
    {
        controls->Branch = 1;
        controls->RegDst = 2;
        controls->MemtoReg = 2;
        controls->ALUSrc = 1; 
        controls->ALUOp = 1;
    }

    else if (op == 0xa)
    {
        controls->ALUOp = 2;
        controls->RegWrite = 1;
        controls->ALUSrc = 1;
    }

    else if (op == 0xb)
    {
        controls->ALUOp = 3;
        controls->RegWrite = 1;
        controls->ALUSrc = 1;
    }

    else if (op == 0x2)
    {
        controls->Jump = 1;
        controls->RegDst = 2;
        controls->Branch = 2;
        controls->MemtoReg = 2;
        controls->ALUSrc = 2;
        controls->ALUOp = 2;
    }
	    
// when none of the cases fit
    else
    {
        return 1;
    }
  
return 0;
}

/* Read Register */
/* 5 Points */
// moves the data from r1/r2 into data1/data2
void read_register(unsigned r1,unsigned r2,unsigned *Reg,unsigned *data1,unsigned *data2)
{
    *data1 = Reg[r1];
    *data2 = Reg[r2];
}

/* Sign Extend */
/* 10 Points */
// extends the ones value to 32 bit
void sign_extend(unsigned offset,unsigned *extended_value)
{
    int ones = 0xffff << 16;
    *extended_value = (offset&1<<15?offset|ones:offset);
}

/* ALU operations */
/* 10 Points */
// Adds the operations of an ALU
int ALU_operations(unsigned data1,unsigned data2,unsigned extended_value,unsigned funct,char ALUOp,char ALUSrc,unsigned *ALUresult,char *Zero)
{
// makes data2 a 32 bit value
    if(ALUSrc == 1) data2 = extended_value;
	
// changes ALUOp value if it is equal to 7
    if(ALUOp == 7)
    {
// reassigns ALUOp value based on funct value
	if (funct == 4)
	{
		ALUOp = 6;
	}
		
	else if (funct == 32)
	{
		ALUOp = 0;
	}
		
	else if (funct == 34)
	{
		ALUOp = 1;
	}
		
        else if (funct == 36)
	{
		ALUOp = 4;
	}
		
        else if (funct == 37)
	{
		ALUOp = 5;
	}
		
	else if (funct == 39)
	{
		ALUOp = 7;
	}
		
        else if (funct == 42)
	{
		ALUOp = 2;
	}
		
	else if (funct == 43)
	{
		ALUOp = 3;
	}
		
        else
	{
                return 1;
        }
    }	    
// executing function  
    else ALU(data1, data2, ALUOp, ALUresult, Zero);

    return 0;

}

/* Read / Write Memory */
/* 10 Points */
// determines whether a memwrite or a memread operation is occurring
    int rw_memory(unsigned ALUresult,unsigned data2,char MemWrite,char MemRead,unsigned *memdata,unsigned *Mem)
    {
    // 
    if (MemWrite == 1)
    {
        if((ALUresult % 4) == 0) 
            Mem[ALUresult >> 2] = data2; 
        else return 1;
    }

    if (MemRead == 1)
    {
        if ((ALUresult % 4) == 0) 
            *memdata = Mem[ALUresult >> 2]; 
        else return 1;
    }

    return 0;
    }


/* Write Register */
/* 10 Points */
// determines what will be written into the r2 and r3 registers 
void write_register(unsigned r2,unsigned r3,unsigned memdata,unsigned ALUresult,char RegWrite,char RegDst,char MemtoReg,unsigned *Reg)
{
// conditions to determin whether to write data to register
    if (MemtoReg == 1 && RegDst == 0 && RegWrite == 1)
    {
// if conditions are met, data is written into register
            Reg[r2] = memdata;
    }
    else if(MemtoReg == 0 && RegDst == 1 &&  RegWrite == 1)
    {
// if conditions are not met, then the ALUresult is written into the register
            Reg[r3] = ALUresult;
    }
  
}

/* PC update */
/* 10 Points */
// updates the counter based on the control signals 
void PC_update(unsigned jsec,unsigned extended_value,char Branch,char Jump,char Zero,unsigned *PC)
{
// icrements the counter by 4
    *PC += 4;
// if there is a jump instruction, the address will be shifted by 2 to the left to align with sig bits
    if (Jump == 1) *PC = (jsec << 2) | (*PC & 0xf0000000);
// shifts the extended_value left by 2 
    if (Zero == 1 && Branch == 1) *PC += extended_value << 2;
}
