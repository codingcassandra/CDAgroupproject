#include "spimcore.h"


/* ALU */
/* 10 Points */
void ALU(unsigned A,unsigned B,char ALUControl,unsigned *ALUresult,char *Zero)
{

}

/* instruction fetch */
/* 10 Points */
int instruction_fetch(unsigned PC,unsigned *Mem,unsigned *instruction)
{

}


/* instruction partition */
/* 10 Points */
void instruction_partition(unsigned instruction, unsigned *op, unsigned *r1,unsigned *r2, unsigned *r3, unsigned *funct, unsigned *offset, unsigned *jsec)
{

}



/* instruction decode */
/* 15 Points */
int instruction_decode(unsigned op,struct_controls *controls)
{

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
int rw_memory(unsigned ALUresult,unsigned data2,char MemWrite,char MemRead,unsigned *memdata,unsigned *Mem)
{
if(MemWrite == 1){
      if((ALUresult % 4) == 0) Mem[ALUresult >> 2] = data2; else return 1;
  }

  if(MemRead == 1){
      if((ALUresult % 4) == 0) *memdata = Mem[ALUresult >> 2]; else return 1;
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

