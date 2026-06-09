#ifndef LEXER_H
#define LEXER_H

#include <stddef.h>
#include <stdint.h>

#include "token.h"

typedef struct {
  const char* source;
  size_t cursor;
  uint32_t line;
  uint32_t col;
} lexer_t;

void lexer_init(lexer_t* lexer, const char* source);

token_t lexer_next_token(lexer_t* lexer);
#endif /* LEXER_H */
