// Name: Brian Baso NID: br168211
// Course: CDA3103 Spring 2018
// Final Version

#include "spimcore.h"

/* ALU */
/* 10 Points */
void ALU(unsigned A,unsigned B,char ALUControl,unsigned *ALUresult,char *Zero) {
    
	switch (ALUControl) {

		// Addition
		case 0:
			*ALUresult = A + B;
			break;
		// Subtraction
		case 1:
			*ALUresult = A - B;
			break;
		// Set on less than signed
		case 2:
			if ((signed) A < (signed) B)
				*ALUresult = 1;
			else
				*ALUresult = 0;
			break;
		// Set on less than unsigned
		case 3:
			if (A < B)
				*ALUresult = 1;
			else
				*ALUresult = 0;
			break;
		// And
		case 4:
			*ALUresult = A & B;
			break;
		// Or
		case 5:
			*ALUresult = A | B;
			break;
		// Left shift
		case 6:
			*ALUresult = B << 16;
			break;
		// Not
		case 7:
			*ALUresult = ~A;
			break;
	}

	// Used for beq to see if A and B are equal
	if ((A - B) == 0)
		*Zero = 1;
	else
		*Zero = 0;
}

/* instruction fetch */
/* 10 Points */
int instruction_fetch(unsigned PC,unsigned *Mem,unsigned *instruction) {

	// Checking to see if the word is not aligned
	if (PC % 4 != 0)
		return 1;

	*instruction = Mem[PC >> 2];
	return 0;
}


/* instruction partition */
/* 10 Points */
void instruction_partition(unsigned instruction, unsigned *op, unsigned *r1,unsigned *r2, unsigned *r3, unsigned *funct, unsigned *offset, unsigned *jsec) {

	// AND instructions with hexadecimal + right shift 
	// get correct bits from instruction section
	// [31 - 26]
	*op = (instruction & 0xFC000000) >> 26;

	// [25 - 21]
	*r1 = (instruction & 0x03E00000) >> 21;

	// [20 - 16]
	*r2 = (instruction & 0x001F0000) >> 16;

	// [15 - 11]
	*r3 = (instruction & 0x0000F800) >> 11;

	// [5 - 0]
	*funct = instruction & 0x0000003F;

	// [15 - 0]
	*offset = instruction & 0x0000FFFF;

	// [25 - 0]
	*jsec = instruction & 0x03FFFFFF;
}


/* instruction decode */
/* 15 Points */
int instruction_decode(unsigned op,struct_controls *controls) {

	switch (op) {

		// R-type
		case 0x0:
			controls->RegDst = 1;
            controls->Jump = 0;
            controls->Branch = 0;
            controls->MemRead = 0;
            controls->MemtoReg = 0;
            controls->ALUOp = 7;
            controls->MemWrite = 0;
            controls->ALUSrc = 0;
            controls->RegWrite = 1;
            break;
		// Jump
		case 0x2:
			controls->RegDst = 2;
            controls->Jump = 1;
            controls->Branch = 2;
            controls->MemRead = 0;
            controls->MemtoReg = 2;
            controls->ALUOp = 0;
            controls->MemWrite = 0;
            controls->ALUSrc = 2;
            controls->RegWrite = 0;
            break;
		// Branch equal
		case 0x4:
			controls->RegDst = 2;
            controls->Jump = 0;
            controls->Branch = 1;
            controls->MemRead = 0;
            controls->MemtoReg = 2;
            controls->ALUOp = 1;
            controls->MemWrite = 0;
            controls->ALUSrc = 2;
            controls->RegWrite = 0;
            break;
		// Add immediate 
		case 0x8:
			controls->RegDst = 0;
            controls->Jump = 0;
            controls->Branch = 0;
            controls->MemRead = 0;
            controls->MemtoReg = 0;
            controls->ALUOp = 0;
            controls->MemWrite = 0;
            controls->ALUSrc = 1;
            controls->RegWrite = 1;
            break;
		// Set less than immediate
		case 0xA:
			controls->RegDst = 0;
            controls->Jump = 0;
            controls->Branch = 0;
            controls->MemRead = 0;
            controls->MemtoReg = 0;
            controls->ALUOp = 2;
            controls->MemWrite = 0;
            controls->ALUSrc = 1;
            controls->RegWrite = 1;
            break;
		// Set less than immediate unsigned
		case 0xB:
            controls->RegDst = 0;
            controls->Jump = 0;
            controls->Branch = 0;
            controls->MemRead = 0;
            controls->MemtoReg = 0;
            controls->ALUOp = 3;
            controls->MemWrite = 0;
            controls->ALUSrc = 1;
            controls->RegWrite = 1;
            break;
		// Load upper immediate
		case 0xF:
            controls->RegDst = 0;
            controls->Jump = 0;
            controls->Branch = 0;
            controls->MemRead = 0;
            controls->MemtoReg = 0;
            controls->ALUOp = 6;
            controls->MemWrite = 0;
            controls->ALUSrc = 1;
            controls->RegWrite = 1;
            break;
		// Load word
		case 0x23:
            controls->RegDst = 0;
            controls->Jump = 0;
            controls->Branch = 0;
            controls->MemRead = 1;
            controls->MemtoReg = 1;
            controls->ALUOp = 0;
            controls->MemWrite = 0;
            controls->ALUSrc = 1;
            controls->RegWrite = 1;
            break;
		// Store word
		case 0x2B:
            controls->RegDst = 0;
            controls->Jump = 0;
            controls->Branch = 0;
            controls->MemRead = 0;
            controls->MemtoReg = 0;
            controls->ALUOp = 0;
            controls->MemWrite = 1;
            controls->ALUSrc = 1;
            controls->RegWrite = 0;
            break;
		// Halt
		default:
			return 1;
	}

	// Success
	return 0;
}

/* Read Register */
/* 5 Points */
void read_register(unsigned r1,unsigned r2,unsigned *Reg,unsigned *data1,unsigned *data2) {

	*data1 = Reg[r1];
	*data2 = Reg[r2];
}


/* Sign Extend */
/* 10 Points */
void sign_extend(unsigned offset,unsigned *extended_value) {

	// Check the sign
	unsigned flag = offset >> 15;

	// If negative, extend
	if (flag)
		*extended_value = offset | 0xFFFF0000;
	else
		*extended_value = offset & 0x0000FFFF;
}

/* ALU operations */
/* 10 Points */
int ALU_operations(unsigned data1,unsigned data2,unsigned extended_value,unsigned funct,char ALUOp,char ALUSrc,unsigned *ALUresult,char *Zero) {

	unsigned char ALUControl = ALUOp;
    
    // Multiple level decoding
	switch (ALUOp) {
		// Addition
		case 0x0:
			break;
		// Subtraction
		case 0x1:
			break;
		// Set less than (signed)
		case 0x2:
			break;
		// Set less than (unsigned)
		case 0x3:
			break;
		// And
		case 0x4:
			break;
		// Or
		case 0x5:
			break;
		// Shift ext
		case 0x6:
			break;
		// R-type
		case 0x7:
			switch (funct) {

				// Addition
				case 0x20:
					ALUControl = 0x0;
					break;
				// Subtraction
				case 0x22:
					ALUControl = 0x1;
					break;
				// Set on less than (signed)
				case 0x2A:
					ALUControl = 0x2;
					break;
				// Set on less than (unsigned)
				case 0x2B:
					ALUControl = 0x3;
					break;
				// And
				case 0x24:
					ALUControl = 0x4;
					break;
				// Or
				case 0x25:
					ALUControl = 0x5;
					break;
				// Halt
				default:
					return 1;
			}
			break;

		default:
			return 1;
	}
    
    // Determine source input of the ALU
    // Control signal to multiplexer
	if (ALUSrc == 1)
		ALU(data1, extended_value, ALUControl, ALUresult, Zero);
	else
		ALU(data1, data2, ALUControl, ALUresult, Zero);
    
	return 0;
}

/* Read / Write Memory */
/* 10 Points */
int rw_memory(unsigned ALUresult,unsigned data2,char MemWrite,char MemRead,unsigned *memdata,unsigned *Mem) {

	// HALT if ALUResult is an address and is not divisble by 4
	if ((MemRead || MemWrite) && ((ALUresult % 4) != 0))
		return 1;

	// If MemRead is true, set *memdata to ALUresult shifted
	if (MemRead)
		*memdata = Mem[ALUresult >> 2];

	// If MemWrite is true, set ALUresult shifted to data2
	if (MemWrite)
		Mem[ALUresult >> 2] = data2;

	return 0;
}


/* Write Register */
/* 10 Points */
void write_register(unsigned r2,unsigned r3,unsigned memdata,unsigned ALUresult,char RegWrite,char RegDst,char MemtoReg,unsigned *Reg) {

	// If RegWrite is false, return
	if (!(RegWrite))
		return;

	// Write data from memory (I type instruction)
	if (RegWrite && MemtoReg) {

		if (RegDst)
			Reg[r3] = memdata;
		else
			Reg[r2] = memdata;
	}

	// Write data from ALUResult
	if (RegWrite && !(MemtoReg)) {

		// R-type instruction
		if (RegDst)
			Reg[r3] = ALUresult;
		// I-type instruction
		else
			Reg[r2] = ALUresult;
	}

}

/* PC update */
/* 10 Points */
void PC_update(unsigned jsec,unsigned extended_value,char Branch,char Jump,char Zero,unsigned *PC) {

	// Initially add four to PC
	*PC += 4;

	// Move to branch address and add the offset to PC
	if (Branch && Zero)
		*PC += (extended_value << 2);

	// Move to jump address, left shift jsec bits by 2
	if (Jump)
		*PC = (*PC & 0xF000000) | (jsec << 2);
}

