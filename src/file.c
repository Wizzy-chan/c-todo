#include <stdio.h>
#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <errno.h>

/* Writes a string to a file */
void file_write_all(const char *filepath, const char *content) {
  FILE *f = fopen(filepath, "wb");
  if (!f) {
    fprintf(stderr, "ERROR: Could not open file %s for writing: %s\n", filepath, strerror(errno));
    return;
  }
  fprintf(f, "%s", content);
  fclose(f);
}

/* Reads the entirety of a file into a new string, string must be freed once finished with */
char *file_read_all(const char *filepath) {
  FILE *f = fopen(filepath, "rb");
  if (!f) {
    fprintf(stderr, "ERROR: Could not open file %s for reading: %s\n", filepath, strerror(errno));
    return NULL;
  }
  fseek(f, 0, SEEK_END);
  size_t size = ftell(f);
  fseek(f, 0, SEEK_SET);
  char* content = malloc(size + 1);
  memset(content, 0, size + 1);
  fread(content, sizeof(char), size, f);
  return content;
}

/* Returns true if the provided file can be read from, otherwise false */
bool file_is_readable(const char *filepath) {
  FILE *f = fopen(filepath, "rb");
  if (!f) {
    return false;
  }
  fclose(f);
  return true;
}
