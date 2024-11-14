#include <stdlib.h>

#ifndef SB_H__
#define SB_H__

typedef struct {
  char* string;
  size_t count;
  size_t size;
} String_Builder;

typedef struct {
  char* string;
  size_t count;
} String_View;

#endif // SB_H__

void sb_append(String_Builder *sb, char c);
void sb_append_cstr(String_Builder *sb, char *cstr);
void sb_append_null(String_Builder *sb);
String_View sb_get_sv(String_Builder *sb);
String_View sb_get_cstr_sv(char *cstr);
