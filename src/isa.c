#include "isa.h"

#include <string.h>

const isa_entry_t instruction_table[] = {
	{.mnemonic = "add", .opcode = 0x33, .funct3 = 0x00, .funct7 = 0x00, .format = R_TYPE},
	{.mnemonic = "sub", .opcode = 0x33, .funct3 = 0x00, .funct7 = 0x20, .format = R_TYPE},
	{.mnemonic = "xor", .opcode = 0x33, .funct3 = 0x04, .funct7 = 0x00, .format = R_TYPE},
	{.mnemonic = "or", .opcode = 0x33, .funct3 = 0x06, .funct7 = 0x00, .format = R_TYPE},
	{.mnemonic = "and", .opcode = 0x33, .funct3 = 0x07, .funct7 = 0x00, .format = R_TYPE},
	{.mnemonic = "sll", .opcode = 0x33, .funct3 = 0x01, .funct7 = 0x00, .format = R_TYPE},
	{.mnemonic = "srl", .opcode = 0x33, .funct3 = 0x05, .funct7 = 0x00, .format = R_TYPE},
	{.mnemonic = "sra", .opcode = 0x33, .funct3 = 0x05, .funct7 = 0x20, .format = R_TYPE},
	{.mnemonic = "slt", .opcode = 0x33, .funct3 = 0x02, .funct7 = 0x00, .format = R_TYPE},
	{.mnemonic = "sltu", .opcode = 0x33, .funct3 = 0x03, .funct7 = 0x00, .format = R_TYPE},
	{.mnemonic = "addi", .opcode = 0x13, .funct3 = 0x00, .funct7 = 0x00, .format = I_TYPE},
	{.mnemonic = "xori", .opcode = 0x13, .funct3 = 0x04, .funct7 = 0x00, .format = I_TYPE},
	{.mnemonic = "ori", .opcode = 0x13, .funct3 = 0x06, .funct7 = 0x00, .format = I_TYPE},
	{.mnemonic = "andi", .opcode = 0x13, .funct3 = 0x07, .funct7 = 0x00, .format = I_TYPE},
	{.mnemonic = "slli", .opcode = 0x13, .funct3 = 0x01, .funct7 = 0x00, .format = I_TYPE},
	{.mnemonic = "srli", .opcode = 0x13, .funct3 = 0x05, .funct7 = 0x00, .format = I_TYPE},
	{.mnemonic = "srai", .opcode = 0x13, .funct3 = 0x05, .funct7 = 0x20, .format = I_TYPE},
	{.mnemonic = "slti", .opcode = 0x13, .funct3 = 0x02, .funct7 = 0x00, .format = I_TYPE},
	{.mnemonic = "sltiu", .opcode = 0x13, .funct3 = 0x03, .funct7 = 0x00, .format = I_TYPE},
	{.mnemonic = "lb", .opcode = 0x03, .funct3 = 0x00, .funct7 = 0x00, .format = I_TYPE},
	{.mnemonic = "lh", .opcode = 0x03, .funct3 = 0x01, .funct7 = 0x00, .format = I_TYPE},
	{.mnemonic = "lw", .opcode = 0x03, .funct3 = 0x02, .funct7 = 0x00, .format = I_TYPE},
	{.mnemonic = "lbu", .opcode = 0x03, .funct3 = 0x04, .funct7 = 0x00, .format = I_TYPE},
	{.mnemonic = "lhu", .opcode = 0x03, .funct3 = 0x05, .funct7 = 0x00, .format = I_TYPE},
	{.mnemonic = "sb", .opcode = 0x23, .funct3 = 0x00, .funct7 = 0x00, .format = S_TYPE},
	{.mnemonic = "sh", .opcode = 0x23, .funct3 = 0x01, .funct7 = 0x00, .format = S_TYPE},
	{.mnemonic = "sw", .opcode = 0x23, .funct3 = 0x02, .funct7 = 0x00, .format = S_TYPE},
	{.mnemonic = "beq", .opcode = 0x63, .funct3 = 0x00, .funct7 = 0x00, .format = B_TYPE},
	{.mnemonic = "bne", .opcode = 0x63, .funct3 = 0x01, .funct7 = 0x00, .format = B_TYPE},
	{.mnemonic = "blt", .opcode = 0x63, .funct3 = 0x04, .funct7 = 0x00, .format = B_TYPE},
	{.mnemonic = "bge", .opcode = 0x63, .funct3 = 0x05, .funct7 = 0x00, .format = B_TYPE},
	{.mnemonic = "bltu", .opcode = 0x63, .funct3 = 0x06, .funct7 = 0x00, .format = B_TYPE},
	{.mnemonic = "bgeu", .opcode = 0x63, .funct3 = 0x07, .funct7 = 0x00, .format = B_TYPE},
	{.mnemonic = "jal", .opcode = 0x6F, .funct3 = 0x00, .funct7 = 0x00, .format = J_TYPE},
	{.mnemonic = "jalr", .opcode = 0x67, .funct3 = 0x00, .funct7 = 0x00, .format = I_TYPE},
	{.mnemonic = "lui", .opcode = 0x37, .funct3 = 0x00, .funct7 = 0x00, .format = U_TYPE},
	{.mnemonic = "auipc", .opcode = 0x17, .funct3 = 0x00, .funct7 = 0x00, .format = U_TYPE},
	{.mnemonic = "ecall", .opcode = 0x73, .funct3 = 0x00, .funct7 = 0x00, .format = I_TYPE},
	{.mnemonic = "ebreak", .opcode = 0x73, .funct3 = 0x00, .funct7 = 0x01, .format = I_TYPE},
	{.mnemonic = NULL, .opcode = 0, .funct3 = 0, .funct7 = 0, .format = 0} // sentinel

};

const register_name_t register_names[] = {{"x0", 0},   {"zero", 0}, {"x1", 1},
					  {"ra", 1},   {"x2", 2},   {"sp", 2},
					  {"x3", 3},   {"gp", 3},   {"x4", 4},
					  {"tp", 4},   {"x5", 5},   {"t0", 5},
					  {"x6", 6},   {"t1", 6},   {"x7", 7},
					  {"t2", 7},   {"x8", 8},   {"s0", 8},
					  {"fp", 8},   {"x9", 9},   {"s1", 9},
					  {"x10", 10}, {"a0", 10},  {"x11", 11},
					  {"a1", 11},  {"x12", 12}, {"a2", 12},
					  {"x13", 13}, {"a3", 13},  {"x14", 14},
					  {"a4", 14},  {"x15", 15}, {"a5", 15},
					  {"x16", 16}, {"a6", 16},  {"x17", 17},
					  {"a7", 17},  {"x18", 18}, {"s2", 18},
					  {"x19", 19}, {"s3", 19},  {"x20", 20},
					  {"s4", 20},  {"x21", 21}, {"s5", 21},
					  {"x22", 22}, {"s6", 22},  {"x23", 23},
					  {"s7", 23},  {"x24", 24}, {"s8", 24},
					  {"x25", 25}, {"s9", 25},  {"x26", 26},
					  {"s10", 26}, {"x27", 27}, {"s11", 27},
					  {"x28", 28}, {"t3", 28},  {"x29", 29},
					  {"t4", 29},  {"x30", 30}, {"t5", 30},
					  {"x31", 31}, {"t6", 31},  {NULL, UNKNOWN_REGISTER_ID}};

const isa_entry_t *
find_instruction(const char *mnemonic, size_t length)
{
	for (size_t i = 0; instruction_table[i].mnemonic != NULL; i++) {
		if (strncmp(instruction_table[i].mnemonic, mnemonic, length) == 0 &&
		    instruction_table[i].mnemonic[length] == '\0') {
			return &instruction_table[i];
		}
	}
	return NULL;
}
