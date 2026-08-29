#include "tcp_buffer.h"

#include <string.h>

int sa_tcp_buffer_append(char *data, int *used, int capacity,
                         const void *input, int length) {
  if (data == NULL || used == NULL || input == NULL || length < 0 ||
      *used < 0 || *used > capacity || length > capacity - *used)
    return -1;
  memcpy(data + *used, input, (size_t)length);
  *used += length;
  return length;
}

int sa_tcp_buffer_consume(char *data, int *used, int length) {
  if (data == NULL || used == NULL || length < 0 || length > *used)
    return -1;
  memmove(data, data + length, (size_t)(*used - length));
  *used -= length;
  return length;
}

int sa_tcp_buffer_read_line(char *data, int *used, char *output,
                            int output_capacity, int keep_line_ending) {
  int i, copied;
  if (data == NULL || used == NULL || output == NULL || output_capacity <= 0)
    return -1;
  for (i = 0; i < *used; i++) {
    if (data[i] != '\n')
      continue;
    copied = keep_line_ending ? i + 1 : i;
    if (!keep_line_ending && copied > 0 && data[copied - 1] == '\r')
      copied--;
    if (copied >= output_capacity)
      return -1;
    memcpy(output, data, (size_t)copied);
    output[copied] = '\0';
    sa_tcp_buffer_consume(data, used, i + 1);
    return copied;
  }
  return 0;
}
