#include "version.h"
#define __NPCCREATE__

#include "common.h"
//
#include "autil.h"
#include "buf.h"
#include "char_data.h"
#include "config_file.h"
#include "handletime.h"
#include "npccreate.h"
#include "npctemplate.h"
#include "readmap.h"
#include "util.h"

void NPC_setDefaultNPCCreate(NPC_Create *cr);

INLINE int NPC_CHECKCREATEINDEX(int index) {
  return (NPC_createnum <= index || index < 0) ? FALSE : TRUE;
}

INLINE int NPC_CHECKCREATEINTINDEX(int index) {
  return (NPC_CREATEINTNUM <= index || index < 0) ? FALSE : TRUE;
}

INLINE int NPC_setCreateInt(int index, NPC_CREATEINT element, int data) {
  int buf;
  buf = NPC_create[index].intdata[element];
  NPC_create[index].intdata[element] = data;
  return buf;
}

INLINE int NPC_getCreateInt(int index, NPC_CREATEINT element) {
  return NPC_create[index].intdata[element];
}

BOOL NPC_initCreateArray(int createnum) {
  int i;
  NPC_createnum = createnum;
  NPC_create = (NPC_Create *)allocateMemory(sizeof(NPC_Create) * NPC_createnum);

  print("开启大小:%d. 创建数量:%d.\n", sizeof(NPC_Create), createnum);
  if (NPC_create == NULL) {
    return FALSE;
  }
  memset(NPC_create, 0, sizeof(NPC_Create) * NPC_createnum);
  for (i = 0; i < NPC_createnum; i++) {
    NPC_setDefaultNPCCreate(&NPC_create[i]);
  }
  NPC_create_readindex = 0;
  return TRUE;
}

void NPC_setDefaultNPCCreate(NPC_Create *cr) {
  if (cr == NULL)
    return;

  cr->intdata[NPC_CREATEFLOORID] = 0;
  cr->intdata[NPC_CREATEBORNLEFTUPX] = 0;
  cr->intdata[NPC_CREATEBORNLEFTUPY] = 0;
  cr->intdata[NPC_CREATEBORNRIGHTDOWNX] = 0;
  cr->intdata[NPC_CREATEBORNRIGHTDOWNY] = 0;
  cr->intdata[NPC_CREATEMOVELEFTUPX] = 0;
  cr->intdata[NPC_CREATEMOVELEFTUPY] = 0;
  cr->intdata[NPC_CREATEMOVERIGHTDOWNX] = 0;
  cr->intdata[NPC_CREATEMOVERIGHTDOWNY] = 0;

  cr->intdata[NPC_CREATEDIR] = 0;
  // jeffrey 1231
#ifdef _ADD_ACTION
  cr->intdata[NPC_CREATEACTION] = 0;
#endif
  cr->intdata[NPC_CREATEBASEIMAGENUMBER] = -1;
  cr->intdata[NPC_CREATETIME] = 0;
  cr->intdata[NPC_CREATEBORNNUM] = 0;
  cr->intdata[NPC_CREATEENEMYNUM] = 0;
  cr->intdata[NPC_CREATEBOUNDARY] = 1;
  cr->intdata[NPC_CREATEIGNOREINVINCIBLE] = 0;
  cr->intdata[NPC_CREATEDATE] = 0;
  cr->intdata[NPC_CREATEFAMILY] = 0;
  cr->chardata[NPC_CREATENAME].string[0] = '\0';
  cr->workdata[NPC_CREATEWORKENEMYNUM] = 0;
  cr->workdata[NPC_CREATEWORKMAKESTARTSEC] = 0;
  cr->workdata[NPC_CREATEWORKMAKESTARTUSEC] = 0;
  cr->workdata[NPC_CREATEWORKNEVERMAKE] = 0;

#ifdef _NPC_AUTO_MOVE
  cr->intdata[NPC_AUTOMOVELEFTUPX] = 0;
  cr->intdata[NPC_AUTOMOVELEFTUPY] = 0;
  cr->intdata[NPC_AUTOMOVERIGHTDOWNX] = 0;
  cr->intdata[NPC_AUTOMOVERIGHTDOWNY] = 0;
#endif
  {
    int i;
    for (i = 0; i < arraysizeof(cr->templateindex); i++) {
      cr->templateindex[i] = -1;
      cr->arg[i].string[0] = '\0';
    }
  }
}

BOOL NPC_IsNPCCreateFile(char *filename) {
  FILE *f;
  char line1[128];
  char *ret;
  if (filename == NULL || strlen(filename) < 1 ||
      filename[strlen(filename) - 1] == '~' || filename[0] == '#' ||
      strcmptail(filename, ".bak") == 0)
    return FALSE;
  f = fopen(filename, "r");
  if (f == NULL)
    goto RETURNFALSE;
  ret = fgets(line1, sizeof(line1), f);
  if (ret == NULL)
    goto FCLOSERETURNFALSE;
  if (strcasecmp(NPC_CREATEFILEMAGIC, line1) == 0) {
    fclose(f);
    return TRUE;
  }

FCLOSERETURNFALSE:
  fclose(f);
RETURNFALSE:
  return FALSE;
}

int NPC_readCreateFile(char *filename) {
  FILE *f;
  char line[512];
  int linenum = 0;
  int start = OFF;
  NPC_Create cr;
  int enemyreadindex = 0;

  typedef struct tagPOINT {
    int x, y;
  } POINT;
  typedef struct tagREC {
    int w, h;
  } REC;
  POINT center[2] = {{0, 0}, {0, 0}}; /*  born 互 0 匹 move 互 1 */
  REC wh[2] = {{0, 0}, {0, 0}};       /*  born 互 0 匹 move 互 1 */

  POINT lu[2] = {{0, 0}, {0, 0}}; /*  born 互 0 匹 move 互 1 */
  POINT rd[2] = {{0, 0}, {0, 0}}; /*  born 互 0 匹 move 互 1 */

  int defborn = FALSE;
  int defmove = FALSE;

#ifdef _NPC_AUTO_MOVE
  POINT automove[2] = {{0, 0}, {0, 0}};
#endif
  int deflurd[2] = {FALSE, FALSE}; /*  lu,rd 匹涩烂仄凶井升丹井 */
  char *ret;

  if (NPC_create_readindex >= NPC_createnum) {
    print("创建超过配置缓冲\n");
    print("配置缓冲数目是 %d\n", NPC_createnum);
    print("没用文件 %s\n", filename);
    return FALSE;
  }

  NPC_setDefaultNPCCreate(&cr);
#ifdef _CRYPTO_DATA
  BOOL crypto = FALSE;
  if (strcmptail(filename, ".allblues") == 0) {
    crypto = TRUE;
  }
#endif
  f = fopen(filename, "r");
  if (f == NULL)
    return FALSE;

  ret = fgets(line, sizeof(line), f);
  if (ret == NULL)
    goto FCLOSERETURNFALSE;
#ifdef _CRYPTO_DATA
  if (crypto == TRUE) {
    DecryptKey(line);
  }
#endif
  if (strcmp(NPC_CREATEFILEMAGIC, line) != 0) {
    print("这不是一个create文件.\n");
    goto FCLOSERETURNFALSE;
  }
  linenum = 1;

  while (fgets(line, sizeof(line), f)) {
#ifdef _CRYPTO_DATA
    if (crypto == TRUE) {
      DecryptKey(line);
    }
#endif
    linenum++;

    if (line[0] == '#')
      continue; /* comment */
    if (line[0] == '\n')
      continue; /* none    */
    chomp(line);

    switch (line[0]) {
    case '{':
      if (start == ON) {
        printEx("Find {. But already START state. %s:%d\n", filename, linenum);
        printEx("退出\n");
        goto FCLOSERETURNFALSE;

      } else {
        NPC_setDefaultNPCCreate(&cr);
        start = ON;
      }
      break;
    case '}':
      if (start == ON) {
        int err = FALSE;
        if (enemyreadindex == 0) {
          err = TRUE;
          print("这不是遇敌数据  %s:%d\n", filename, linenum);
        } else if (MAP_IsThereSpecificFloorid(cr.intdata[NPC_CREATEFLOORID]) ==
                   FALSE) {
          err = TRUE;
          print("地图ID号有毛病 %s:%d-floor:%d\n", filename, linenum,
                cr.intdata[NPC_CREATEFLOORID]);
        } else if (defborn == FALSE) {
          err = TRUE;
          print("这些不是 born 安装 %s:%d\n", filename, linenum);
        }

        if (err == FALSE) {
          cr.intdata[NPC_CREATEENEMYNUM] = enemyreadindex;

          if (deflurd[0]) {
            cr.intdata[NPC_CREATEBORNLEFTUPX] = min(lu[0].x, rd[0].x);
            cr.intdata[NPC_CREATEBORNRIGHTDOWNX] = max(lu[0].x, rd[0].x);
            cr.intdata[NPC_CREATEBORNLEFTUPY] = min(lu[0].y, rd[0].y);
            cr.intdata[NPC_CREATEBORNRIGHTDOWNY] = max(lu[0].y, rd[0].y);
          } else {
            cr.intdata[NPC_CREATEBORNLEFTUPX] = center[0].x - wh[0].w / 2;
            cr.intdata[NPC_CREATEBORNRIGHTDOWNX] = center[0].x + wh[0].w / 2;
            cr.intdata[NPC_CREATEBORNLEFTUPY] = center[0].y - wh[0].h / 2;
            cr.intdata[NPC_CREATEBORNRIGHTDOWNY] = center[0].y + wh[0].h / 2;
          }
          if (defmove) {
            if (deflurd[0]) {
              cr.intdata[NPC_CREATEMOVELEFTUPX] = min(lu[1].x, rd[1].x);
              cr.intdata[NPC_CREATEMOVERIGHTDOWNX] = max(lu[1].x, rd[1].x);
              cr.intdata[NPC_CREATEMOVELEFTUPY] = min(lu[1].y, rd[1].y);
              cr.intdata[NPC_CREATEMOVERIGHTDOWNY] = max(lu[1].y, rd[1].y);
            } else {
              cr.intdata[NPC_CREATEMOVELEFTUPX] = center[1].x - wh[1].w / 2;
              cr.intdata[NPC_CREATEMOVERIGHTDOWNX] = center[1].x + wh[1].w / 2;
              cr.intdata[NPC_CREATEMOVELEFTUPY] = center[1].y - wh[1].h / 2;
              cr.intdata[NPC_CREATEMOVERIGHTDOWNY] = center[1].y + wh[1].h / 2;
            }

          } else {
            /*  born 毛戊疋□允月 */
            cr.intdata[NPC_CREATEMOVELEFTUPX] =
                cr.intdata[NPC_CREATEBORNLEFTUPX];
            cr.intdata[NPC_CREATEMOVERIGHTDOWNX] =
                cr.intdata[NPC_CREATEBORNRIGHTDOWNX];
            cr.intdata[NPC_CREATEMOVELEFTUPY] =
                cr.intdata[NPC_CREATEBORNLEFTUPY];
            cr.intdata[NPC_CREATEMOVERIGHTDOWNY] =
                cr.intdata[NPC_CREATEBORNRIGHTDOWNY];
          }

          if (NPC_create_readindex >= NPC_createnum) {
            printEx("NPC create capacity exceeded in file: %s\n", filename);
            goto FCLOSERETURNFALSE;
          }
          memcpy(&NPC_create[NPC_create_readindex], &cr, sizeof(NPC_Create));

          NPC_create_readindex++;

          if (NPC_create_readindex > NPC_createnum) {
            print("创建超过配置缓冲\n");
            print("配置创建文件数目 %d\n", NPC_createnum);
            print("没用文件:%s\n", filename);
            goto FCLOSERETURNFALSE;
          }
        }

        NPC_setDefaultNPCCreate(&cr);
        enemyreadindex = 0;
        defborn = FALSE;
        defmove = FALSE;
        deflurd[0] = FALSE;
        deflurd[1] = FALSE;
        start = OFF;
      } else {
        printEx("未解决 '}' at %s:%d\n", filename, linenum);
        goto FCLOSERETURNFALSE;
      }
      break;
    default: {

      char firstToken[256];
      char secondToken[256];
      int ret;
      ret = getStringFromIndexWithDelim(line, "=", 1, firstToken,
                                        sizeof(firstToken));
      if (ret == FALSE) {
        print("Find error at %s in line %d. Ignore\n", filename, linenum);
        break;
      }
      ret = getStringFromIndexWithDelim(line, "=", 2, secondToken,
                                        sizeof(secondToken));
      if (ret == FALSE) {
        print("Find error at %s in line %d. Ignore\n", filename, linenum);
        break;
      }

      if (strcasecmp("floorid", firstToken) == 0) {
        cr.intdata[NPC_CREATEFLOORID] = atoi(secondToken);

      } else if (strcasecmp("borncenter", firstToken) == 0) {
        getFourIntsFromString(secondToken, &center[0].x, &center[0].y, &wh[0].w,
                              &wh[0].h);
        defborn = TRUE;
      } else if (strcasecmp("borncorner", firstToken) == 0) {
        getFourIntsFromString(secondToken, &lu[0].x, &lu[0].y, &rd[0].x,
                              &rd[0].y);
        defborn = TRUE;
        deflurd[0] = TRUE;
      } else if (strcasecmp("movecenter", firstToken) == 0) {
        getFourIntsFromString(secondToken, &center[1].x, &center[1].y, &wh[1].w,
                              &wh[1].h);
        defmove = TRUE;
      } else if (strcasecmp("movecorner", firstToken) == 0) {
        getFourIntsFromString(secondToken, &lu[1].x, &lu[1].y, &rd[1].x,
                              &rd[1].y);
        defmove = TRUE;
        deflurd[1] = TRUE;

      }
#ifdef _NPC_AUTO_MOVE
      else if (strcasecmp("automove", firstToken) == 0) {
        getFourIntsFromString(secondToken, &automove[0].x, &automove[0].y,
                              &automove[1].x, &automove[1].y);
        cr.intdata[NPC_AUTOMOVELEFTUPX] = min(automove[0].x, automove[1].x);
        cr.intdata[NPC_AUTOMOVERIGHTDOWNX] = max(automove[0].x, automove[1].x);
        cr.intdata[NPC_AUTOMOVELEFTUPY] = min(automove[0].y, automove[1].y);
        cr.intdata[NPC_AUTOMOVERIGHTDOWNY] = max(automove[0].y, automove[1].y);
      }
#endif

      else if (strcasecmp("dir", firstToken) == 0) {
        cr.intdata[NPC_CREATEDIR] = atoi(secondToken);

      } else if (strcasecmp("graphicname", firstToken) == 0) {
        cr.intdata[NPC_CREATEBASEIMAGENUMBER] =
            CHAR_seekGraphicNumberFromString(secondToken);

      } else if (strcasecmp("name", firstToken) == 0) {
        strncpysafe(cr.chardata[NPC_CREATENAME].string,
                   sizeof(cr.chardata[NPC_CREATENAME].string), secondToken);

      } else if (strcasecmp("time", firstToken) == 0) {
        cr.intdata[NPC_CREATETIME] = atoi(secondToken);

      } else if (strcasecmp("date", firstToken) == 0) {
        cr.intdata[NPC_CREATEDATE] = atoi(secondToken);

      } else if (strcasecmp("createnum", firstToken) == 0) {
        cr.intdata[NPC_CREATEBORNNUM] = atoi(secondToken);

      } else if (strcasecmp("boundary", firstToken) == 0) {
        cr.intdata[NPC_CREATEBOUNDARY] = atoi(secondToken);

      } else if (strcasecmp("ignoreinvincible", firstToken) == 0) {
        cr.intdata[NPC_CREATEIGNOREINVINCIBLE] = atoi(secondToken);

        // jeffrey 1231
#ifdef _ADD_ACTION
      } else if (strcasecmp("action", firstToken) == 0) {
        cr.intdata[NPC_CREATEACTION] = atoi(secondToken);
#endif
        // Robin 0731
      } else if (strcasecmp("family", firstToken) == 0) {
        cr.intdata[NPC_CREATEFAMILY] = atoi(secondToken);

      } else if (strcasecmp("enemy", firstToken) == 0) {
        int templateindex;
        char enemyname[64];

        /*  雁钗瞬民尼永弁  */
        if (enemyreadindex >= arraysizeof(cr.templateindex)) {
          printEx("Too many enemy entries in %s:%d (max:%d)\n", filename,
                 linenum, arraysizeof(cr.templateindex));
          goto FCLOSERETURNFALSE;
        }

        getStringFromIndexWithDelim(secondToken, "|", 1, enemyname,
                                    sizeof(enemyname));
        templateindex = NPC_templateGetTemplateIndex(enemyname);
        if (templateindex != -1) {
          cr.templateindex[enemyreadindex] = templateindex;
          {
            char tmp[2];
            int ret;
            ret = getStringFromIndexWithDelim(secondToken, "|", 2, tmp,
                                              sizeof(tmp));
            if (ret == FALSE)
              cr.arg[enemyreadindex].string[0] = '\0';
            else
              strncpysafe(cr.arg[enemyreadindex].string,
                         sizeof(cr.arg[enemyreadindex].string),
                         secondToken + strlen(enemyname) + 1);
            // print("[NPCCreate] enemy secondToken:%s, enemyname:%s, arg:%s\n",
            //       secondToken, enemyname, cr.arg[enemyreadindex].string);
          }
          enemyreadindex++;
        } else
          printEx("没有这种模块:\n[%s(%d)%s] enemy:%s\n", filename, linenum,
                 secondToken, enemyname);
      } else {
        printEx("没有这种登陆 %s %s:%d\n", firstToken, filename, linenum);
      }
    }
    }
  }

  fclose(f);
  return TRUE;

FCLOSERETURNFALSE:
  fclose(f);
  return FALSE;
}

BOOL NPC_readNPCCreateFiles(char *topdirectory, int createsize) {
  STRING64 *filenames;
  int filenum;
  int i;
  filenames =
      (STRING64 *)allocateMemory(sizeof(STRING64) * (int)getFilesearchnum());
  if (filenames == NULL) {
    print("读取NPC创建文件... 内存错误\n");
    return FALSE;
  }
  filenum = rgetFileName(topdirectory, filenames, getFilesearchnum());
  if (filenum == -1) {
    freeMemory(filenames);
    return FALSE;
  }
  if (filenum >= getFilesearchnum()) {
    print("文件搜索数目失败 %d\n", getFilesearchnum());
    while (1)
      ;
  }
  if (!NPC_initCreateArray(createsize)) {
    printEx("开启创建数组错误\n");
    freeMemory(filenames);
    return FALSE;
  }
  print("读取NPC创建文件...");
  for (i = 0; i < filenum; i++) {
    if (NPC_IsNPCCreateFile(filenames[i].string)) {
      if (NPC_readCreateFile(filenames[i].string) == FALSE) {
        printEx("Failed to read NPC create file: %s\n", filenames[i].string);
        freeMemory(filenames);
        return FALSE;
      }
    }
  }
  print("正确创建NPC %d ...", NPC_create_readindex);
  /* Do not expose unused capacity to the runtime generation loop. */
  NPC_createnum = NPC_create_readindex;
#ifdef DEBUG
  /* 99/4/8 By Kawata csv溥挚卞仄化支月 */
  print("Npc_Create's\n");
  for (i = 0; i < NPC_createnum; i++) {
    int j;
    /*print( "NPC_create[%d]:\n", i);*/
    print("%d,", i);
    for (j = 0; j < NPC_CREATECHARNUM; j++)
      print("%s,", NPC_create[i].chardata[j].string);
    /*print( "\tIntdata\t" );*/
    for (j = 0; j < NPC_CREATEINTNUM; j++)
      print("%d,", NPC_create[i].intdata[j]);
    /***print("\n" );
    print( "\tChardata\t" );***/
    for (j = 0; j < NPC_create[i].intdata[NPC_CREATEENEMYNUM]; j++)
      print("%d", NPC_create[i].templateindex[j]);
    /*print("\n" );*/
    print("\n");
  }
#endif /*DEBUG*/

  freeMemory(filenames);
  return TRUE;
}

/*------------------------------------------------------------
 * createindex 井日 boundary 互涩烂今木化中月井升丹井毛苇月
 * 娄醒
 *  createindex         int         create及奶件犯永弁旦
 * 忒曰袄
 *  涩烂今木化中月          TRUE(1)
 *  涩烂今木化中卅中        FALSE(0)
 ------------------------------------------------------------*/
BOOL NPC_isBoundarySet(int createindex) {
  if (NPC_CHECKCREATEINDEX(createindex) == FALSE)
    return FALSE;
  return NPC_create[createindex].intdata[NPC_CREATEBOUNDARY];
}
/*------------------------------------------------------------
 * 娄醒
 *  r                   RECT*       袄毛熬仃午月RECT 及禾奶件正
 * 忒曰袄
 ------------------------------------------------------------*/
BOOL NPC_createGetRECT(int createindex, RECT *r) {
  if (NPC_CHECKCREATEINDEX(createindex) == FALSE)
    return FALSE;

  r->x = NPC_create[createindex].intdata[NPC_CREATEMOVELEFTUPX];
  r->y = NPC_create[createindex].intdata[NPC_CREATEMOVELEFTUPY];
  r->width = NPC_create[createindex].intdata[NPC_CREATEMOVERIGHTDOWNX] -
             NPC_create[createindex].intdata[NPC_CREATEMOVELEFTUPX];
  r->height = NPC_create[createindex].intdata[NPC_CREATEMOVERIGHTDOWNY] -
              NPC_create[createindex].intdata[NPC_CREATEMOVELEFTUPY];
  return TRUE;
}

/*------------------------------------------------------------
 * 参数
 *  cindex int
 * 返回值
 *  BOOL    允许创建 TRUE(1)
 *  BOOL    不能创建 FALSE(0)
 ------------------------------------------------------------*/
BOOL NPC_createCheckGenerateFromTime(int cindex) {
  if (!NPC_CHECKCREATEINDEX(cindex))
    return FALSE;

  if (NPC_create[cindex].workdata[NPC_CREATEWORKNEVERMAKE])
    return FALSE;

  if (NPC_create[cindex].intdata[NPC_CREATEBORNNUM] <=
      NPC_create[cindex].workdata[NPC_CREATEWORKENEMYNUM])
    return FALSE;

  if (NPC_create[cindex].intdata[NPC_CREATETIME] < 0)
    return FALSE;

  struct timeval lastTime;
  lastTime.tv_sec = NPC_create[cindex].workdata[NPC_CREATEWORKMAKESTARTSEC];
  lastTime.tv_usec = NPC_create[cindex].workdata[NPC_CREATEWORKMAKESTARTUSEC];

  // print("%d, %d, %d, %d\n", NowTime.tv_sec, NowTime.tv_usec, lastTime.tv_sec, lastTime.tv_usec);
  // print("%d\n", NPC_create[cindex].intdata[NPC_CREATETIME]);
  if (time_diff_us(NowTime, lastTime) <=
      NPC_create[cindex].intdata[NPC_CREATETIME] * 1000) {
    // print("FFFF\n");
    return FALSE;
  }
  // print("TTTT\n");
  return TRUE;
}

/*------------------------------------------------------------
 * 参数
 *  tindex      int     奶件犯永弁旦
 * 返回值
 *  无
 ------------------------------------------------------------*/
void NPC_createInitTime(int index) {
  NPC_create[index].workdata[NPC_CREATEWORKMAKESTARTSEC] = NowTime.tv_sec;
  NPC_create[index].workdata[NPC_CREATEWORKMAKESTARTUSEC] = NowTime.tv_usec;
}

/*------------------------------------------------------------
 * 输入
 *  index int
 * 返回值
 *  无
 ------------------------------------------------------------*/
void NPC_createIncreaseEnemynum(int index) {
  if (!NPC_CHECKCREATEINDEX(index))
    return;
  NPC_create[index].workdata[NPC_CREATEWORKENEMYNUM]++;
}
/*------------------------------------------------------------
 * 
 * 参数
 *  index int
 * 返回值
 *  无
 ------------------------------------------------------------*/
void NPC_createDecreaseEnemynum(int index) {
  if (!NPC_CHECKCREATEINDEX(index))
    return;
  NPC_create[index].workdata[NPC_CREATEWORKENEMYNUM]--;
}
/*------------------------------------------------------------
 *
 *  index
 *  TRUE:MAX  FALSE:
 ------------------------------------------------------------*/
BOOL NPC_createCheckMaxEnemynum(int index) {
  if (NPC_create[index].intdata[NPC_CREATEBORNNUM] <=
      NPC_create[index].workdata[NPC_CREATEWORKENEMYNUM])
    return TRUE;
  else
    return FALSE;
}
