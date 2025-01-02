#include "sdk/VMProtectSDK.h"

unsigned long DELTA = 0x9e3779b9;

#define MX                                                                     \
  (z >> 5 ^ y << 2) + (y >> 3 ^ z << 4) ^ (sum ^ y) + (k[p & 3 ^ e] ^ z);

long tea_encrypt(long *v, long n, long *k) {
#ifdef _VMP_
  VMProtectBegin("tea_encrypt");
#endif
  unsigned long z, y, sum, e;
  z = v[n - 1];
  y = v[0];
  sum = 0;
  long p, q;
  /* 加密过程 */
  q = 6 + 52 / n;
  while (q-- > 0) {
    sum += DELTA;
    e = (sum >> 2) & 3;
    for (p = 0; p < n - 1; p++)
      y = v[p + 1], z = v[p] += MX;
    y = v[0];
    z = v[n - 1] += MX;
  }
#ifdef _VMP_
  VMProtectEnd();
#endif
  return 0;
}

long tea_decrypt(long *v, long n, long *k) {
  unsigned long z, y, sum, e;
  z = v[n - 1];
  y = v[0];
  sum = 0;
  long p, q;
  q = 6 + 52 / n;
  sum = q * DELTA;
  while (sum != 0) {
    e = (sum >> 2) & 3;
    for (p = n - 1; p > 0; p--)
      z = v[p - 1], y = v[p] -= MX;
    z = v[n - 1];
    y = v[0] -= MX;
    sum -= DELTA;
  }
  return 0;
}
