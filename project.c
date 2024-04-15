// Kevin Zheng, Cassandra Morales, Matthew Arroyave
// Group Project
// CDA 3103
// 4/21/24
// Dr. Ozdag

#include "spimcore.h"


/* ALU */
/* 10 Points */
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
int instruction_fetch(unsigned PC,unsigned *Mem,unsigned *instruction)
{
   if(PC % 4 != 0)
    {
        return 1;
    }
    else
    {
        *instruction = Mem[PC >> 2];
        return 0;
    }
}


/* instruction partition */
/* 10 Points */
void instruction_partition(unsigned instruction, unsigned *op, unsigned *r1,unsigned *r2, unsigned *r3, unsigned *funct, unsigned *offset, unsigned *jsec)
{
   *op = (instruction & 0xfc000000) >> 26;

  // 0000 0011 1110 0000 0000 0000 0000 0000 --> instruction [25-21]
    *r1 = (instruction & 0x03e00000) >> 21;

  // 0000 0000 0001 1111 0000 0000 0000 0000 --> instruction [20-16]
    *r2 = (instruction & 0x001f0000) >> 16;

  // 0000 0000 0000 0000 1111 1000 0000 0000 --> instruction [15-11]
    *r3 = (instruction & 0x0000f800) >> 11;

  // 0000 0000 0000 0000 0000 0000 0011 1111 --> instruction [5-0]
    *funct = instruction & 0x0000003f;

    *offset = instruction & 0x0000ffff;

  // 0000 0011 1111 1111 1111 1111 1111 1111 --> instruction [25-0]
    *jsec = instruction & 0x03ffffff;
}



/* instruction decode */
/* 15 Points */
int instruction_decode(unsigned op,struct_controls *controls)
{
   controls->RegDst = 0;
	controls->Jump = 0;
	controls->Branch = 0;
	controls->MemRead = 0;
	controls->MemtoReg = 0;
	controls->ALUOp = 0;
	controls->MemWrite = 0;
	controls->ALUSrc = 0;
	controls->RegWrite = 0;

    if (op == 0x0)
    {
        controls->RegDst = 1;
        controls->ALUOp = 7;
        controls->RegWrite = 1;
    }

    // Add immediate --> 0000 1000
    else if (op == 0x8)
    {    
        controls->RegWrite = 1;
        controls->ALUSrc = 1;
    }
    // Load word (lw) --> 0010 0011
    if (op == 0x23)
    {
        controls->RegWrite = 1;
        controls->MemRead = 1;
        controls->MemtoReg = 1;
        controls->ALUSrc = 1;
    }
    // Store word (sw) --> 0010 1011
    else if (op == 0x2b)
    {
        controls->MemWrite = 1;
        controls->RegDst = 2; // ?
        controls->MemtoReg = 2; // ?
        controls->ALUSrc = 1;
    }
    // Load upper immediate (lui) --> 0000 1111
    else if(op == 0xf)
    {
        controls->RegWrite = 1;
      // Requires upper 16 bits, so set ALU operation to shift
        controls->ALUOp = 6;
        controls->ALUSrc = 1;
    }
    // Branch on equal (beq) --> 0000 0100
    else if (op == 0x4)
    {
     // PC updates for branch to multiplexer path 1
        controls->Branch = 1;
        controls->RegDst = 2;
        controls->MemtoReg = 2;
        controls->ALUSrc = 1; // 2? 0?
      // Branching requires subtraction
        controls->ALUOp = 1;
    }

    // Set less than immediate (slti) --> 0000 1010
    else if (op == 0xa)
    {
        // Set ALU operation for 'set less than'
        controls->ALUOp = 2;
        controls->RegWrite = 1;
        controls->ALUSrc = 1;
    }

    // Set less than immediate unsigned (sltiu) --> 0000 1011
    else if (op == 0xb)
    {
         // Set ALU operation for 'set less than unsigned'
        controls->ALUOp = 3;
        controls->RegWrite = 1;
        controls->ALUSrc = 1;
    }

    // Jump (j) --> 0000 0010
    else if (op == 0x2)
    {
        controls->Jump = 1;
        controls->RegDst = 2;
        controls->Branch = 2;
        controls->MemtoReg = 2;
        controls->ALUSrc = 2;
        controls->ALUOp = 2;
    }

    // If none of the cases apply, halt condition occurs, so return 1
    else
    {
        return 1;
    }
  
  // No halt condition = successful encoding
    return 0;
}

/* Read Register */
/* 5 Points */
void read_register(unsigned r1,unsigned r2,unsigned *Reg,unsigned *data1,unsigned *data2)
{
    *data1 = Reg[r1];
    *data2 = Reg[r2];
}


/* Sign Extend */
/* 10 Points */
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

    if(ALUSrc == 1) data2 = extended_value;

    if(ALUOp == 7){

        switch (funct){

            case 4:
                ALUOp = 6;
                break;
            
            case 32:
                ALUOp = 0;
                break;

            case 34:
                ALUOp = 1;
                break; 

            case 36:
                ALUOp = 4;
                break;

            case 37:
                ALUOp = 5;
                break;
            
            case 39:
                ALUOp = 7;
                break;
            
            case 42:
                ALUOp = 2;
                break;
            
            case 43:
                ALUOp = 3;
                break;
            
            default:
                return 1;

        }
        
        ALU(data1, data2, ALUOp, ALUresult, Zero);

    } else ALU(data1, data2, ALUOp, ALUresult, Zero);

    return 0;

}

/* Read / Write Memory */
/* 10 Points */
// determines whether a memwrite or a memread operation is occurring
int rw_memory(unsigned ALUresult,unsigned data2,char MemWrite,char MemRead,unsigned *memdata,unsigned *Mem)
{
// 
if(MemWrite == 1)
{
      if((ALUresult % 4) == 0) 
         Mem[ALUresult >> 2] = data2; 
      else return 1;
}

  if(MemRead == 1)
  {
      if((ALUresult % 4) == 0) 
         *memdata = Mem[ALUresult >> 2]; 
      else return 1;
  }

  return 0;
}


/* Write Register */
/* 10 Points */
void write_register(unsigned r2,unsigned r3,unsigned memdata,unsigned ALUresult,char RegWrite,char RegDst,char MemtoReg,unsigned *Reg)
{

  if(MemtoReg == 1 && RegDst == 0 && RegWrite == 1){
        Reg[r2] = memdata;
  }
  else if(MemtoReg == 0 && RegDst == 1 &&  RegWrite == 1){
        Reg[r3] = ALUresult;
  }
  
}

/* PC update */
/* 10 Points */
void PC_update(unsigned jsec,unsigned extended_value,char Branch,char Jump,char Zero,unsigned *PC)
{
  *PC += 4;
  if(Jump == 1) *PC = (jsec << 2) | (*PC & 0xf0000000);
  if(Zero == 1 && Branch == 1) *PC += extended_value << 2;
}

