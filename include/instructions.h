#ifndef INSTRUCTIONS_H
#define INSTRUCTIONS_H

#include "isa.h"

typedef struct {
	format_t fmt;
	uint8_t opcode;
	uint32_t address;
	union {
		struct {
			uint8_t rd, rs1, rs2, funct3, funct7;
		} r;
		struct {
			uint8_t rd, rs1, funct3;
			int32_t imm;
		} i;
		struct {
			uint8_t rs1, rs2, funct3;
			int32_t imm;
		} s;
		struct {
			uint8_t rs1, rs2, funct3;
			int32_t imm; // target label for jumps
		} b;
		struct {
			uint8_t rd;
			int32_t imm;
		} u;
		struct {
			uint8_t rd;
			int32_t imm; // target label for jumps
		} j;
	};
} instruction_t;

extern instruction_t *instructions;
extern size_t instruction_capacity;
extern size_t current_instruction;

extern void i_emit(instruction_t *i);
extern instruction_t make_r_instr(uint8_t opcode, uint8_t rd, uint8_t rs1, uint8_t rs2,
				  uint8_t funct3, uint8_t funct7);

#endif /* INSTRUCTIONS_H */
