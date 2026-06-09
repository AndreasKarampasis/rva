#include "parser.h"

#include <stdio.h>

static void error_at(parser_t* parser, const char* message) {
  fprintf(stderr, "[%u:%u] Error: %s\n", parser->previous.line,
          parser->previous.col, message);
  parser->had_error = true;
}

static void parser_advance(parser_t* parser) {
  parser->previous = parser->current;
  while (true) {
    parser->current = lexer_next_token(parser->lexer);
    if (parser->current.type != TOK_ERROR) {
      break;
    }
    error_at(parser, parser->current.start);
  }
}

void parser_init(parser_t* parser, lexer_t* lexer) {
  parser->lexer = lexer;
  parser->had_error = false;
  parser_advance(parser);
}

static bool parser_check(parser_t* parser, tokentype_t type) {
  return parser->current.type == type;
}

static bool parser_match(parser_t* parser, tokentype_t type) {
  if (!parser_check(parser, type)) {
    return false;
  }
  printf("[%u:%u] Matched token: %.*s\n", parser->current.line,
         parser->current.col, (int)parser->current.length,
         parser->current.start);
  parser_advance(parser);
  return true;
}

static bool parser_expect(parser_t* parser, tokentype_t type,
                          const char* error_message) {
  if (parser_check(parser, type)) {
    return parser_match(parser, type);
  }
  error_at(parser, error_message);
  return false;
}

static void parse_inst(parser_t* parser) {
  // TODO: search for the instruction in the instruction table
  // switch on the instruction format and parse the operands accordingly
  parser_advance(parser);  // Consume the instruction token
  // only support R-type instructions for now
  if (parser_expect(parser, TOK_REGISTER, "Expected register")) {
    // Parse register
    // parse_register(parser);
  }
  parser_expect(parser, TOK_COMMA, "Expected comma");
  parser_expect(parser, TOK_REGISTER, "Expected register");
  parser_expect(parser, TOK_COMMA, "Expected comma");
  parser_expect(parser, TOK_REGISTER, "Expected register");
}

void parser_parse(parser_t* parser) {
  while (!parser_check(parser, TOK_EOF)) {
    if (parser_check(parser, TOK_INSTRUCTION)) {
      parse_inst(parser);
    }
    parser_advance(parser);
  }
}