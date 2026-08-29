#ifndef STONEAGE_COMMON_TCP_BUFFER_H
#define STONEAGE_COMMON_TCP_BUFFER_H

#include <stddef.h>

int sa_tcp_buffer_append(char *data, int *used, int capacity,
                         const void *input, int length);
int sa_tcp_buffer_consume(char *data, int *used, int length);
/* Returns bytes copied, 0 for an incomplete line, -1 when output is too small. */
int sa_tcp_buffer_read_line(char *data, int *used, char *output,
                            int output_capacity, int keep_line_ending);

#endif
