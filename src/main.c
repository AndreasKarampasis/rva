#include <stdio.h>
#include <stdlib.h>

#include "parser.h"

static char* read_file(const char* path) {
  FILE* file = fopen(path, "rb");
  if (file == NULL) {
    fprintf(stderr, "error: cannot open '%s'\n", path);
    exit(1);
  }
  fseek(file, 0L, SEEK_END);
  size_t size_size = ftell(file);
  rewind(file);

  char* buffer = malloc(size_size + 1);
  if (buffer == NULL) {
    fprintf(stderr, "error: failed to allocate memory for source file\n");
    exit(1);
  }
  size_t bytes_read = fread(buffer, sizeof(char), size_size, file);
  if (bytes_read != size_size) {
    fprintf(stderr, "error: failed to read entire source file\n");
    free(buffer);
    exit(1);
  }
  buffer[bytes_read] = '\0';

  fclose(file);
  return buffer;
}

int main(int argc, const char* argv[]) {
  const char* source;
  if (argc == 2) {
    source = read_file(argv[1]);
  } else {
    fprintf(stderr, "Usage: %s <source_file>\n", argv[0]);
    return 1;
  }

  lexer_t lexer;
  lexer_init(&lexer, source);

  parser_t parser;
  parser_init(&parser, &lexer);
  parser_parse(&parser);

  return parser.had_error ? 1 : 0;
}