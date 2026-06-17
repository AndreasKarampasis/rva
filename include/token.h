/*
 * token.h
 *
 * Author:  Andreas Karampasis
 * Created: 2025-05-30
 *
 * Token type definitions for the RISC-V assembler lexer.
 */

#ifndef TOKEN_H
#define TOKEN_H

#include <stddef.h>
#include <stdint.h>

typedef enum {
	TOK_INSTRUCTION,
	TOK_REGISTER,
	TOK_INTEGER,
	TOK_NAME,
	TOK_DOT,
	// TOK_STRING, // TODO: support string literals
	// Delimiters
	TOK_COMMA,
	TOK_LPAREN,
	TOK_RPAREN,
	TOK_NEWLINE,
	TOK_COLON,
	TOK_EOF,
	TOK_ERROR,
} tokentype_t;

typedef struct {
	tokentype_t type;
	uint32_t line;
	uint32_t col;
	const char *start;
	size_t length;
	union {
		int32_t int_value; // For TOK_INTEGER
		uint8_t reg_id;	   // FOR TOK_REGISTER
	};
} token_t;

#endif /* TOKEN_H */
