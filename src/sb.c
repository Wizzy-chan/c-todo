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

String_View sv_from_sb(String_Builder *sb) {
  String_View sv = {0};
  sv.string = sb->string;
  sv.count = sb->count;
  return sv;
}

String_View sv_from_str(char *cstr) {
  String_View sv = {0};
  sv.string = cstr;
  sv.count = strlen(cstr);
  return sv;
}

String_View sv_slice_left(String_View *view, char delim) {
  size_t i = 0;
  while (i < view->count && view->string[i] != delim) {
    i++;
  }
  String_View res = {0};
  res.string = view->string;
  res.count = i;
  view->count -= i+1;
  view->string += i+1;
  return res;
}

char *sv_to_str(String_View *view) {
  char* res = malloc((view->count + 1) * sizeof(char));
  for (size_t i = 0; i < view->count; i++) {
    res[i] = view->string[i];
  }
  res[view->count] = '\000';
  return res;
}
