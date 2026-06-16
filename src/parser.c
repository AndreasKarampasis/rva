#include "parser.h"

#include <assert.h>
#include <stdio.h>

#include "isa.h"

static uint32_t parse_b_type(parser_t* parser, const isa_entry_t* inst);
static uint32_t parse_u_type(parser_t* parser, const isa_entry_t* inst);
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

static inline uint8_t parse_reg(token_t token) { return token.reg_id; }

static inline int parse_imm(token_t token) { return token.int_value; }

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
  printf("%08X\n", instruction);
  // emit(instruction);
  // emit(instruction)

  return instruction;
}

static uint32_t parse_jalr(parser_t* parser, const isa_entry_t* inst) {
  parser_expect(parser, TOK_REGISTER, "Expected register");
  uint8_t rd = parse_reg(parser->previous);

  parser_expect(parser, TOK_COMMA, "Expected comma");
  parser_expect(parser, TOK_INTEGER, "Expected immediate value");
  int imm = parse_imm(parser->previous);

  parser_expect(parser, TOK_LPAREN, "Expected '('");
  parser_expect(parser, TOK_REGISTER, "Expected register");
  uint8_t rs1 = parse_reg(parser->previous);

  parser_expect(parser, TOK_RPAREN, "Expected ')'");
  parser_expect(parser, TOK_NEWLINE, "expected newline after instruction");

  uint32_t instruction = inst->opcode;
  instruction |= (rd & 0x1F) << 7;
  instruction |= (inst->funct3 & 0x07) << 12;
  instruction |= (rs1 & 0x1F) << 15;
  instruction |= (imm & 0xFFF) << 20;
  printf("%08X\n", instruction);
  return instruction;
}

static uint32_t parse_alu_imm(parser_t* parser, const isa_entry_t* inst) {
  parser_expect(parser, TOK_REGISTER, "Expected register");
  uint8_t rd = parse_reg(parser->previous);

  parser_expect(parser, TOK_COMMA, "Expected comma");
  parser_expect(parser, TOK_REGISTER, "Expected register");
  uint8_t rs1 = parse_reg(parser->previous);

  parser_expect(parser, TOK_COMMA, "Expected comma");
  parser_expect(parser, TOK_INTEGER, "Expected immediate value");
  int imm = parse_imm(parser->previous);

  parser_expect(parser, TOK_NEWLINE, "expected newline after instruction");

  uint32_t instruction = inst->opcode;
  instruction |= (rd & 0x1F) << 7;
  instruction |= (inst->funct3 & 0x07) << 12;
  instruction |= (rs1 & 0x1F) << 15;
  instruction |= (imm & 0xFFF) << 20;
  printf("%08X\n", instruction);
  return instruction;
}

static uint32_t parse_load(parser_t* parser, const isa_entry_t* inst) {
  parser_expect(parser, TOK_REGISTER, "Expected register");
  uint8_t rd = parse_reg(parser->previous);

  parser_expect(parser, TOK_COMMA, "Expected comma");
  parser_expect(parser, TOK_INTEGER, "Expected immediate value");
  int imm = parse_imm(parser->previous);

  parser_expect(parser, TOK_LPAREN, "Expected '('");
  parser_expect(parser, TOK_REGISTER, "Expected register");
  uint8_t rs1 = parse_reg(parser->previous);

  parser_expect(parser, TOK_RPAREN, "Expected ')'");
  parser_expect(parser, TOK_NEWLINE, "expected newline after instruction");

  uint32_t instruction = inst->opcode;
  instruction |= (rd & 0x1F) << 7;
  instruction |= (inst->funct3 & 0x07) << 12;
  instruction |= (rs1 & 0x1F) << 15;
  instruction |= (imm & 0xFFF) << 20;
  printf("%08X\n", instruction);
  return instruction;
}

static uint32_t parse_b_type(parser_t* parser, const isa_entry_t* inst) {
  parser_expect(parser, TOK_REGISTER, "Expected register");
  uint8_t rs1 = parse_reg(parser->previous);

  parser_expect(parser, TOK_COMMA, "Expected comma");
  parser_expect(parser, TOK_REGISTER, "Expected register");
  uint8_t rs2 = parse_reg(parser->previous);

  parser_expect(parser, TOK_COMMA, "Expected comma");
  parser_expect(parser, TOK_INTEGER, "Expected immediate value");
  int imm = parse_imm(parser->previous);
  // labels?

  parser_expect(parser, TOK_NEWLINE, "expected newline after instruction");

  uint32_t instruction = inst->opcode;
  instruction |= (inst->funct3 & 0x07) << 12;  // funct3 in bits 14:12
  instruction |= (rs1 & 0x1F) << 15;           // rs1 in bits 19:15
  instruction |= (rs2 & 0x1F) << 20;           // rs2 in bits 24:20
  instruction |= ((imm >> 5) & 0x7E) << 25;    // imm[11:5] in bits 31:25
  instruction |= ((imm & 0x1F) << 7);          // imm[4:0] in bits 11:7
  printf("%08X\n", instruction);
  return instruction;
}

static uint32_t parse_j_type(parser_t* parser, const isa_entry_t* inst) {
  parser_expect(parser, TOK_REGISTER, "Expected register");
  uint8_t rd = parse_reg(parser->previous);

  parser_expect(parser, TOK_COMMA, "Expected comma");
  parser_expect(parser, TOK_INTEGER, "Expected immediate value");
  int imm = parse_imm(parser->previous);

  parser_expect(parser, TOK_NEWLINE, "expected newline after instruction");

  uint32_t instruction = inst->opcode;
  instruction |= (rd & 0x1F) << 7;
  instruction |= (imm & 0xFFFFF) << 12;  // imm[19:0] in bits 31:12
  printf("%08X\n", instruction);
  return instruction;
}

#define OPCODE_JALR 0x67
#define OPCODE_LOAD 0x03
#define OPCODE_ALU_IMM 0x13
static uint32_t parse_i_type(parser_t* parser, const isa_entry_t* inst) {
  switch (inst->opcode) {
    case OPCODE_ALU_IMM:
      return parse_alu_imm(parser, inst);
    case 0x73:  // ECALL/EBREAK
      break;
    case OPCODE_JALR:
      return parse_jalr(parser, inst);
    case OPCODE_LOAD:
      return parse_load(parser, inst);
  }
  error_at(parser, "Unsupported I-type instruction");
  return 0xFFFFFFFF;  // Invalid instruction encoding
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
  printf("%08X\n", instruction);
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
      return parse_b_type(parser, inst);
    case U_TYPE:
      assert(0 && "U-type parsing not implemented yet");
    case J_TYPE:
      return parse_j_type(parser, inst);
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
    if (parser_match(parser, TOK_NEWLINE)) {
      continue;
    } else if (parser_check(parser, TOK_INSTRUCTION)) {
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
