#include "parser.h"

#include <stdio.h>

#include "isa.h"
// TODO: move it to a separate header/module
typedef struct {
  bool is_valid;
  uint8_t opcode;
  uint8_t rd;
  uint8_t funct3;
  uint8_t rs1;
  uint8_t rs2;
  uint8_t funct7;
} instruction_t;

// static void synchronise(parser_t* parser);
static void error_at(parser_t* parser, const char* message);
static void parser_advance(parser_t* parser);
static bool parser_check(parser_t* parser, tokentype_t type);
static bool parser_match(parser_t* parser, tokentype_t type);
static bool parser_expect(parser_t* parser, tokentype_t type,
                          const char* error_message);
static uint32_t parse_inst(parser_t* parser);
static uint32_t parse_r_type(parser_t* parser, const isa_entry_t* inst);
static uint32_t parse_i_type(parser_t* parser, const isa_entry_t* inst);
static uint32_t parse_s_type(parser_t* parser, const isa_entry_t* inst);
static uint32_t parse_b_type(parser_t* parser, const isa_entry_t* inst);
static uint32_t parse_u_type(parser_t* parser, const isa_entry_t* inst);
static uint32_t parse_j_type(parser_t* parser, const isa_entry_t* inst);
static uint8_t parse_reg(token_t token);
static int parse_imm(token_t token);

static void parse_label(parser_t* parser);
static void parse_directive(parser_t* parser);

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

static uint32_t parse_s_type(parser_t* parser, const isa_entry_t* inst) {
  parser_expect(parser, TOK_REGISTER, "Expected register");
  uint8_t rs2 = parse_reg(parser->previous);

  parser_expect(parser, TOK_COMMA, "Expected comma");
  parser_expect(parser, TOK_INTEGER, "Expected immediate value");
  int imm = parse_imm(parser->previous);

  parser_expect(parser, TOK_LPAREN, "Expected '('");
  parser_expect(parser, TOK_REGISTER, "Expected register");
  uint8_t rs1 = parse_reg(parser->previous);

  parser_expect(parser, TOK_RPAREN, "Expected ')'");
  parser_expect(parser, TOK_NEWLINE, "expected newline after instruction");

  uint32_t instruction = inst->opcode;
  instruction |= (imm & 0x1F) << 7;            // imm[4:0] in rd field
  instruction |= (inst->funct3 & 0x07) << 12;  // funct3 in bits 14:12
  instruction |= (rs1 & 0x1F) << 15;           // rs1 in bits 19:15
  instruction |= (rs2 & 0x1F) << 20;           // rs2 in bits 24:20
  instruction |= ((imm >> 5) & 0x7F) << 25;    // imm[11:5] in bits 31:25

  return instruction;
}

static uint32_t parse_i_type(parser_t* parser, const isa_entry_t* inst) {
  parser_expect(parser, TOK_REGISTER, "Expected register");
  uint8_t rd = parse_reg(parser->previous);

  parser_expect(parser, TOK_COMMA, "Expected comma");
  parser_expect(parser, TOK_REGISTER, "Expected register");
  uint8_t rs1 = parse_reg(parser->previous);

  parser_expect(parser, TOK_COMMA, "Expected comma");
  parser_expect(parser, TOK_INTEGER, "Expected immediate value");
  parser_expect(parser, TOK_NEWLINE, "expected newline after instruction");
  int imm = parse_imm(parser->previous);

  uint32_t instruction = inst->opcode;
  instruction |= (rd & 0x1F) << 7;
  instruction |= (inst->funct3 & 0x07) << 12;
  instruction |= (rs1 & 0x1F) << 15;
  instruction |= (imm & 0xFFF) << 20;
  return instruction;
}

static uint32_t parse_r_type(parser_t* parser, const isa_entry_t* inst) {
  parser_expect(parser, TOK_REGISTER, "Expected register");
  uint8_t rd = parse_reg(parser->previous);

  parser_expect(parser, TOK_COMMA, "Expected comma");
  parser_expect(parser, TOK_REGISTER, "Expected register");
  uint8_t rs1 = parse_reg(parser->previous);

  parser_expect(parser, TOK_COMMA, "Expected comma");
  parser_expect(parser, TOK_REGISTER, "Expected register");
  uint8_t rs2 = parse_reg(parser->previous);

  parser_expect(parser, TOK_NEWLINE, "expected newline after instruction");
  uint32_t instruction = inst->opcode;
  instruction |= (rd & 0x1F) << 7;
  instruction |= (inst->funct3 & 0x07) << 12;
  instruction |= (rs1 & 0x1F) << 15;
  instruction |= (rs2 & 0x1F) << 20;
  instruction |= (inst->funct7 & 0x7F) << 25;
  return instruction;
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

static uint32_t parse_inst(parser_t* parser) {
  const isa_entry_t* inst =
      find_instruction(parser->current.start, parser->current.length);
  if (inst == NULL) {
    error_at(parser, "Unknown instruction");
    return 0xFFFFFFFF;  // Invalid instruction encoding
  }

  parser_advance(parser);  // eat instruction token

  switch (inst->format) {
    case R_TYPE:
      return parse_r_type(parser, inst);
    case I_TYPE:
      return parse_i_type(parser, inst);
    case S_TYPE:
      return parse_s_type(parser, inst);
    case B_TYPE:
      assert(0 && "B-type parsing not implemented yet");
    case U_TYPE:
      assert(0 && "U-type parsing not implemented yet");
    case J_TYPE:
      assert(0 && "J-type parsing not implemented yet");
    default:
      error_at(parser, "Invalid instruction format");
  }
  return 0xFFFFFFFF;  // Invalid instruction encoding
}

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
