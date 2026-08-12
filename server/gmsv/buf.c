#include "handletime.h"

static int UNIT;
static int UNITNUMBER;
static int memconfig;
static int readblock;
static int NowMemory;
static struct timeval AllocOldTime;

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

BOOL configmem(int unit, int unitnumber) {
  if (memconfig == TRUE)
    return FALSE;
  UNIT = unit;
  UNITNUMBER = unitnumber;
  if (UNIT <= 0 || UNITNUMBER <= 0)
    return memconfig = FALSE;
  return memconfig = TRUE;
}

BOOL memInit(void) {
  int i;
  size_t metadata_size;
  size_t pool_size;
  if (memconfig == FALSE)
    return FALSE;
  metadata_size = sizeof(Memory) * (size_t)UNITNUMBER;
  pool_size = (size_t)UNIT * (size_t)UNITNUMBER;
  mem = calloc((size_t)UNITNUMBER, sizeof(Memory));
  if (mem == NULL) {
    print("memInit: cannot allocate metadata table: %zu bytes\n", metadata_size);
    return FALSE;
  }

  for (i = 0; i < UNITNUMBER; i++) {
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
  print("Memory pool allocated: %.2f MB...", pool_size / 1024.0 / 1024.0);
  readblock = 0;
  for (i = 0; i < UNITNUMBER; i++)
    mem[i].pointer = mem[0].pointer + i * UNIT;

  NowMemory = 0;
  AllocOldTime.tv_sec = NowTime.tv_sec;
  AllocOldTime.tv_usec = NowTime.tv_usec;
  return TRUE;
}

void *allocateMemory(const unsigned int nbyte) {
  int i;
  int arrayAllocSize;
  BOOL flg = FALSE;
  void *ret;
  int first = 0;

  arrayAllocSize = nbyte / UNIT + (nbyte % UNIT ? 1 : 0);
  if (arrayAllocSize == 0)
    return NULL;
  i = readblock;
  while (1) {
    if (i > UNITNUMBER - arrayAllocSize) {
      i = 0;
    }
    if (mem[i].used != FALSE) {
      i += mem[i].nsize;
    } else {
      int j;
      BOOL found = TRUE;
      for (j = i; j < i + arrayAllocSize; j++) {
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
        mem[i].nsize = arrayAllocSize;
        readblock = i + arrayAllocSize;
        ret = mem[i].pointer;
        break;
      }
    }
    if ((i >= readblock || i > UNITNUMBER - arrayAllocSize) && flg == TRUE) {
      ret = NULL;
      break;
    }
    if (i > UNITNUMBER - arrayAllocSize) {
      i = 0;
      flg = TRUE;
    }
  }
  if (ret == NULL) {
    print("Can't Allocate %d byte .remnants:%4.2f\n", nbyte,
          (float)(NowMemory / UNITNUMBER));
  } else {
    NowMemory += arrayAllocSize;

    if (NowTime.tv_sec > AllocOldTime.tv_sec + 10) {
      print("\n");
      if (NowMemory > (double)UNITNUMBER * 0.9) {
        print("Warning!! Memory use rate exceeded 90%% .remnants:%d\n",
              UNITNUMBER - NowMemory);
      } else if (NowMemory > (double)UNITNUMBER * 0.8) {
        print("Warning!! Memory use rate exceeded 80%% .remnants:%d\n",
              UNITNUMBER - NowMemory);
      } else if (NowMemory > (double)UNITNUMBER * 0.7) {
        print("Memory use rate exceeded 70%% .remnants:%d\n",
              UNITNUMBER - NowMemory);
      }
      memcpy(&AllocOldTime, &NowTime, sizeof(AllocOldTime));
    }
  }
  return ret;
}
void freeMemory(void *freepointer) {
  int arrayindex;
  char *toppointer;
  toppointer = mem[0].pointer;
  if (freepointer == NULL)
    return;
  arrayindex = (int)(((uintptr_t)freepointer - (uintptr_t)toppointer) / UNIT);
  if (arrayindex < readblock) {
    readblock = arrayindex;
  }
  mem[arrayindex].used = FALSE;

  NowMemory -= mem[arrayindex].nsize;
}

void showMem(char *buf) {
  sprintf(buf, "NowMemory.remnants:%d%%",
          ((UNITNUMBER - NowMemory) * 100) / UNITNUMBER);
  printf("\n");
  printf(buf);
}
