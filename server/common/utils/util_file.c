#include "util_file.h"
#include "common.h"

FILE* open_realop_file(const char *filename, int *use_realop_file) {
  FILE *file;
#ifdef _CRYPTO_DATA
  char realop_file[256];
  sprintf(realop_file, "%s.allblues", filename);
  file = fopen(realop_file, "r");
  if (file != NULL) {
    *use_realop_file = 1;
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
    sa_normalize_text_line(line);
    if (line[0] == '#' || line[0] == '\0') {
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
    sa_normalize_text_line(line);
    if (line[0] == '#' || line[0] == '\0') {
      continue;
    }
    callback(line_num, line);
  }
  fclose(file);
  return *line_num;
}

int read_dir(const char *dirname, CallbackFunction callback) {
  char filename[1024];
  struct dirent64 *p_dirent;
  struct stat s;
  DIR *p_dir = opendir(dirname);
  if (p_dir == NULL) {
    logErr("打开文件失败 %s\n", dirname);
    return -1;
  }
  while (1) {
    p_dirent = readdir64(p_dir);
    if (p_dirent == NULL) // 没有文件了
      break;
    if (p_dirent->d_name[0] != '.') { // 规避
      snprintf(filename, sizeof(filename), "%s/%s", dirname, p_dirent->d_name);
      logOut("读取数据:%s\n", filename);
      if (stat(filename, &s) < 0 || !(s.st_mode & S_IFREG)) {
        continue;
      }
      int line_num = 0; // 目的仅为适配get_file_lines和callback的API.
      get_file_lines(filename, &line_num, callback);
    }
  }
  closedir(p_dir);
  return 0;
}

int IsFileExist(const char *filename) {
  FILE *fp = fopen(filename, "r");
  if (fp == NULL) {
    return 0;
  } else {
    fclose(fp);
    return 1;
  }
}
