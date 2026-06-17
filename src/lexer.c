/*
 * lexer.c
 *
 * Author:  Andreas Karampasis
 * Created: 2025-05-30
 *
 * Tokenizes RISC-V assembly source into a flat stream of tokens.
 */

#include "lexer.h"

#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

#include "isa.h"

static char
peek(lexer_t *lexer)
{
	return lexer->source[lexer->cursor];
}
static bool
is_at_end(lexer_t *lexer)
{
	return peek(lexer) == '\0';
}

static char
advance(lexer_t *lexer)
{
	char c = lexer->source[lexer->cursor++];
	if (c == '\n') {
		lexer->line++;
		lexer->col = 0;
	} else {
		lexer->col++;
	}
	return c;
}

static uint8_t
find_register(const char *text, size_t length)
{
	for (size_t i = 0; register_names[i].name != NULL; i++) {
		if (strncmp(text, register_names[i].name, length) == 0 &&
		    register_names[i].name[length] == '\0') {
			return register_names[i].id;
		}
	}
	return UNKNOWN_REGISTER_ID;
}

static token_t
make_token(lexer_t *lexer, tokentype_t type, size_t start_offset, uint32_t start_col,
	   uint32_t start_line)
{
	return (token_t){
		.type = type,
		.line = start_line,
		.col = start_col,
		.start = lexer->source + start_offset,
		.length = lexer->cursor - start_offset,
	};
}

static token_t
make_register_token(lexer_t *lexer, size_t start_offset, uint32_t start_col, uint32_t start_line,
		    uint8_t register_id)
{
	return (token_t){
		.type = TOK_REGISTER,
		.line = start_line,
		.col = start_col,
		.start = lexer->source + start_offset,
		.length = lexer->cursor - start_offset,
		.reg_id = register_id,
	};
}

static token_t
make_int_token(lexer_t *lexer, size_t start_offset, uint32_t start_col, uint32_t start_line)
{
	return (token_t){
		.type = TOK_INTEGER,
		.line = start_line,
		.col = start_col,
		.start = lexer->source + start_offset,
		.length = lexer->cursor - start_offset,
		.int_value = (int32_t)strtol(lexer->source + start_offset, NULL, 0),
	};
}

static void
skip_whitespace_and_comments(lexer_t *lexer)
{
	while (true) {
		// skip spaces, tabs, carriage returns
		while (peek(lexer) == ' ' || peek(lexer) == '\t' || peek(lexer) == '\r') {
			advance(lexer);
		}
		// skip comment if present
		if (peek(lexer) == '#') {
			while (peek(lexer) != '\n' && !is_at_end(lexer)) {
				advance(lexer);
			}
		} else {
			break; // no comment, done
		}
	}
}

static bool
is_digit(char c)
{
	return c >= '0' && c <= '9';
}
static bool
is_hex_digit(char c)
{
	return (c >= '0' && c <= '9') || (c >= 'a' && c <= 'f') || (c >= 'A' && c <= 'F');
}
static bool
is_alpha(char c)
{
	return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || c == '_';
}

static void
read_number(lexer_t *lexer, const char first_char)
{
	// Check if it's a hex number starting with '0x' or '0X'
	if (first_char == '0' && (peek(lexer) == 'x' || peek(lexer) == 'X')) {
		advance(lexer); // consume 'x'
		while (is_hex_digit(peek(lexer))) {
			advance(lexer);
		}
		return;
	}

	while (is_digit(peek(lexer))) {
		advance(lexer);
	}
}

static char
peek_next(lexer_t *lexer)
{
	if (peek(lexer) != '\0') {
		return lexer->source[lexer->cursor + 1];
	}
	return '\0';
}

static void
read_negative_number(lexer_t *lexer)
{
	// cursor is right after '-', peek is at the first digit
	if (peek(lexer) == '0' && (peek_next(lexer) == 'x' || peek_next(lexer) == 'X')) {
		advance(lexer); // consume '0'
		advance(lexer); // consume 'x'
		while (is_hex_digit(peek(lexer))) {
			advance(lexer);
		}
	} else {
		while (is_digit(peek(lexer))) {
			advance(lexer);
		}
	}
}

static void
read_identifier(lexer_t *lexer)
{
	while (is_alpha(peek(lexer)) || is_digit(peek(lexer))) {
		advance(lexer);
	}
}

static bool
is_instruction(const char *text, size_t length)
{
	for (size_t i = 0; instruction_table[i].mnemonic != NULL; i++) {
		// strncmp because text is not null-terminated
		if (strncmp(text, instruction_table[i].mnemonic, length) == 0 &&
		    instruction_table[i].mnemonic[length] == '\0') {
			return true;
		}
	}
	return false;
}

static token_t
classify_name(lexer_t *lexer, size_t start_offset, uint32_t start_col, uint32_t start_line)
{
	size_t length = lexer->cursor - start_offset;
	const char *text = lexer->source + start_offset;

	// Check if it's an instruction
	if (is_instruction(text, length)) {
		return make_token(lexer, TOK_INSTRUCTION, start_offset, start_col, start_line);
	}

	// Check if it's a register
	uint8_t register_id = find_register(text, length);
	if (register_id != UNKNOWN_REGISTER_ID) {
		return make_register_token(lexer, start_offset, start_col, start_line, register_id);
	}
	// Otherwise it's a name
	return make_token(lexer, TOK_NAME, start_offset, start_col, start_line);
}

void
lexer_init(lexer_t *lexer, const char *source)
{
	lexer->source = source;
	lexer->cursor = 0;
	lexer->line = 1;
	lexer->col = 1;
}

token_t
lexer_next_token(lexer_t *lexer)
{
	skip_whitespace_and_comments(lexer);
	// save position before consuming, token location is where it starts
	size_t start_offset = lexer->cursor;
	uint32_t start_col = lexer->col;
	uint32_t start_line = lexer->line;

	if (is_at_end(lexer)) {
		return make_token(lexer, TOK_EOF, start_offset, start_col, start_line);
	}

	char c = advance(lexer);

	switch (c) {
	case ',':
		return make_token(lexer, TOK_COMMA, start_offset, start_col, start_line);
	case '(':
		return make_token(lexer, TOK_LPAREN, start_offset, start_col, start_line);
	case ')':
		return make_token(lexer, TOK_RPAREN, start_offset, start_col, start_line);
	case '\n':
		return make_token(lexer, TOK_NEWLINE, start_offset, start_col, start_line);
	case ':':
		return make_token(lexer, TOK_COLON, start_offset, start_col, start_line);
	case '.':
		return make_token(lexer, TOK_DOT, start_offset, start_col, start_line);
	case '-':
		if (is_digit(peek(lexer))) {
			read_negative_number(lexer);
			return make_int_token(lexer, start_offset, start_col, start_line);
		}
		break;
	default:
		if (is_digit(c)) {
			read_number(lexer, c);
			return make_int_token(lexer, start_offset, start_col, start_line);
		}
		if (is_alpha(c)) {
			read_identifier(lexer);
			return classify_name(lexer, start_offset, start_col, start_line);
		}
	}
	return make_token(lexer, TOK_ERROR, start_offset, start_col, start_line);
}
