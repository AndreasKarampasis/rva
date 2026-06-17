#include "instructions.h"

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define EXPAND_SIZE 1024
#define CURRENT_SIZE (instruction_capacity * sizeof(instruction_t))
#define NEW_SIZE (CURRENT_SIZE + EXPAND_SIZE * sizeof(instruction_t))

instruction_t *instructions = NULL;
size_t instruction_capacity = 0;
size_t instruction_count = 0;

void
i_expand(void)
{
	assert(current_instruction == instruction_capacity);
	instruction_t *p = malloc(NEW_SIZE);
	if (p == NULL) {
		fprintf(stderr, "Failed to allocate memory for instructions\n");
		exit(EXIT_FAILURE);
	}
	if (instructions != NULL) {
		memcpy(p, instructions, CURRENT_SIZE);
		free(instructions);
	}
	instructions = p;
	instruction_capacity += EXPAND_SIZE;
}

instruction_t
make_r_instr(uint8_t opcode, uint8_t rd, uint8_t rs1, uint8_t rs2, uint8_t funct3, uint8_t funct7)
{
	instruction_t parsed_instr = {0};
	parsed_instr.opcode = opcode;
	parsed_instr.fmt = R_TYPE;
	parsed_instr.r.rd = rd;
	parsed_instr.r.funct3 = funct3;
	parsed_instr.r.rs1 = rs1;
	parsed_instr.r.rs2 = rs2;
	parsed_instr.r.funct7 = funct7;
}

void
i_emit(instruction_t *i)
{
	if (instruction_count == instruction_capacity) {
		i_expand();
	}
}