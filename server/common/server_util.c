#define __SERVER_UTIL_C__

#include "server_util.h"

int JENCODE_KEY = 1000;

#ifdef SERVER_ENCRYPT
long ringoCompressor(unsigned char *code, long codelen, unsigned char *text,
                     long textlen);
long ringoDecompressor(unsigned char *text, long textlen, unsigned char *code,
                       long codelen);
#endif

#ifdef SERVER_ENCRYPT
static void DecodeString(char *src, char *out);
static void EncodeString(char *src, char *out, int maxoutlen);
#endif

void GetMessageInfo(int *id, char *function_name, const int len,
                    const char **token_list) {
  if (token_list[0] == NULL || token_list[1] == NULL) {
    *id = 0;
    strcpysafe(function_name, len, "");
    return;
  }
  *id = strtoul(token_list[0], NULL, 10);
  strcpysafe(function_name, len, token_list[1]);
  return;
}

void SplitString(char *src, WorkSpace *ws) {
  int i, c = 0;
  char *decoded;
#ifdef SERVER_ENCRYPT
  decoded = ws->crypt_work;
  DecodeString(src, decoded);
#else
  decoded = src;
#endif
  // TODO: log decoded string.
  for (i = 0;; i++) {
    if (decoded[i] == '\0')
      break;
    if (i == 0) {
      ws->token_list[c++] = &(decoded[i]);
    }
    if (decoded[i] == ' ') {
      ws->token_list[c++] = &(decoded[i + 1]);
    }
  }
  while (*decoded) {
    if ((*decoded) == ' ' || (*decoded) == '\n' || (*decoded) == '\r')
      *decoded = '\0';
    decoded++;
  }
  ws->token_list[c] = (char *)NULL;
}

unsigned GetNewMessageID() {
  static unsigned sMessageId = 0;
  return sMessageId++;
}

void DebugSend(WorkSpace *ws, const int fd, char *msg) {
  Send(ws, fd, msg);
}

void Send(WorkSpace *ws, const int fd, char *msg) {
  char *encoded;
  // TODO: log thie original string.
#ifdef SERVER_ENCRYPT
  encoded = ws->crypt_work;
  EncodeString(msg, encoded, ws->work_buf_size * 3);
#else
  encoded = msg;
#endif
  int encoded_len = strlen(encoded);
  if (encoded_len < ws->work_buf_size * 3) {
    encoded[encoded_len] = '\n';
    encoded[encoded_len + 1] = 0;
    ++encoded_len;
  } else {
    // log("encoded_len :%d > work_buf_size:%d err : \n (%s)\n", encoded_len,
    //     ws->work_buf_size, msg);
  }
  ws->write_func(fd, encoded, encoded_len);
}
void CreateHeader(char *out, const char *fname) {
  sprintf(out, "%u %s ", GetNewMessageID(), fname);
}
void CreateHeaderID(char *out, const unsigned msg_id, const char *fname) {
  sprintf(out, "%u %s ", msg_id, fname);
}

#ifdef SERVER_ENCRYPT
void encode64(unsigned char *in, int i, unsigned char *out);
int decode64(unsigned char *in, unsigned char *out);
void jDecode(char *src, int src_len, int key, char *decoded, int *decoded_len);
void jEncode(char *src, int src_len, int key, char *encoded, int *encoded_len,
             int maxencodedlen);
void EncodeString(char *src, char *out, int maxoutlen) {
  int jencodedlen = 0;
  long compressed_l = 0;
  int srclen = strlen(src) + 1;
  int flag = srclen;
  if (srclen < 100) {
    if ((int)srclen > (int)(ws->work_buf_size * 3 - 2)) {
      fprintf(stderr, "lsgen: badly configured work buflen\n");
      exit(1);
    }
    if ((flag & 0x1) == 1)
      flag++;
    ws->compress_work[0] = flag;
    memcpy(ws->compress_work + 1, src, srclen);
    compressed_l = srclen + 1;
  } else {
    if ((flag & 0x1) == 0)
      flag++;
    ws->compress_work[0] = flag;
    compressed_l = ringoCompressor((unsigned char *)ws->compress_work + 1,
                                   (long)ws->work_buf_size * 3 - 1,
                                   (unsigned char *)src, (long)strlen(src)) +
                   1; /* be careful! */
  }
  /* return empty line if error or buffer excess */
  if (compressed_l <= 0) {
    strcpysafe(out, maxoutlen, "\n");
    return;
  }
  memcpy(ws->jencodecopy, ws->compress_work, compressed_l);
  jEncode(ws->jencodecopy, compressed_l, JENCODE_KEY,
          ws->jencodeout, &jencodedlen, ws->work_buf_size * 3 - 1);
  encode64((unsigned char *)ws->jencodeout, jencodedlen,
           (unsigned char *)out);
}
/* translate code64 text to original lsrpc text */
void DecodeString(char *src, char *out) {
  int compressed_l = 0, outlen64;
  int l;
  long decompressed_l = 0;
  /* copy src to copybuffer because jencoder modifies the input buffer */
  l = strlen(src);
  if (src[l - 1] == '\n' || src[l - 1] == '\r')
    src[l - 1] = 0;
  if (src[l - 2] == '\n' || src[l - 2] == '\r')
    src[l - 2] = 0;
  outlen64 =
      decode64((unsigned char *)src, (unsigned char *)ws->jencodecopy);
  jDecode(ws->jencodecopy, outlen64, JENCODE_KEY, ws->compress_work,
          &compressed_l);
  /*out[outlen]=0;  PENDING*/
  if ((ws->compress_work[0] % 2) == 0) {
    if (compressed_l <= 0) {
      decompressed_l = 0;
      fprintf(stderr, "LSRPC: too short:[%s]\n", src);
    } else {
      memcpy(out, ws->compress_work + 1, compressed_l - 1);
      decompressed_l = compressed_l - 1;
    }
  } else {
    decompressed_l = ringoDecompressor(
        (unsigned char *)out, (long)ws->work_buf_size,
        (unsigned char *)ws->compress_work + 1, (long)compressed_l - 1);
  }
  out[decompressed_l] = 0;
}
/* followings are taken from code64.c */
char charset[64] = {'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K',
                    'L', 'M', 'N', 'O', 'P', 'Q', 'R', 'S', 'T', 'U', 'V',
                    'W', 'X', 'Y', 'Z', 'a', 'b', 'c', 'd', 'e', 'f', 'g',
                    'h', 'i', 'j', 'k', 'l', 'm', 'n', 'o', 'p', 'q', 'r',
                    's', 't', 'u', 'v', 'w', 'x', 'y', 'z', '0', '1', '2',
                    '3', '4', '5', '6', '7', '8', '9', '+', '-'};
char reversecharset[256] = {
    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
    0,  0,  0,  0,  0,  62, 0,  63, 0,  0,  52, 53, 54, 55, 56, 57, 58, 59, 60,
    61, 0,  0,  0,  0,  0,  0,  0,  0,  1,  2,  3,  4,  5,  6,  7,  8,  9,  10,
    11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 0,  0,  0,  0,
    0,  0,  26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40, 41, 42,
    43, 44, 45, 46, 47, 48, 49, 50, 51, 0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
    0,  0,  0,  0,  0,  0,  0,  0,  0};

void encode64(unsigned char *in, int len, unsigned char *out) {
  int i;
  int use_bytes;
  int address = 0;
  out[0] = 0;
  for (i = 0;; i += 3) {
    unsigned char in1, in2, in3;
    unsigned char out1, out2, out3, out4;
    if (i >= len)
      break;
    if (i >= (len - 1)) { /* the last letter ( to be thrown away ) */
      in1 = in[i] & 0xff;
      in2 = in3 = 0;
      use_bytes = 2;
    } else if (i >= (len - 2)) { /* the last 2 letters ( process only 1 byte)*/
      in1 = in[i] & 0xff;
      in2 = in[i + 1] & 0xff;
      in3 = 0;
      use_bytes = 3;
    } else { /* there are more or equal than 3 letters */
      in1 = in[i] & 0xff;
      in2 = in[i + 1] & 0xff;
      in3 = in[i + 2] & 0xff;
      use_bytes = 4;
    }
    out1 = ((in1 & 0xfc) >> 2) & 0x3f;
    out2 = ((in1 & 0x03) << 4) | (((in2 & 0xf0) >> 4) & 0x0f);
    out3 = ((in2 & 0x0f) << 2) | (((in3 & 0xc0) >> 6) & 0x03);
    out4 = (in3 & 0x3f);
    if (use_bytes >= 2) {
      out[address++] = charset[out1];
      out[address++] = charset[out2];
      out[address] = 0;
    }
    if (use_bytes >= 3) {
      out[address++] = charset[out3];
      out[address] = 0;
    }
    if (use_bytes >= 4) {
      out[address++] = charset[out4];
      out[address] = 0;
    }
  }
}

static int decode64(unsigned char *in, unsigned char *out) {
  unsigned char in1, in2, in3, in4;
  unsigned char out1, out2, out3;
  int use_bytes;
  int address = 0;
  int i;
  for (i = 0;; i += 4) {
    if (in[i] == 0) {
      break;
    } else if (in[i + 1] == 0) { /* the last letter */
      break;
    } else if (in[i + 2] == 0) { /* the last 2 letters */
      in1 = reversecharset[in[i]];
      in2 = reversecharset[in[i + 1]];
      in3 = in4 = 0;
      use_bytes = 1;
    } else if (in[i + 3] == 0) { /* the last  3 letters */
      in1 = reversecharset[in[i]];
      in2 = reversecharset[in[i + 1]];
      in3 = reversecharset[in[i + 2]];
      in4 = 0;
      use_bytes = 2;
    } else { /* process 4 letters */
      in1 = reversecharset[in[i]];
      in2 = reversecharset[in[i + 1]];
      in3 = reversecharset[in[i + 2]];
      in4 = reversecharset[in[i + 3]];
      use_bytes = 3;
    }
    out1 = (in1 << 2) | (((in2 & 0x30) >> 4) & 0x0f);
    out2 = ((in2 & 0x0f) << 4) | (((in3 & 0x3c) >> 2) & 0x0f);
    out3 = ((in3 & 0x03) << 6) | (in4 & 0x3f);
    if (use_bytes >= 1) {
      out[address++] = out1;
    }
    if (use_bytes >= 2) {
      out[address++] = out2;
    }
    if (use_bytes >= 3) {
      out[address++] = out3;
    }
    if (use_bytes != 3) {
      break;
    }
  }
  return address;
}

/* followings are taken from Jencode.c by jun */
static void jEncode(char *src, int srclen, int key, char *encoded,
                    int *encodedlen, int maxencodedlen) {
  char sum = 0;
  int i;
  if (srclen + 1 > maxencodedlen) {
    *encodedlen = maxencodedlen;
    for (i = 0; i < (*encodedlen); i++)
      encoded[i] = src[i];
  }
  if (srclen + 1 <= maxencodedlen) {
    *encodedlen = srclen + 1;
    for (i = 0; i < srclen; i++) {
      sum = sum + src[i];
      if (((key % 7) == (i % 5)) || ((key % 2) == (i % 2)))
        src[i] = ~src[i];
    }
    for (i = 0; i < (*encodedlen); i++) {
      if (abs((key % srclen)) > i)
        encoded[i] = src[i] + sum * ((i * i) % 3);
      if (abs((key % srclen)) == i)
        encoded[i] = sum;
      if (abs((key % srclen)) < i)
        encoded[i] = src[i - 1] + sum * ((i * i) % 7);
    }
  }
}
static void jDecode(char *src, int srclen, int key, char *decoded,
                    int *decodedlen) {
  char sum = 0;
  int i;
  *decodedlen = srclen - 1;
  if (*decodedlen == 0) {
    return; /* return error if length is 0 */
  }
  sum = src[abs(key % (*decodedlen))];
  for (i = 0; i < srclen; i++) {
    if (abs((key % (*decodedlen))) > i)
      decoded[i] = src[i] - sum * ((i * i) % 3);
    if (abs((key % (*decodedlen))) < i)
      decoded[i - 1] = src[i] - sum * ((i * i) % 7);
  }
  for (i = 0; i < (*decodedlen); i++) {
    if (((key % 7) == (i % 5)) || ((key % 2) == (i % 2)))
      decoded[i] = ~decoded[i];
  }
}

/* masks for first byte ( write )*/
int modifymask_first[8][9] = {
    {0x00000000, 0x00000001, 0x00000011, 0x00000111, 0x00001111, 0x00011111,
     0x00111111, 0x01111111, 0x11111111}, /* mod 0*/
    {0x00000000, 0x00000011, 0x00000111, 0x00001111, 0x00011111, 0x00111111,
     0x01111111, 0x11111111, 0x11111111}, /* mod 1*/
    {0x00000000, 0x00000111, 0x00001111, 0x00011111, 0x00111111, 0x01111111,
     0x11111111, 0x11111111, 0x11111111}, /* mod 2*/
    {0x00000000, 0x00001111, 0x00011111, 0x00111111, 0x01111111, 0x11111111,
     0x11111111, 0x11111111, 0x11111111}, /* mod 3*/
    {0x00000000, 0x00011111, 0x00111111, 0x01111111, 0x11111111, 0x11111111,
     0x11111111, 0x11111111, 0x11111111}, /* mod 4*/
    {0x00000000, 0x00111111, 0x01111111, 0x11111111, 0x11111111, 0x11111111,
     0x11111111, 0x11111111, 0x11111111}, /* mod 5*/
    {0x00000000, 0x01111111, 0x11111111, 0x11111111, 0x11111111, 0x11111111,
     0x11111111, 0x11111111, 0x11111111}, /* mod 6*/
    {0x00000000, 0x11111111, 0x11111111, 0x11111111, 0x11111111, 0x11111111,
     0x11111111, 0x11111111, 0x11111111}, /* mod 7*/
};

/* masks for second byte ( write ) */
int modifymask_second[8][9] = {
    {0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000,
     0x00000000, 0x00000000, 0x00000000}, /* mod 0 */
    {0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000,
     0x00000000, 0x00000000, 0x00000001}, /* mod 1 */
    {0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000,
     0x00000000, 0x00000001, 0x00000011}, /* mod 2 */
    {0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000,
     0x00000001, 0x00000011, 0x00000111}, /* mod 3 */
    {0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000001,
     0x00000011, 0x00000111, 0x00001111}, /* mod 4 */
    {0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000001, 0x00000011,
     0x00000111, 0x00001111, 0x00011111}, /* mod 5 */
    {0x00000000, 0x00000000, 0x00000000, 0x00000001, 0x00000011, 0x00000111,
     0x00001111, 0x00011111, 0x00111111}, /* mod 6 */
    {0x00000000, 0x00000000, 0x00000001, 0x00000011, 0x00000111, 0x00001111,
     0x00011111, 0x00111111, 0x01111111}, /* mod 7 */
};

int bitstream_maxbyte, bitstream_bitaddr;
char *bitstream_buf;

static int initOutputBitStream(char *buf, int buflen) {
  bitstream_bitaddr = 0;
  bitstream_maxbyte = buflen;
  bitstream_buf = buf;
  memset(buf, 0, buflen);
  return 0;
}

static int initInputBitStream(char *buf, int buflen) {
  bitstream_bitaddr = 0;
  bitstream_maxbyte = buflen;
  bitstream_buf = buf;
  return 0;
}

static unsigned int readInputBitStreamBody(int bwidth) {
  int mod = bitstream_bitaddr % 8;
  int byteaddr = bitstream_bitaddr / 8;
  /* return if excess */
  if (byteaddr >= bitstream_maxbyte)
    return 0;
  if (bwidth >= 1 && bwidth <= 8) {
    int b1 = ((bitstream_buf[byteaddr] & modifymask_first[mod][bwidth]) >> mod);
    int b2 = ((bitstream_buf[byteaddr + 1] & modifymask_second[mod][bwidth])
              << (8 - mod));
    bitstream_bitaddr += bwidth;
    return b1 | b2;
  } else {
    return 0;
  }
}

static unsigned int readInputBitStream(int bwidth) {
  if (bwidth <= 0) {
    return 0;
  } else if (bwidth >= 1 && bwidth <= 8) {
    return readInputBitStreamBody(bwidth);
  } else if (bwidth >= 9 && bwidth <= 16) {
    unsigned int first = readInputBitStreamBody(8);
    unsigned int second = readInputBitStreamBody(bwidth - 8);
    return first + (second << 8);
  } else if (bwidth >= 17 && bwidth <= 24) {
    unsigned int first = readInputBitStreamBody(8);
    unsigned int second = readInputBitStreamBody(8);
    unsigned int third = readInputBitStreamBody(bwidth - 8);
    return first + (second << 8) + (third << 16);
  } else if (bwidth >= 25 && bwidth <= 32) {
    unsigned int first = readInputBitStreamBody(8);
    unsigned int second = readInputBitStreamBody(8);
    unsigned int third = readInputBitStreamBody(8);
    unsigned int forth = readInputBitStreamBody(bwidth - 8);
    return first + (second << 8) + (third << 16) + (forth << 24);
  }
  return 0;
}

static int writeOutputBitStreamBody(int bwidth, unsigned char b) {
  int mod = bitstream_bitaddr % 8;
  int byteaddr = bitstream_bitaddr / 8;
  /* return error if excess */
  if (bitstream_maxbyte <= (byteaddr + 1))
    return -1;
  bitstream_buf[byteaddr] &= modifymask_first[mod][bwidth];
  bitstream_buf[byteaddr] |= (b << mod) & modifymask_first[mod][bwidth];
  bitstream_buf[byteaddr + 1] &= modifymask_second[mod][bwidth];
  bitstream_buf[byteaddr + 1] |=
      (b >> (8 - mod)) & modifymask_second[mod][bwidth];
  bitstream_bitaddr += bwidth;
  return byteaddr + 1;
}

static int writeOutputBitStream(int bwidth, unsigned int dat) {
  int ret;
  if (bwidth <= 0) {
    return -1;
  } else if (bwidth >= 1 && bwidth <= 8) {
    if ((ret = writeOutputBitStreamBody(bwidth, (unsigned char)dat)) < 0)
      return -1;
  } else if (bwidth > 8 && bwidth <= 16) {
    if (writeOutputBitStreamBody(8, (unsigned char)(dat & 0xff)) < 0)
      return -1;
    if ((ret = writeOutputBitStreamBody(
             bwidth - 8, (unsigned char)((dat >> 8) & 0xff))) < 0)
      return -1;
  } else if (bwidth > 16 && bwidth <= 24) {
    if (writeOutputBitStreamBody(8, (unsigned char)(dat & 0xff)) < 0)
      return -1;
    if (writeOutputBitStreamBody(8, (unsigned char)((dat >> 8) & 0xff)) < 0)
      return -1;
    if ((ret = writeOutputBitStreamBody(
             bwidth - 16, (unsigned char)((dat >> 16) & 0xff))) < 0)
      return -1;
  } else if (bwidth > 24 && bwidth <= 32) {
    if (writeOutputBitStreamBody(8, (unsigned char)(dat & 0xff)) < 0)
      return -1;
    if (writeOutputBitStreamBody(8, (unsigned char)((dat >> 8) & 0xff)) < 0)
      return -1;
    if (writeOutputBitStreamBody(8, (unsigned char)((dat >> 16) & 0xff)) < 0)
      return -1;
    if ((ret = writeOutputBitStreamBody(
             bwidth - 24, (unsigned char)((dat >> 24) & 0xff))) < 0)
      return -1;
  } else {
    return -1;
  }
  return ret;
}
#define CHAR_SIZE 256
#define NODE_SIZE 512
#define BITS_LEN 9 /* 9 bit lzw compression */
typedef struct {
  unsigned char chr;
  int parent;
  int brother;
  int child;
} NODE;
long ringoCompressor(unsigned char *code, long codelen, unsigned char *text,
                     long textlen) {
  NODE node[NODE_SIZE];
  int freeNode;
  int w, k;    /* used in this algo */
  int textind; /* index to text buffer */
  int i;
  int position = 0; /* indicates the last byte of code buffer */
  if (textlen <= 0)
    return -1;
  initOutputBitStream((char *)code, codelen);
  /* fill characters ( 0 ~ 255 ) in the beggining part of
     Node list */
  for (i = 0; i <= CHAR_SIZE; i++) {
    node[i].chr = (unsigned char)i;
    node[i].brother = i + 1;
    node[i].parent = 0;
    node[i].child = 0;
  }
  node[CHAR_SIZE].brother = 0;
  freeNode = CHAR_SIZE + 1;
  w = text[0];
  textind = 1;
  while (1) {
    int rv;
    if (textind >= textlen) {
      k = CHAR_SIZE; /* indicates EOF */
    } else {
      k = text[textind];
    }
    /* search if pattern 'wk' is registered or not. */
    rv = node[w].child;
    while (1) {
      if (rv <= 0)
        break;
      if (node[rv].chr == k)
        break;
      rv = node[rv].brother;
    }
    if (rv > 0) {
      /* found it */
      w = rv;
    } else {
      position = writeOutputBitStream(BITS_LEN, w);
      /* return if buffer excession  */
      if (position > codelen)
        return -1;
      /* register pattern 'wk' in the dictionary */
      if (freeNode < NODE_SIZE) {
        node[freeNode].parent = w;
        node[freeNode].chr = k;
        node[freeNode].brother = node[w].child;
        node[freeNode].child = 0;
        node[w].child = freeNode;
        freeNode++;
      }
      w = k;
    }
    if (textind == (textlen + 1))
      break;
    textind++;
  }
  return position;
}

long ringoDecompressor(unsigned char *text, long textlen, unsigned char *code,
                       long codelen) {
  NODE node[NODE_SIZE];
  int stack[NODE_SIZE];
  int sp;
  int freeNode;
  int len;
  int i;
  int k = 0;
  int w = 0;
  if (codelen <= 0)
    return -1;
  initInputBitStream((char *)code, codelen);
  for (i = 0; i < CHAR_SIZE; i++) {
    node[i].chr = (unsigned char)i;
    node[i].brother = i + 1;
    node[i].parent = 0;
    node[i].child = 0;
  }
  node[CHAR_SIZE].brother = 0;
  freeNode = CHAR_SIZE + 1;
  len = 0; /* index to text buffer */
  sp = 0;  /* stack pointer */
  while (1) {
    int rv = 0;
    rv = readInputBitStream(BITS_LEN);
    /* return OK if EOF */
    if (rv == CHAR_SIZE)
      break;
    if (rv >= freeNode) {
      stack[sp++] = k; /* exception */
      if (sp >= (sizeof(stack) / sizeof(stack[0])))
        return -1;
      k = w;
    } else {
      k = rv;
    }
    while (k > CHAR_SIZE) {
      if (k >= (sizeof(node) / sizeof(node[0])))
        return -1;
      stack[sp++] = node[k].chr;
      k = node[k].parent;
      if (sp >= (sizeof(stack) / sizeof(stack[0])))
        return -1;
    }
    stack[sp++] = k;
    if (sp >= (sizeof(stack) / sizeof(stack[0])))
      return -1;
    /* output to text buffer from stack.*/
    while (sp) {
      if (++len > textlen)
        return -1;
      *text++ = stack[--sp];
    }
    /* register the pattern 'wk'*/
    if (len > 1 && freeNode < NODE_SIZE) {
      node[freeNode].parent = w;
      node[freeNode].chr = k;
      if (w >= (sizeof(node) / sizeof(node[0])))
        return -1;
      node[freeNode].brother = node[w].child;
      node[freeNode].child = 0;
      node[w].child = freeNode;
      freeNode++;
    }
    w = rv;
  }
  return len;
}
#endif /* ifdef SERVER_ENCRYPT */
