#include <stdbool.h>

void file_write_all(const char *filepath, const char *content);
char *file_read_all(const char *filepath);
bool file_is_readable(const char *filepath);

