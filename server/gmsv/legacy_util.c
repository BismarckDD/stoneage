#include "util.h"

#include "config_file.h"

#include <ctype.h>
#include <limits.h>

int charInclude(const char *text, const char *characters) {
  const char *match;
  if (text == NULL || characters == NULL)
    return -1;
  match = strpbrk(text, characters);
  return match == NULL ? -1 : (int)(match - text);
}

int strcmptail(const char *text, const char *suffix) {
  size_t text_length;
  size_t suffix_length;
  if (text == NULL || suffix == NULL)
    return 1;
  text_length = strlen(text);
  suffix_length = strlen(suffix);
  if (suffix_length > text_length)
    return 1;
  return strcmp(text + text_length - suffix_length, suffix);
}

int textToInt(const char *text) {
  long long value = 0;
  if (text == NULL)
    return 0;
  while (*text != '\0' && !isdigit((unsigned char)*text))
    ++text;
  while (isdigit((unsigned char)*text)) {
    value = value * 10 + (*text - '0');
    if (value >= INT_MAX)
      return INT_MAX;
    ++text;
  }
  return (int)value;
}

BOOL strtolchecknum(char *text, void *number, int base, CTYPE type) {
  char *end;
  long value;
  if (text == NULL || number == NULL)
    return FALSE;
  errno = 0;
  value = strtol(text, &end, base);
  if (errno == ERANGE || end == text || *end != '\0')
    return FALSE;
  switch (type) {
  case CHAR:
    *(char *)number = (char)value;
    break;
  case SHORT:
    *(short *)number = (short)value;
    break;
  case INT:
    *(int *)number = (int)value;
    break;
  case DOUBLE:
    *(double *)number = (double)value;
    break;
  default:
    return FALSE;
  }
  return TRUE;
}

void deleteCharFromString(char *text, const char *characters) {
  char *read_cursor = text;
  char *write_cursor = text;
  if (text == NULL || characters == NULL)
    return;
  while (*read_cursor != '\0') {
    if (*read_cursor == BACKSLASH && read_cursor[1] != '\0') {
      *write_cursor++ = read_cursor[1];
      read_cursor += 2;
    } else if (strchr(characters, *read_cursor) != NULL) {
      ++read_cursor;
    } else {
      *write_cursor++ = *read_cursor++;
    }
  }
  *write_cursor = '\0';
}

void deleteCharFromStringNoEscape(char *text, const char *characters) {
  char *read_cursor = text;
  char *write_cursor = text;
  if (text == NULL || characters == NULL)
    return;
  while (*read_cursor != '\0') {
    if (strchr(characters, *read_cursor) == NULL)
      *write_cursor++ = *read_cursor;
    ++read_cursor;
  }
  *write_cursor = '\0';
}

char *replaceString(char *text, char old_character, char new_character) {
  char *cursor;
  if (text == NULL)
    return NULL;
  for (cursor = text; *cursor != '\0'; ++cursor) {
    if (*cursor == old_character)
      *cursor = new_character;
  }
  return text;
}

void getFourIntsFromString(char *text, int *value1, int *value2, int *value3,
                           int *value4) {
  int *outputs[] = {value1, value2, value3, value4};
  int index;
  for (index = 0; index < 4; ++index) {
    char token[128];
    if (outputs[index] == NULL)
      continue;
    if (getStringFromIndexWithDelim(text, ",", index + 1, token,
                                    sizeof(token)))
      *outputs[index] = atoi(token);
    else
      *outputs[index] = 0;
  }
}

void deleteSequentChar(char *text, char *characters) {
  char *read_cursor = text;
  char *write_cursor = text;
  char previous = '\0';
  if (text == NULL || characters == NULL)
    return;
  while (*read_cursor != '\0') {
    char current = *read_cursor++;
    if (current == BACKSLASH && *read_cursor != '\0') {
      *write_cursor++ = current;
      *write_cursor++ = *read_cursor++;
      previous = '\0';
      continue;
    }
    if (current == previous && strchr(characters, current) != NULL)
      continue;
    *write_cursor++ = current;
    previous = strchr(characters, current) != NULL ? current : '\0';
  }
  *write_cursor = '\0';
}

int bindLocalhost(unsigned short port) {
  struct sockaddr_in address;
  int fd = socket(AF_INET, SOCK_STREAM, 0);
  if (fd < 0)
    return -1;
  if (getReuseaddr()) {
    int enabled = 1;
    if (setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, (const char *)&enabled,
                   sizeof(enabled)) < 0) {
      close(fd);
      return -1;
    }
  }
  memset(&address, 0, sizeof(address));
  address.sin_family = AF_INET;
  address.sin_port = htons(port);
  address.sin_addr.s_addr = htonl(INADDR_ANY);
  if (bind(fd, (struct sockaddr *)&address, sizeof(address)) < 0 ||
      listen(fd, 5) < 0) {
    close(fd);
    return -1;
  }
  return fd;
}

int connectHost(char *hostname, unsigned short port) {
  struct sockaddr_in address;
  struct hostent *host;
  int fd;
  if (hostname == NULL)
    return -1;
  memset(&address, 0, sizeof(address));
  address.sin_family = AF_INET;
  address.sin_port = htons(port);
  address.sin_addr.s_addr = inet_addr(hostname);
  if (address.sin_addr.s_addr == INADDR_NONE) {
    host = gethostbyname(hostname);
    if (host == NULL || host->h_addr_list == NULL || host->h_addr_list[0] == NULL)
      return -1;
    memcpy(&address.sin_addr, host->h_addr_list[0], sizeof(address.sin_addr));
  }
  fd = socket(AF_INET, SOCK_STREAM, 0);
  if (fd < 0)
    return -1;
  if (connect(fd, (struct sockaddr *)&address, sizeof(address)) < 0) {
    close(fd);
    return -1;
  }
  return fd;
}

static BOOL collectFiles(const char *directory, STRING64 *files, int capacity,
                         int *count) {
  DIR *dir = opendir(directory);
  struct dirent *entry;
  if (dir == NULL)
    return FALSE;
  while (*count < capacity && (entry = readdir(dir)) != NULL) {
    char path[1024];
    struct stat status;
    if (entry->d_name[0] == '.')
      continue;
    snprintf(path, sizeof(path), "%s/%s", directory, entry->d_name);
    if (stat(path, &status) != 0)
      continue;
    if (S_ISDIR(status.st_mode)) {
      if (!collectFiles(path, files, capacity, count)) {
        closedir(dir);
        return FALSE;
      }
    } else {
      strncpysafe(files[*count].string, sizeof(files[*count].string), path);
      ++*count;
    }
  }
  closedir(dir);
  return TRUE;
}

int rgetFileName(const char *directory, STRING64 *files, int capacity) {
  int count = 0;
  if (directory == NULL || files == NULL || capacity < 0)
    return -1;
  return collectFiles(directory, files, capacity, &count) ? count : -1;
}

BOOL checkStringsUnique(char **strings, int count, int verbose) {
  int left;
  int right;
  for (left = 0; left < count - 1; ++left) {
    for (right = left + 1; right < count; ++right) {
      if (strcmp(strings[left], strings[right]) == 0) {
        if (verbose)
          print("Overlapping string: %s\n", strings[left]);
        return FALSE;
      }
    }
  }
  return TRUE;
}

BOOL PointInRect(RECT *rect, POINT *point) {
  if (rect == NULL || point == NULL)
    return FALSE;
  return rect->x <= point->x && point->x <= rect->x + rect->width &&
         rect->y <= point->y && point->y <= rect->y + rect->height;
}

BOOL CoordinateInRect(RECT *rect, int x, int y) {
  POINT point = {x, y};
  return PointInRect(rect, &point);
}

int clipRect(RECT *rect1, RECT *rect2, RECT *result) {
  if (rect1 == NULL || rect2 == NULL || result == NULL)
    return FALSE;
  if (rect1->x > rect2->x + rect2->width - 1 ||
      rect2->x > rect1->x + rect1->width - 1 ||
      rect1->y > rect2->y + rect2->height - 1 ||
      rect2->y > rect1->y + rect1->height - 1)
    return FALSE;
  result->x = max(rect1->x, rect2->x);
  result->y = max(rect1->y, rect2->y);
  result->width = min(rect1->x + rect1->width, rect2->x + rect2->width) -
                  result->x;
  result->height = min(rect1->y + rect1->height, rect2->y + rect2->height) -
                   result->y;
  return TRUE;
}

BOOL isstring1or0(char *text) {
  if (text == NULL)
    return FALSE;
  return strcasecmp(text, "TRUE") == 0 || strcasecmp(text, "1") == 0 ||
         strcasecmp(text, "ON") == 0;
}

float linearDiv(float value1, float value2, float delta) {
  return value1 + (value2 - value1) * delta;
}

void makeRandomString(char *candidates, char *output, int length) {
  int index;
  size_t candidate_count;
  if (output == NULL || length < 0)
    return;
  candidate_count = candidates == NULL ? 0 : strlen(candidates);
  if (candidate_count == 0) {
    output[0] = '\0';
    return;
  }
  for (index = 0; index < length; ++index)
    output[index] = candidates[rand() % candidate_count];
  output[length] = '\0';
}

BOOL checkRedundancy(const int *array, int array_size) {
  int left;
  int right;
  for (left = 0; left < array_size; ++left) {
    if (array[left] == -1)
      continue;
    for (right = left + 1; right < array_size; ++right) {
      if (array[left] == array[right])
        return TRUE;
    }
  }
  return FALSE;
}

static unsigned char reverseBits(unsigned char value) {
  value = (unsigned char)((value >> 4) | (value << 4));
  value = (unsigned char)(((value & 0xCC) >> 2) | ((value & 0x33) << 2));
  return (unsigned char)(((value & 0xAA) >> 1) | ((value & 0x55) << 1));
}

unsigned short CheckCRC(unsigned char *data, int size) {
  unsigned short crc = 0;
  int index;
  int bit;
  for (index = 0; index < size; ++index) {
    crc ^= (unsigned short)reverseBits(data[index]) << 8;
    for (bit = 0; bit < 8; ++bit)
      crc = (unsigned short)((crc & 0x8000) ? (crc << 1) ^ 0x1021
                                           : crc << 1);
  }
  return crc;
}
