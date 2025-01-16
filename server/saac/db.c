#include "main.h"
#include "util.h"
#define _DB_C_
#include "db.h"

// #define CHARVALUE_MAX 1024
#define MAXTABLE 16
// Spock 2000/10/12
#define CHARVALUE_MAX 256 // DB 字串资料的buffer大小
#define KEY_MAX 64        // DB Key字串的buffer大小
#define HASH_SIZE 65536   // Hash Table 一次增加的Entry数量
#define HASH_PRIME 65521  // Hash Function 使用的质数
#define DBINIT_SIZE 16384 // DB 每次配置Entry的数量

typedef struct tagDbEntry {
  int use;
  int ivalue;
  // Spock 2000/10/12
  int prev; // 前一个DBEntry, -1表示此项为head
  int next; // 下一个DBEntry, -1表示此项为tail
  char key[KEY_MAX];
  char charvalue[CHARVALUE_MAX];
} DBEntry;

// Spock 2000/10/12
// Database HashTable
typedef struct tagHashEntry {
  char key[KEY_MAX]; // 索引key值
  int use;           // 是否已被使用
  int dbind;         // 指向 dbentry 的 index
  int prev;          // 同一key值的上一个 hashentry, -1为head
  int next;          // 同一key值的下一个 hashentry, -1为tail
} HashEntry;
// Spock end

typedef enum {
  DB_INT_SORTED,
  DB_STRING,
} DBTYPE;

typedef struct tagTable {
  int use;       // 0:未使用 1:已使用
  DBTYPE type;   /* DB类型 */
  char name[32]; /* 表名称 */
  int num;       /*  */
  int toplinkindex;
  // Spock 2000/10/12
  HashEntry *hashtable;
  int hashsize;
  int updated;    // 0:dbflush後未更新 1:已更新
  int ent_finder; // 指向最後一次配置的 hashentry
} Table;

DBEntry *gDBEntry; /* 巨件玄伉筏盛迕 */
int dbsize = 0;    /* 1,2,4,8,16...*/
static int dbent_finder = 0;
Table dbt[MAXTABLE];

static void dbShowAllTable(void);

// Spock 2000/10/12
int dbHash(const char *s) {
  char *p;
  unsigned int h = 0, g;
  for (p = s; *p; p++) {
    h = (h << 4) + (*p);
    if ((g = h & 0xf0000000) != 0) {
      h = h ^ (g >> 24);
      h = h ^ g;
    }
  }
  return h % HASH_PRIME;
}
// Spock end

static int reallocDB(void) {
  DBEntry *previous = gDBEntry;
  DBEntry *newbuf;
  int new_dbsize;
  /* Spock deleted 2000/10/12
      if( dbsize == 0 ){
          new_dbsize = 1;
      } else {
          new_dbsize = dbsize * 2;
      }
  */
  // Spock+1 2000/10/12
  new_dbsize = dbsize + DBINIT_SIZE;
  newbuf = (DBEntry *)calloc(1, new_dbsize * sizeof(DBEntry));
  if (newbuf == NULL) {
    log("重新分配数据: 内存不足!!! 新数据大小: %d\n", new_dbsize);
    return -1;
  }
  memset(newbuf, 0, new_dbsize * sizeof(DBEntry));
  if (dbsize > 0) {
    memcpy(newbuf, previous, dbsize * sizeof(DBEntry));
    free(previous);
  }
  // Spock end

  dbent_finder = dbsize; // 将 dbent_finder 指向未使用的 entry
  dbsize = new_dbsize;
  gDBEntry = newbuf;

  log("重新分配数据: 新数据大小:%d 旧地址: %x 新地址:%x\n", new_dbsize,
      (unsigned int)previous, (unsigned int)newbuf);

  return 0;
}

/* allocate a node */
// Spock +1 2000/10/13
static int dbAllocNode() {
  int i;
  for (i = 0; i < dbsize; i++) {
    dbent_finder++;
    if (dbent_finder == dbsize) {
      dbent_finder = 0;
    }
    if (gDBEntry[dbent_finder].use == 0) {
      gDBEntry[dbent_finder].use = 1;
      return dbent_finder;
    }
  }
  log("数据进入队列失败. 重新分配中....\n");
  if (reallocDB() < 0) {
    log("重新分配失败\n");
  } else {
    // return dbAllocNode( type );
    //  Spock 2000/10/13
    gDBEntry[dbent_finder].use = 1;
    log("数据探测=%d\n", dbent_finder);
    return dbent_finder;
    // Spock end
  }
  return -1;
}

static void dbReleaseNode(int index) {
  // Spock 2000/10/12
  int prev = gDBEntry[index].prev;
  int next = gDBEntry[index].next;
  gDBEntry[index].use = 0;
  if (prev >= 0)
    gDBEntry[prev].next = next;
  if (next >= 0)
    gDBEntry[next].prev = prev;
}

void dbShowLink(int topind) {
  int cur = topind;
  log("开始从 %d 链接数据\n", cur);
  while (cur >= 0) {
    if (gDBEntry[cur].use == 0) {
      log("dbShowLink: use is 0! key:%s\n", gDBEntry[cur].key);
      return;
    }
    log("%s %i\n", gDBEntry[cur].key, gDBEntry[cur].ivalue);
    cur = gDBEntry[cur].next;
  }
}
// Spock 2000/10/13
static int reallocHash(int dbi) {
  HashEntry *previous = dbt[dbi].hashtable;
  HashEntry *newbuf;
  int new_hashsize;

  new_hashsize = dbt[dbi].hashsize + HASH_SIZE;
  newbuf = (HashEntry *)calloc(1, new_hashsize * sizeof(HashEntry));
  if (newbuf == NULL) {
    log("重新分配无用信息: 内存不足!!! 新无用信息大小: %d\n", new_hashsize);
    return -1;
  }

  memset(newbuf, 0, new_hashsize * sizeof(HashEntry));
  if (previous) {
    memcpy(newbuf, previous, dbt[dbi].hashsize * sizeof(HashEntry));
    free(previous);
  }

  if (dbt[dbi].hashsize > HASH_PRIME)
    dbt[dbi].ent_finder = dbt[dbi].hashsize;
  else
    dbt[dbi].ent_finder = HASH_PRIME;
  dbt[dbi].hashsize = new_hashsize;
  dbt[dbi].hashtable = newbuf;

  log("重新分配无用信息: 新无用信息大小:%d 旧地址: %x 新地址:%x\n",
      new_hashsize, (unsigned int)previous, (unsigned int)newbuf);

  return 0;
}

static int tableGetEntry(int dbi, char *k) {
  int hashkey = dbHash(k);
  HashEntry *hash = dbt[dbi].hashtable;
  if (hash[hashkey].use == 0)
    return -1;
  while (1) {
    // if (hash[hashkey].use == 1 && strcmp(hash[hashkey].key, k) == 0){
    if (hash[hashkey].use == 1) {
      if (strcmp(hash[hashkey].key, k) == 0)
        return hashkey;
    }
    hashkey = hash[hashkey].next;
    if (hashkey <= 0) {
      // log("err not found hash[%x] -%s!\n", hashkey, k)
      return -1;
    }
  }
}

static int tableInsertNode(int dbi, char *k, int dbind) {
  int hashkey = dbHash(k);
  int hashnext = -1;
  int i;
  HashEntry *hash = dbt[dbi].hashtable;

  if (hash[hashkey].use == 0) {
    strcpy(hash[hashkey].key, k);
    hash[hashkey].use = 1;
    hash[hashkey].dbind = dbind;
    hash[hashkey].prev = -1;
    hash[hashkey].next = -1;
    dbt[dbi].num++;
    return hashkey;
  } else {
    for (i = 0; i < dbt[dbi].hashsize - HASH_PRIME; i++) {
      dbt[dbi].ent_finder++;
      if (dbt[dbi].ent_finder >= dbt[dbi].hashsize)
        dbt[dbi].ent_finder = HASH_PRIME;
      if (hash[dbt[dbi].ent_finder].use == 0) {
        hashnext = dbt[dbi].ent_finder;
        break;
      }
    }
    if (hashnext < HASH_PRIME) {
      log("tableInsertNode: hashentry array full. reallocating....\n");
      if (reallocHash(dbi) < 0) {
        log("tableInsertNode: reallocation fail\n");
        return -1;
      } else {
        hash = dbt[dbi].hashtable;
        hashnext = dbt[dbi].ent_finder;
      }
    }
    strcpy(hash[hashnext].key, k);
    hash[hashnext].use = 1;
    hash[hashnext].dbind = dbind;
    hash[hashnext].prev = hashkey;
    hash[hashnext].next = hash[hashkey].next;
    if (hash[hashkey].next >= 0)
      hash[hash[hashkey].next].prev = hashnext;
    hash[hashkey].next = hashnext;
    dbt[dbi].num++;
    return hashnext;
  }
}

static void tableReleaseNode(int dbi, int ind) {
  dbt[dbi].hashtable[ind].use = 0;
  if (dbt[dbi].hashtable[ind].prev >= 0) {
    dbt[dbi].hashtable[dbt[dbi].hashtable[ind].prev].next =
        dbt[dbi].hashtable[ind].next;
  }
  if (dbt[dbi].hashtable[ind].next >= 0) {
    dbt[dbi].hashtable[dbt[dbi].hashtable[ind].next].prev =
        dbt[dbi].hashtable[ind].prev;
  }
  dbt[dbi].num--;
}

// Spock 2000/10/13
static int dbExtractNodeByKey(int dbi, char *k) {
  int hash_index = tableGetEntry(dbi, k);

  if (hash_index < 0) {
    log("dbExtractNodeByKey: tableGetEntry fail, key:%s\n", k);
    return -1;
  }
  if (dbt[dbi].hashtable[hash_index].dbind < 0) {
    log("dbExtractNodeByKey: invalid dbind in hash, key:%s\n", k);
    return -1;
  }
  dbReleaseNode(dbt[dbi].hashtable[hash_index].dbind);
  tableReleaseNode(dbi, hash_index);
  return 0;
}

// Spock 2000/10/13
static int dbInsertNodeByIValue(int topind, int ins) {
  int cur = topind;
  if ((topind < 0) || (topind >= dbsize) || (ins < 0) || (ins >= dbsize))
    return -1;
  while (gDBEntry[cur].next >= 0) {
    if (gDBEntry[gDBEntry[cur].next].ivalue < gDBEntry[ins].ivalue)
      break;
    cur = gDBEntry[cur].next;
  }
  gDBEntry[ins].prev = cur;
  gDBEntry[ins].next = gDBEntry[cur].next;
  if (gDBEntry[cur].next >= 0)
    gDBEntry[gDBEntry[cur].next].prev = ins;
  gDBEntry[cur].next = ins;
  return 0;
}

// Spock 2000/10/13
static int dbAppendNode(int topind, int ins) {

  if ((topind < 0) || (topind >= dbsize) || (ins < 0) || (ins >= dbsize))
    return -1;
  gDBEntry[ins].prev = topind;
  gDBEntry[ins].next = gDBEntry[topind].next;
  if (gDBEntry[topind].next >= 0)
    gDBEntry[gDBEntry[topind].next].prev = ins;
  gDBEntry[topind].next = ins;
  return 0;
}

static int dbGetTableIndex(char *tname, DBTYPE type) {
  int i;
  for (i = 0; i < MAXTABLE; i++) {
    if (dbt[i].use && strcmp(dbt[i].name, tname) == 0 && dbt[i].type == type) {
      return i;
    }
  }
  for (i = 0; i < MAXTABLE; i++) {
    if (dbt[i].use == 0) {
      int topind;
      dbt[i].use = 1;
      dbt[i].type = type;
      snprintf(dbt[i].name, sizeof(dbt[i].name), "%s", tname);
      // Spock 2000/10/16
      if (reallocHash(i) < 0) {
        log("重新分配无用信息失败\n");
        return -2;
      }
      dbt[i].ent_finder = HASH_PRIME;
      // Spock end

      // topind = dbAllocNode( type );
      //  Spock +1 2000/10/16
      topind = dbAllocNode();
      if (topind < 0) {
        log("数据分配节点失败\n");
        return -2;
      }
      gDBEntry[topind].ivalue = 0x7fffffff;
      gDBEntry[topind].prev = -1;
      gDBEntry[topind].next = -1;
      strcpy(gDBEntry[topind].key, "top_node");
      gDBEntry[topind].charvalue[0] = 0;
      // Spock end
      dbt[i].toplinkindex = topind;
      return i;
    }
  }
  log("dbGetTableIndex: table full. now tables are:\n");
  dbShowAllTable();
  return -1;
}

int dbUpdateEntryInt(const char *table, const char *key, const int value,
                     const char *info) {
  int dbi = dbGetTableIndex(table, DB_INT_SORTED);
  int dbind, newpos;
  if (strlen(key) >= KEY_MAX) {
    log("dbUpdateEntryInt: key is too long, key:%s\n", key);
    return -1;
  }
  if (strlen(info) >= CHARVALUE_MAX) {
    log("dbUpdateEntryInt: charvalue is too long, charvalue:%s\n", info);
    return -1;
  }
  if (dbi < 0) {
    log("dbUpdateEntryInt: dbGetTableIndex fail\n");
    return -1;
  }
  const int hash_index = tableGetEntry(dbi, key);
  if (hash_index < 0) {
    dbind = dbAllocNode();
    if (dbind < 0) {
      log("dbUpdateEntryInt: dbAllocNode fail\n");
      return -1;
    }
    gDBEntry[dbind].ivalue = value;
    strcpy(gDBEntry[dbind].key, key);
    strcpy(gDBEntry[dbind].charvalue, info);
    if (dbInsertNodeByIValue(dbt[dbi].toplinkindex, dbind) < 0) {
      gDBEntry[dbind].use = 0;
      log("dbUpdateEntryInt: dbInsertNodeByIValue fail\n");
      return -1;
    }
    if (tableInsertNode(dbi, key, dbind) < 0) {
      dbReleaseNode(dbind);
      log("dbUpdateEntryInt: tableInsertNode fail\n");
      return -1;
    }
  } else {
    dbind = dbt[dbi].hashtable[hash_index].dbind;
    gDBEntry[dbind].ivalue = value;
    strcpy(gDBEntry[dbind].charvalue, info);
    newpos = dbind;
    while (gDBEntry[newpos].prev >= 0) {
      if (value <= gDBEntry[gDBEntry[newpos].prev].ivalue) {
        break;
      }
      newpos = gDBEntry[newpos].prev;
    }
    if (newpos != dbind) {
      gDBEntry[gDBEntry[dbind].prev].next = gDBEntry[dbind].next;
      if (gDBEntry[dbind].next >= 0)
        gDBEntry[gDBEntry[dbind].next].prev = gDBEntry[dbind].prev;
      gDBEntry[dbind].prev = gDBEntry[newpos].prev;
      gDBEntry[dbind].next = newpos;
      if (gDBEntry[newpos].prev >= 0)
        gDBEntry[gDBEntry[newpos].prev].next = dbind;
      gDBEntry[newpos].prev = dbind;
      dbt[dbi].updated = 1;
      return 0;
    }
    while (gDBEntry[newpos].next >= 0) {
      if (value >= gDBEntry[gDBEntry[newpos].next].ivalue) {
        break;
      }
      newpos = gDBEntry[newpos].next;
    }
    if (newpos != dbind) {
      gDBEntry[gDBEntry[dbind].prev].next = gDBEntry[dbind].next;
      gDBEntry[gDBEntry[dbind].next].prev = gDBEntry[dbind].prev;
      gDBEntry[dbind].prev = newpos;
      gDBEntry[dbind].next = gDBEntry[newpos].next;
      if (gDBEntry[newpos].next >= 0)
        gDBEntry[gDBEntry[newpos].next].prev = dbind;
      gDBEntry[newpos].next = dbind;
    }
  }
  dbt[dbi].updated = 1;
  return 0;
}

int dbDeleteEntryInt(const char *table, const char *key) {
  int dbi = dbGetTableIndex(table, DB_INT_SORTED);
  int r;
  if (strlen(key) >= KEY_MAX) {
    log("dbDeleteEntryInt: key is too long, key:%s\n", key);
    return -1;
  }
  if (dbi < 0) {
    log("dbDeleteEntryInt: dbGetTableIndex failed for %s\n", table);
    return -1;
  }
  r = dbExtractNodeByKey(dbi, key);
  if (r < 0) {
    log("dbDeleteEntryInt: dbExtractNodeByKey failed for %s in %s\n", key,
        table);
    return -1;
  }
  dbt[dbi].updated = 1;
  log("删除人物 %s 来自表 %s\n", key, table);
  return 0;
}

static void dbShowAllTable(void) {
  int i;

  for (i = 0; i < MAXTABLE; i++) {
    if (dbt[i].use) {
      log("%d Name:%s Use:%d Type:%d\n", i, dbt[i].name, dbt[i].num,
          dbt[i].type);
    }
  }
}

int dbGetEntryInt(const char *table, const char *key, int *output) {
  int dbi = dbGetTableIndex(table, DB_INT_SORTED);
  if (dbi < 0) {
    log("dbGetEntryInt: dbGetTableIndex fail\n");
    return -1;
  }
  // Spock 2000/10/19
  if (strlen(key) >= KEY_MAX) {
    log("dbGetEntryInt: key is too long, key:%s\n", key);
    return -1;
  }
  const int hash_index = tableGetEntry(dbi, key);
  if (hash_index < 0)
    return -1;
  const int entry_index = dbt[dbi].hashtable[hash_index].dbind;
  if (entry_index < 0) {
    log("dbGetEntryInt: Invalid dbind in hashtable of %s\n", table);
    return -1;
  }
  *output = gDBEntry[entry_index].ivalue;
  return 0;
}

int dbGetEntryRank(char *table, char *key, int *rank_out, int *count_out) {
  int dbi = dbGetTableIndex(table, DB_INT_SORTED);
  int cur;
  int now_score = 0x7fffffff; /*int 匹中切壬氏匹井中袄 */
  int r = -1, i = 0;
  if (strlen(key) >= KEY_MAX) {
    log("dbGetEntryRank: key is too long, key:%s\n", key);
    return -1;
  }
  if (dbi < 0) {
    log("dbGetEntryRank: dbGetTableIndex fail\n");
    return -1;
  }
  cur = gDBEntry[dbt[dbi].toplinkindex].next;
  while (cur >= 0) {
    if (gDBEntry[cur].ivalue != now_score) {
      r = i;
      now_score = gDBEntry[cur].ivalue;
    }
    if (strcmp(gDBEntry[cur].key, key) == 0) {
      *rank_out = r;
      *count_out = i;
      return 0;
    }
    cur = gDBEntry[cur].next;
    i++;
  }
  *count_out = i;
  *rank_out = r;
  return 0;
}

int dbGetEntryRankRange(char *table, int start, int end, char *output,
                        int outlen) {
#define MAXHITS 1024 /* 赝癫支卅丐［匹手仇木匹蜗坌日仄中冗 ringo */
  struct hitent { /* 仇及厌瞻  卞甲永玄仄凶支勾毛凶户化中仁 */
    int entry_index;
    int rank;
  };

  int r = 0;
  struct hitent hits[MAXHITS];
  int dbi = dbGetTableIndex(table, DB_INT_SORTED);
  int cur;
  int hitsuse = 0, i;
  int now_score = 0x7fffffff;

  if (dbi < 0)
    return -1;
  if (outlen <= 0)
    return -1;

  cur = dbt[dbi].toplinkindex;
  while (cur >= 0) {
    if (gDBEntry[cur].ivalue != now_score) {
      r++;
      now_score = gDBEntry[cur].ivalue;
    }
    if (r >= start && r <= end) {
      hits[hitsuse].entry_index = cur;
      hits[hitsuse].rank = r;
      hitsuse++;
      if (hitsuse >= MAXHITS)
        break;
    }
    cur = gDBEntry[cur].next;
  }
  output[0] = 0;

  for (i = 0; i < hitsuse; i++) {
    char tmp[1024];
    snprintf(tmp, sizeof(tmp), "%d,%s,%d,%s", hits[i].rank,
             gDBEntry[hits[i].entry_index].key,
             gDBEntry[hits[i].entry_index].ivalue,
             gDBEntry[hits[i].entry_index].charvalue);
    strcatsafe(output, outlen, tmp);
    if (i != (hitsuse - 1)) {
      strcatsafe(output, outlen, "|");
    }
  }
  return 0;
}

int dbFlush(const char *dir) {
  int i;

  for (i = 0; i < MAXTABLE; i++) {
    FILE *fp;
    char filename[1024];
    int entry_index;
    if (!dbt[i].use)
      continue;
    // Spock 2000/10/23
    if (dbt[i].updated == 0) {
      log("dbFlush: table %s not updated\n", dbt[i].name);
      continue;
    }
    // Spock end

    if (dbt[i].type == DB_INT_SORTED) {
      snprintf(filename, sizeof(filename), "%s/int/%s", dir, dbt[i].name);
    } else {
      snprintf(filename, sizeof(filename), "%s/string/%s", dir, dbt[i].name);
    }

    fp = fopen(filename, "w");
    if (fp == NULL) {
      log("cannot open file: %s %s\n", filename, strerror(errno));
      continue;
    }

    // Spock 2000/10/19
    entry_index = gDBEntry[dbt[i].toplinkindex].next;
    while (entry_index >= 0) {
      if (dbt[i].type == DB_INT_SORTED) {
        fprintf(fp, "%s %d %s\n", gDBEntry[entry_index].key,
                gDBEntry[entry_index].ivalue,
                makeStringFromEscaped(gDBEntry[entry_index].charvalue));
      } else {
        fprintf(fp, "%s %s\n", gDBEntry[entry_index].key,
                makeStringFromEscaped(gDBEntry[entry_index].charvalue));
      }
      entry_index = gDBEntry[entry_index].next;
    }
    fclose(fp);
    dbt[i].updated = 0;
  }

  return 0;
}

int dbRead(const char *dir) {
  char dirname[1024];
  DIR *d;
  struct dirent *de;
  // Spock +1 2000/10/19
  memset(dbt, 0, MAXTABLE * sizeof(Table));

  char tmp[1024];
  snprintf(tmp, sizeof(tmp), "%s/int", dir);
  if (mkdir(tmp, 0755) == 0) {
    log("创建 %s\n", tmp);
  }
  snprintf(tmp, sizeof(tmp), "%s/string", dir);
  if (mkdir(tmp, 0755) == 0) {
    log("创建 %s\n", tmp);
  }
  snprintf(dirname, sizeof(dirname), "%s/int", dir);
  d = opendir(dirname);
  if (d == NULL) {
    log("不能打开文件 %s\n", dirname);
    return -1;
  }
  while (1) {
    de = readdir(d);
    if (de == NULL)
      break;
    if (de->d_name[0] != '.') {
      char filename[1024];
      FILE *fp;
      struct stat s;
      snprintf(filename, sizeof(filename), "%s/%s", dirname, de->d_name);
      log("读取数据:%s\n..", filename);
      if (stat(filename, &s) < 0) {
        continue;
      }
      if (!(s.st_mode & S_IFREG)) {
        continue;
      }

      fp = fopen(filename, "r");
      if (fp == NULL) {
        log("不能打开文件 %s %s\n", filename, strerror(errno));
        continue;
      }
      while (1) {
        char line[1024];
        char k[1024], v[1024], info[1024];
        if (fgets(line, sizeof(line), fp) == NULL)
          break;
        chop(line);
        k[0] = '\0';
        easyGetTokenFromString(line, 1, k, sizeof(k));
        v[0] = '\0';
        easyGetTokenFromString(line, 2, v, sizeof(v));
        info[0] = '\0';
        easyGetTokenFromString(line, 3, info, sizeof(info));
        dbUpdateEntryInt(de->d_name, k, atoi(v), info);
      }
      fclose(fp);
    }
  }
  closedir(d);
  snprintf(dirname, sizeof(dirname), "%s/string", dir);
  d = opendir(dirname);
  if (d == NULL) {
    log("不能打开文件 %s\n", dirname);
    return -1;
  }
  while (1) {
    de = readdir(d);
    if (de == NULL)
      break;
    if (de->d_name[0] != '.') {
      char filename[1024];
      FILE *fp;
      struct stat s;
      snprintf(filename, sizeof(filename), "%s/%s", dirname, de->d_name);
      log("读取数据:%s\n..", filename);

      if (stat(filename, &s) < 0) {
        continue;
      }
      if (!(s.st_mode & S_IFREG)) {
        continue;
      }
      fp = fopen(filename, "r");
      if (fp == NULL) {
        log("不能打开文件 %s %s\n", filename, strerror(errno));
        continue;
      }
      while (1) {
        char line[CHARVALUE_MAX + 1024];
        char k[1024];
        if (fgets(line, sizeof(line), fp) == NULL)
          break;
        chop(line);
        k[0] = '\0';
        easyGetTokenFromString(line, 1, k, sizeof(k));
        dbUpdateEntryString(de->d_name, k, line + strlen(k) + 1);
      }
      // Nuke +1 1027: Close for safe
      fclose(fp);
    }
  }
  closedir(d);
  return 0;
}

int dbGetEntryCountRange(const char *table, const int count_start,
                         const int num, char *output, const int outlen) {
  int dbi = dbGetTableIndex(table, DB_INT_SORTED);
  int cur;
  int now_score = 0x7fffffff;
  if (dbi < 0)
    return -1;
  if (outlen < 1)
    return -1;
  output[0] = 0;
  cur = gDBEntry[dbt[dbi].toplinkindex].next;
  int i = 0;
  int r = 0;
  while (cur != -1) {
    if (gDBEntry[cur].ivalue != now_score) {
      r = i;
      now_score = gDBEntry[cur].ivalue;
    }
    if ((i >= count_start) && (i < (count_start + num))) {
      char tmp[1024];
      if ((i != count_start)) {
        strcatsafe(output, outlen, "|");
      }
      snprintf(tmp, sizeof(tmp), "%d,%d,%s,%d,%s", i, r, gDBEntry[cur].key,
               gDBEntry[cur].ivalue, gDBEntry[cur].charvalue);
      strcatsafe(output, outlen, tmp);
    }
    ++i;
    cur = gDBEntry[cur].next;
  }
  return 0;
}

int dbUpdateEntryString(const char *table, const char *key, const char *value) {
  const int dbi = dbGetTableIndex(table, DB_STRING);
  if (strlen(key) >= KEY_MAX) {
    log("dbUpdateEntryString: key is too long, key:%s\n", key);
    return -1;
  }
  if (strlen(value) >= CHARVALUE_MAX) {
    log("dbUpdateEntryString: charvalue is too long, charvalue:%s\n", value);
    return -1;
  }
  if (dbi < 0) {
    log("dbUpdateEntryString: dbGetTableIndex fail, table:%s\n", table);
    return -1;
  }
  const int hash_index = tableGetEntry(dbi, key);
  int dbind;
  if (hash_index < 0) {
    dbind = dbAllocNode();
    if (dbind < 0) {
      log("dbUpdateEntryString: dbAllocNode failed.\n");
      return -1;
    }
    strcpy(gDBEntry[dbind].key, key);
    strcpy(gDBEntry[dbind].charvalue, value);
    if (dbAppendNode(dbt[dbi].toplinkindex, dbind) < 0) {
      gDBEntry[dbind].use = 0;
      log("dbUpdateEntryString: dbAppendNode fail\n");
      return -1;
    }
    if (tableInsertNode(dbi, key, dbind) < 0) {
      dbReleaseNode(dbind);
      log("dbUpdateEntryString: tableInsertNode fail\n");
      return -1;
    }
  } else {
    dbind = dbt[dbi].hashtable[hash_index].dbind;
    strcpy(gDBEntry[dbind].charvalue, value);
  }
  dbt[dbi].updated = 1;
  return 0;
}

int dbGetEntryString(const char *table, const char *key, char *output,
                     const int outlen) {
  int dbi = dbGetTableIndex(table, DB_STRING);
  // Spock 2000/10/23
  if (strlen(key) >= KEY_MAX) {
    log("dbGetEntryString: key is too long, key:%s\n", key);
    return -1;
  }
  if (dbi < 0) {
    log("dbGetEntryString: dbGetTableIndex fail\n");
    return -1;
  }
  // Spock 2000/10/19
  const int hash_index = tableGetEntry(dbi, key);
  if (hash_index < 0) {
    log("err hash index < 0.\n");
    return -1;
  }
  const int entry_index = dbt[dbi].hashtable[hash_index].dbind;
  if (entry_index < 0) {
    log("error entry index < 0.\n");
    return -1;
  }
  snprintf(output, outlen, "%s", gDBEntry[entry_index].charvalue);
  return 0;
}

int dbDeleteEntryString(const char *table, const char *key) {
  int dbi = dbGetTableIndex(table, DB_STRING);
  if (strlen(key) >= KEY_MAX) {
    log("dbDeleteEntryString: key is too long, key:%s\n", key);
    return -1;
  }
  if (dbi < 0) {
    log("dbDeleteEntryString: dbGetTableIndex fail\n");
    return -1;
  }
  int r = dbExtractNodeByKey(dbi, key);
  if (r < 0) {
    log("dbDeleteEntryString: dbExtractNodeByKey failed for %s in %s\n", key,
        table);
    return -1;
  }
  dbt[dbi].updated = 1;
  log("删除人物 %s 来自表 %s\n", key, table);
  return 0;
}
