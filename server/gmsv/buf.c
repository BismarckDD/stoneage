#include "handletime.h"

static int sUnitSize;       // 每个内存块的大小
static int sUnitNumTotal;   // 所有可用的内存块的数量
static int sUnitNumCurr;    // 当前已用的内存块的数量
static int readblock;
static struct timeval sLastAllocTime;

typedef struct tagMemory {
  char *pointer;
  BOOL used;
  unsigned int nsize;
} Memory;
static Memory *mem;

void memEnd(void) {
  if (mem != NULL && mem[0].pointer != NULL) {
    free(mem[0].pointer);
    free(mem);
  }
}

BOOL configmem(int unit_size, int unit_number) {
  sUnitSize = unit_size;
  sUnitNumTotal = unit_number;
  if (sUnitSize <= 0 || sUnitNumTotal <= 0)
  return TRUE;
}

BOOL memInit(void) {
  int i;
  size_t metadata_size = sizeof(Memory) * (size_t)sUnitNumTotal;
  size_t pool_size = (size_t)sUnitSize * (size_t)sUnitNumTotal;
  mem = calloc((size_t)sUnitNumTotal, sizeof(Memory));
  if (mem == NULL) {
    print("memInit: cannot allocate metadata table: %zu bytes\n", metadata_size);
    return FALSE;
  }

  for (i = 0; i < sUnitNumTotal; i++) {
    mem[i].pointer = NULL;
    mem[i].used = FALSE;
    mem[i].nsize = 0;
  }
  mem[0].pointer = calloc(1, pool_size);
  if (mem[0].pointer == NULL) {
    print("memInit: cannot allocate memory pool: %zu bytes\n", pool_size);
    free(mem);
    mem = NULL;
    return FALSE;
  }
  print("内存分配: %.2f MB 空间......", pool_size / 1024.0 / 1024.0);
  readblock = 0;
  for (i = 0; i < sUnitNumTotal; i++)
    mem[i].pointer = mem[0].pointer + i * sUnitSize;

  sUnitNumCurr = 0;
  sLastAllocTime.tv_sec = NowTime.tv_sec;
  sLastAllocTime.tv_usec = NowTime.tv_usec;
  return TRUE;
}

// 分配nbyte个字节的内存空间，返回指向该内存空间的指针
void *allocateMemory(const unsigned int nbyte) {
  int i;
  BOOL flg = FALSE;
  void *ret;
  int first = 0;
  const int unitNumAlloc = nbyte / sUnitSize + (nbyte % sUnitSize ? 1 : 0);
  if (unitNumAlloc == 0) return NULL;
  i = readblock;
  while (1) {
    if (i > sUnitNumTotal - unitNumAlloc) {
      i = 0;
    }
    if (mem[i].used != FALSE) {
      i += mem[i].nsize;
    } else {
      int j;
      BOOL found = TRUE;
      for (j = i; j < i + unitNumAlloc; j++) {
        if (mem[j].used != FALSE) {
          i = j + mem[j].nsize;
          found = FALSE;
          if (first == 0)
            first = 1;
          break;
        }
      }
      if (found) {
        mem[i].used = TRUE;
        mem[i].nsize = unitNumAlloc;
        readblock = i + unitNumAlloc;
        ret = mem[i].pointer;
        break;
      }
    }
    if ((i >= readblock || i > sUnitNumTotal - unitNumAlloc) && flg == TRUE) {
      ret = NULL;
      break;
    }
    if (i > sUnitNumTotal - unitNumAlloc) {
      i = 0;
      flg = TRUE;
    }
  }
  if (ret == NULL) {
    print("Can't Allocate %d byte .remnants:%4.2f\n", nbyte,
          (float)(sUnitNumCurr / sUnitNumTotal));
  } else {
    sUnitNumCurr += unitNumAlloc;
    if (NowTime.tv_sec > sLastAllocTime.tv_sec + 10) {
      if (sUnitNumCurr > (double)sUnitNumTotal * 0.9) {
        print("Warning!! Memory use rate exceeded 90%% .remnants:%d\n",
              sUnitNumTotal - sUnitNumCurr);
      } else if (sUnitNumCurr > (double)sUnitNumTotal * 0.8) {
        print("Warning!! Memory use rate exceeded 80%% .remnants:%d\n",
              sUnitNumTotal - sUnitNumCurr);
      } else if (sUnitNumCurr > (double)sUnitNumTotal * 0.7) {
        print("Memory use rate exceeded 70%% .remnants:%d\n",
              sUnitNumTotal - sUnitNumCurr);
      }
      memcpy(&sLastAllocTime, &NowTime, sizeof(sLastAllocTime));
    }
  }
  return ret;
}
void freeMemory(void *freepointer) {
  char *toppointer;
  toppointer = mem[0].pointer;
  if (freepointer == NULL)
    return;
  int arrayindex = (int)(((uintptr_t)freepointer - (uintptr_t)toppointer) / sUnitSize);
  if (arrayindex < readblock) {
    readblock = arrayindex;
  }
  mem[arrayindex].used = FALSE;
  sUnitNumCurr -= mem[arrayindex].nsize;
}

void showMem(char *buf) {
  sprintf(buf, "内存剩余比例:%d%%",
          ((sUnitNumTotal - sUnitNumCurr) * 100) / sUnitNumTotal);
  printf("\n");
  printf(buf);
}
