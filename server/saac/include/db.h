#ifndef __DB_H__
#define __DB_H__

int dbUpdateEntryInt(const char *table, const char *key, const int value, const char *info);
int dbUpdateEntryString(const char *table, const char *key, const char *value);
int dbDeleteEntryInt(const char *table, const char *key);
int dbDeleteEntryString(const char *table, const char *key);
int dbGetEntryInt(const char *table, const char *key, int *output);
int dbGetEntryString(const char *table, const char *key, char *output, const int outlen);
int dbGetEntryRank(char *table, char *key, int *r_out, int *c_out);
int dbGetEntryRankRange(char *table, int start, int end, char *output,
                        int outlen);
int dbGetEntryCountRange(const char *table, const int offset, const int limit, char *output,
                         const int outlen);

int dbRead(const char *dir);
int dbFlush(const char *dir);

#endif
