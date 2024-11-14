#include "sb.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#define MIN_STRING_SIZE 5

void sb_append(String_Builder *sb, char c) {
  if (sb->count+1 >= sb->size) {
    if (sb->size == 0) {
      sb->size = MIN_STRING_SIZE;
    } else {
      sb->size *= 2;
    }
    sb->string = realloc(sb->string, sb->size * sizeof(char));
  } 
  sb->string[sb->count] = c;
  sb->count++;
  sb->string[sb->count] = '\000';
}

void sb_append_cstr(String_Builder *sb, char *cstr) {
  for (size_t i = 0; i < strlen(cstr); i++) {
    sb_append(sb, cstr[i]);
  }
}

void sb_append_null(String_Builder *sb) {
  sb_append(sb, '\000');
}

String_View sb_get_sv(String_Builder *sb) {
  String_View sv = {0};
  sv.string = sb->string;
  sv.count = sb->count;
  return sv;
}

String_View sb_get_cstr_sv(char *cstr) {
  String_View sv = {0};
  sv.string = cstr;
  sv.count = strlen(cstr);
  return sv;
}
