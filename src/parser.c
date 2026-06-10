#include "parser.h"

#include <stdio.h>

#include "isa.h"
// TODO: move it to a separate header/module
struct instruction_t {
  uint8_t opcode;
  uint8_t rd;
  uint8_t funct3;
  uint8_t rs1;
  uint8_t rs2;
  uint8_t funct7;
};

// static void synchronise(parser_t* parser);
static void error_at(parser_t* parser, const char* message);
static void parser_advance(parser_t* parser);
static bool parser_check(parser_t* parser, tokentype_t type);
static bool parser_match(parser_t* parser, tokentype_t type);
static bool parser_expect(parser_t* parser, tokentype_t type,
                          const char* error_message);
static void parse_inst(parser_t* parser);
static void parse_r_type(parser_t* parser, struct instruction_t* instruction);
static void parse_i_type(parser_t* parser);
static void parse_s_type(parser_t* parser);
static void parse_b_type(parser_t* parser);
static void parse_u_type(parser_t* parser);
static void parse_j_type(parser_t* parser);
static uint8_t parse_reg(token_t token);
static int parse_imm(token_t token);

static void parse_label(parser_t* parser);
static void parse_directive(parser_t* parser);

void parser_init(parser_t* parser, lexer_t* lexer) {
  parser->lexer = lexer;
  parser->had_error = false;
  parser_advance(parser);
}

void parser_parse(parser_t* parser) {
  while (!parser_check(parser, TOK_EOF)) {
    if (parser_match(parser, TOK_NEWLINE)) continue;
    if (parser_check(parser, TOK_INSTRUCTION)) {
      parse_inst(parser);
    } else if (parser_check(parser, TOK_NAME)) {
      parse_label(parser);
    } else if (parser_check(parser, TOK_DOT)) {
      parse_directive(parser);
    } else {
      error_at(parser, "unexpected token");
    }
  }
}

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
    fprintf(stderr, "[%u:%u] Lexer error at '%.*s'\n", parser->current.line,
            parser->current.col, (int)parser->current.length,
            parser->current.start);
    parser->had_error = true;
  }
}

// static void synchronise(parser_t* parser) {
//   while (!parser_check(parser, TOK_EOF) && !parser_check(parser,
//   TOK_NEWLINE)) {
//     parser_advance(parser);
//   }
//   parser_match(parser, TOK_NEWLINE);
// }

static bool parser_check(parser_t* parser, tokentype_t type) {
  return parser->current.type == type;
}

static bool parser_match(parser_t* parser, tokentype_t type) {
  if (!parser_check(parser, type)) {
    return false;
  }
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

static uint8_t parse_reg(token_t token) {
  printf("Parsed register: %.*s (id=%u)\n", (int)token.length, token.start,
         token.reg_id);
  return token.reg_id;
}

static int parse_imm(token_t token) {
  printf("Parsed immediate: %.*s (value=%d)\n", (int)token.length, token.start,
         token.int_value);
  return token.int_value;
}

static void parse_label(parser_t* parser) {
  parser_match(parser, TOK_NAME);
  parser_expect(parser, TOK_COLON, "Expected ':' after label");
  parser_expect(parser, TOK_NEWLINE, "Expected newline after label");
}

static void parse_directive(parser_t* parser) {
  parser_match(parser, TOK_DOT);
  parser_expect(parser, TOK_NAME, "Expected directive name after '.'");
}

static void parse_i_type(parser_t* parser) {
  parser_expect(parser, TOK_REGISTER, "Expected register");
  // uint8_t rs1 = parse_reg(parser->previous);

  parser_expect(parser, TOK_COMMA, "Expected comma");
  parser_expect(parser, TOK_REGISTER, "Expected register");
  // uint8_t rs2 = parse_reg(parser->previous);

  parser_expect(parser, TOK_COMMA, "Expected comma");
  parser_expect(parser, TOK_INTEGER, "Expected immediate value");
  // int imm = parse_imm(parser->previous);
}

static void parse_r_type(parser_t* parser, struct instruction_t* instruction) {
  parser_expect(parser, TOK_INSTRUCTION, "Expected instruction");
  parser_expect(parser, TOK_REGISTER, "Expected register");
  instruction->rd = parse_reg(parser->previous);

  parser_expect(parser, TOK_COMMA, "Expected comma");
  parser_expect(parser, TOK_REGISTER, "Expected register");
  instruction->rs1 = parse_reg(parser->previous);

  parser_expect(parser, TOK_COMMA, "Expected comma");
  parser_expect(parser, TOK_REGISTER, "Expected register");
  instruction->rs2 = parse_reg(parser->previous);

  parser_expect(parser, TOK_NEWLINE, "expected newline after instruction");
}

static void parse_inst(parser_t* parser) {
  const isa_entry_t* inst =
      find_instruction(parser->current.start, parser->current.length);
  if (inst == NULL) {
    error_at(parser, "Unknown instruction");
    return;
  }

  struct instruction_t instruction = {
      .funct3 = inst->funct3, .funct7 = inst->funct7, .opcode = inst->opcode};

  switch (inst->format) {
    case R_TYPE:
      parse_r_type(parser, &instruction);
      break;
    case I_TYPE:
      parse_i_type(parser);
      break;
    case S_TYPE:
      printf("Parsing S-type instruction: %s\n", inst->mnemonic);
      break;
    case B_TYPE:
      printf("Parsing B-type instruction: %s\n", inst->mnemonic);
      break;
    case U_TYPE:
      printf("Parsing U-type instruction: %s\n", inst->mnemonic);
      break;
    case J_TYPE:
      printf("Parsing J-type instruction: %s\n", inst->mnemonic);
      break;
    default:
      error_at(parser, "Invalid instruction format");
      return;
  }
}
