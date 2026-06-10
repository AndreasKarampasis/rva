#include <stdio.h>
#include <stdlib.h>

#include "parser.h"

static char* read_file(const char* path) {
  FILE* file = fopen(path, "rb");
  if (file == NULL) {
    fprintf(stderr, "error: cannot open '%s'\n", path);
    exit(1);
  }
  int32_t status = fseek(file, 0L, SEEK_END);
  if (status != 0) {
    fprintf(stderr, "error: failed to seek to end of '%s'\n", path);
    fclose(file);
    return NULL;
  }

  long pos = ftell(file);
  if (pos < 0L) {
    fprintf(stderr, "error: failed to determine size of '%s'\n", path);
    fclose(file);
    return NULL;
  }
  size_t file_size = (size_t)pos;
  rewind(file);

  char* buffer = malloc(file_size + 1);
  if (buffer == NULL) {
    fprintf(stderr, "error: failed to allocate memory for source file\n");
    fclose(file);
    return NULL;
  }
  size_t bytes_read = fread(buffer, sizeof(char), file_size, file);
  if (bytes_read != file_size) {
    fprintf(stderr, "error: failed to read entire source file\n");
    free(buffer);
    fclose(file);
    return NULL;
  }
  buffer[bytes_read] = '\0';

  fclose(file);
  return buffer;
}

int main(int argc, const char* argv[]) {
  char* source;
  if (argc != 2) {
    fprintf(stderr, "Usage: %s <source_file>\n", argv[0]);
    return 1;
  }

  source = read_file(argv[1]);
  if (source == NULL) {
    return 1;
  }

  lexer_t lexer;
  lexer_init(&lexer, source);

  parser_t parser;
  parser_init(&parser, &lexer);
  parser_parse(&parser);

  free(source);
  return parser.had_error ? 1 : 0;
}