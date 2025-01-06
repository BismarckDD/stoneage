#include <utils/file.h>

#define BOOL int
#define TRUE 1
#define FALSE 0
#define LINE_SIZE 1024

FILE* open_realop_file(const char *filename, int *use_realop_file) {
  FILE *file;
#ifdef _CRYPTO_DATA
  char realop_file[256];
  sprintf(realop_file, "%s.allblues", filename);
  file = fopen(realop_file, "r");
  if (file != NULL) {
    *use_reloap_file = 1;
  } else
#endif
  file = fopen(filename, "r");
  return file;
}

int get_file_line_num(const char* filename) {
  char line[LINE_SIZE];
  int line_num = 0;
  BOOL crypto = FALSE;
  FILE* file = open_realop_file(filename, &crypto);
  if (file == NULL) {
    printf("open file failed: %s\n", filename);
    return 0;
  }
  while (fgets(line, sizeof(line), file)) {
#ifdef _CRYPTO_DATA
    if (crypto == TRUE) {
      DecryptKey(line);
    }
#endif
    if (line[0] == '#' || line[0] == '\n') {
      continue;
    }
    ++line_num;
  }
  fclose(file);
  return line_num;
}

int get_file_lines(const char* filename, int *line_num,
                   CallbackFunction callback) {
  char line[LINE_SIZE];
  *line_num = 0;
  BOOL crypto = FALSE;
  FILE* file = open_realop_file(filename, &crypto);
  if (file == NULL) {
    printf("open file failed: %s\n", filename);
    return 0;
  }
  while (fgets(line, sizeof(line), file)) {
#ifdef _CRYPTO_DATA
    if (crypto == TRUE) {
      DecryptKey(line);
    }
#endif
    if (line[0] == '#' || line[0] == '\n') {
      continue;
    }
    callback(line_num, line);
  }
  fclose(file);
  return line_num;
}

