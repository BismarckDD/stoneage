#ifndef SA_DB_H
#define SA_DB_H

#include <stddef.h>

typedef struct SA_DB SA_DB;
typedef struct SA_DB_RESULT SA_DB_RESULT;
typedef char **SA_DB_ROW;
typedef void SA_DB_FIELD;

enum { SA_DB_MYSQL = 0, SA_DB_SQLITE = 1 };

void sa_db_configure(int backend, const char *sqlite_path);
SA_DB *sa_db_init(SA_DB *db);
SA_DB *sa_db_connect(SA_DB *db, const char *host, const char *user,
                     const char *password, const char *database,
                     unsigned int port);
int sa_db_query(SA_DB *db, const char *sql);
SA_DB_RESULT *sa_db_store_result(SA_DB *db);
size_t sa_db_num_rows(SA_DB_RESULT *result);
SA_DB_ROW sa_db_fetch_row(SA_DB_RESULT *result);
void sa_db_free_result(SA_DB_RESULT *result);
const char *sa_db_error(SA_DB *db);
void sa_db_close(SA_DB *db);

#endif
