/*
 * isa.h
 *
 * Author:  Andreas Karampasis
 * Created: 2025-05-30
 *
 * RISC-V instruction and register name tables.
 * Shared between the lexer (classification) and codegen (encoding).
 */

#ifndef ISA_H
#define ISA_H

#include <stddef.h>
#include <stdint.h>

#define UNKNOWN_REGISTER_ID 0xFF

typedef struct {
	const char *name;
	uint8_t id;
} register_name_t;

typedef enum {
	R_TYPE,
	I_TYPE,
	S_TYPE,
	B_TYPE,
	U_TYPE,
	J_TYPE,
} format_t;

typedef struct {
	const char *mnemonic;
	uint8_t opcode;
	uint8_t funct3;
	uint8_t funct7; // funct7 uses only 7 bits the top bit is always zero
	format_t format;
} isa_entry_t;

extern const isa_entry_t instruction_table[];
extern const register_name_t register_names[];
const isa_entry_t *find_instruction(const char *mnemonic, size_t length);
#endif /* ISA_H */
