#ifndef PARSER_H
#define PARSER_H

#include <stdbool.h>

#include "lexer.h"

typedef struct {
  lexer_t* lexer;
  token_t current;
  token_t previous;
  bool had_error;
} parser_t;

void parser_init(parser_t* parser, lexer_t* lexer);
void parser_parse(parser_t* parser);

#endif /* PARSER_H */
