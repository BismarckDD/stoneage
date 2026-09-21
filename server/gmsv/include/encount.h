#ifndef __ENCOUNT_H__
#define __ENCOUNT_H__

#define ENCOUNT_GROUPMAXNUM 10

BOOL ENCOUNT_initEncount(char *filename);
BOOL ENCOUNT_reinitEncount(void);
int ENCOUNT_getEncountAreaArray(int floor, int x, int y);
int ENCOUNT_getEncountPercentMin(int char_index, int floor, int x, int y);
int ENCOUNT_getEncountPercentMax(int char_index, int floor, int x, int y);
int ENCOUNT_getCreateEnemyMaxNum(int floor, int x, int y);
int ENCOUNT_getEncountIndex(int floor, int x, int y);
int ENCOUNT_getEncountIndexFromArray(int array);
int ENCOUNT_getEncountPercentFromArray(int array);
int ENCOUNT_getCreateEnemyMaxNumFromArray(int array);
int ENCOUNT_getGroupIdFromArray(int array, int grouparray);
int ENCOUNT_getGroupProbFromArray(int array, int grouparray);
int ENCOUNT_getZorderFromArray(int array);

#ifdef _ADD_ENCOUNT // WON ADD 增加敌遭遇触发修件
typedef struct tagENCOUNT_Table {
  int index;
  int floor;
  int encountprob_min; /* エンカウント確率   (遇敌概率) */
  int encountprob_max; /* エンカウント確率   (遇敌概率) */
  int enemymaxnum;     /* どれだけ敵を作るか  (生成多少敌人) */
  int zorder;
  int groupid[ENCOUNT_GROUPMAXNUM];    /* グループNo  (组No) */
  int createprob[ENCOUNT_GROUPMAXNUM]; /* そのグループの出現   (该组的出现) */
  int event_now;
  int event_end;
  int enemy_group; // 怪物的group 编号
  RECT rect;
} ENCOUNT_Table;
ENCOUNT_Table *ENCOUNT_table;
#endif

#endif
