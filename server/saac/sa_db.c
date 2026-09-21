#include "sa_db.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "sqlite3.h"

#include <mysql/mysql.h>

struct SA_DB_RESULT {
  int backend;
  size_t rows;
  size_t cursor;
  int columns;
  char **sqlite_table;
  MYSQL_RES *mysql_result;
};

struct SA_DB {
  int backend;
  char error[512];
  char sqlite_path[512];
  sqlite3 *sqlite;
  SA_DB_RESULT *pending;
  MYSQL mysql;
};

static int configured_backend = SA_DB_MYSQL;
static char configured_sqlite_path[512] = "saac.sqlite3";

static void sqlite_now(sqlite3_context *context, int argc,
                       sqlite3_value **argv) {
  time_t current;
  struct tm *parts;
  char value[32];
  (void)argc;
  (void)argv;
  current = time(NULL);
  parts = localtime(&current);
  if (parts == NULL || strftime(value, sizeof(value), "%Y-%m-%d %H:%M:%S", parts) == 0)
    sqlite3_result_null(context);
  else
    sqlite3_result_text(context, value, -1, SQLITE_TRANSIENT);
}

static void sqlite_to_days(sqlite3_context *context, int argc,
                           sqlite3_value **argv) {
  const unsigned char *value;
  int year, month, day;
  static const int before_month[] =
      {0, 0, 31, 59, 90, 120, 151, 181, 212, 243, 273, 304, 334};
  long days;
  if (argc != 1 || sqlite3_value_type(argv[0]) == SQLITE_NULL) {
    sqlite3_result_null(context);
    return;
  }
  value = sqlite3_value_text(argv[0]);
  if (sscanf((const char *)value, "%d-%d-%d", &year, &month, &day) != 3 ||
      month < 1 || month > 12) {
    sqlite3_result_null(context);
    return;
  }
  days = 365L * year + year / 4 - year / 100 + year / 400 +
         before_month[month] + day;
  if (month <= 2 && year % 4 == 0 && (year % 100 != 0 || year % 400 == 0))
    days--;
  sqlite3_result_int64(context, days);
}

void sa_db_configure(int backend, const char *sqlite_path) {
  configured_backend = backend;
  if (sqlite_path != NULL && sqlite_path[0] != '\0') {
    snprintf(configured_sqlite_path, sizeof(configured_sqlite_path), "%s",
             sqlite_path);
  }
}

static void free_pending(SA_DB *db) {
  if (db != NULL && db->pending != NULL) {
    sa_db_free_result(db->pending);
    db->pending = NULL;
  }
}

SA_DB *sa_db_init(SA_DB *storage) {
  SA_DB *db = storage;
  if (db == NULL) db = (SA_DB *)calloc(1, sizeof(*db));
  if (db == NULL) return NULL;
  memset(db, 0, sizeof(*db));
  db->backend = configured_backend;
  snprintf(db->sqlite_path, sizeof(db->sqlite_path), "%s",
           configured_sqlite_path);
  if (db->backend == SA_DB_MYSQL && mysql_init(&db->mysql) == NULL) return NULL;
  return db;
}

SA_DB *sa_db_connect(SA_DB *db, const char *host, const char *user,
                     const char *password, const char *database,
                     unsigned int port) {
  if (db == NULL) return NULL;
  if (db->backend == SA_DB_SQLITE) {
    int rc = sqlite3_open_v2(db->sqlite_path, &db->sqlite,
                            SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE |
                                SQLITE_OPEN_FULLMUTEX,
                            NULL);
    if (rc != SQLITE_OK) {
      snprintf(db->error, sizeof(db->error), "%s",
               db->sqlite ? sqlite3_errmsg(db->sqlite) : "sqlite open failed");
      return NULL;
    }
    sqlite3_busy_timeout(db->sqlite, 5000);
    sqlite3_exec(db->sqlite, "PRAGMA journal_mode=WAL", NULL, NULL, NULL);
    sqlite3_exec(db->sqlite, "PRAGMA foreign_keys=ON", NULL, NULL, NULL);
    sqlite3_create_function(db->sqlite, "TO_DAYS", 1,
                            SQLITE_UTF8 | SQLITE_DETERMINISTIC, NULL,
                            sqlite_to_days, NULL, NULL);
    sqlite3_create_function(db->sqlite, "NOW", 0, SQLITE_UTF8, NULL,
                            sqlite_now, NULL, NULL);
    return db;
  }
  if (mysql_real_connect(&db->mysql, host, user, password, database, port, NULL,
                         0) == NULL) {
    snprintf(db->error, sizeof(db->error), "%s", mysql_error(&db->mysql));
    return NULL;
  }
  return db;
}

static char *sqlite_sql(const char *sql) {
  const char *needle = "BINARY";
  size_t n = strlen(sql);
  char *out = (char *)malloc(n + 1);
  char *w = out;
  const char *p = sql;
  if (out == NULL) return NULL;
  while (*p) {
    if (strncmp(p, needle, 6) == 0) {
      p += 6;
      continue;
    }
    *w++ = *p++;
  }
  *w = '\0';
  return out;
}

int sa_db_query(SA_DB *db, const char *sql) {
  if (db == NULL) return 1;
  free_pending(db);
  if (db->backend == SA_DB_SQLITE) {
    char *translated = sqlite_sql(sql);
    char *message = NULL;
    char **table = NULL;
    int rows = 0, columns = 0;
    int rc;
    if (translated == NULL) return 1;
    rc = sqlite3_get_table(db->sqlite, translated, &table, &rows, &columns,
                           &message);
    free(translated);
    if (rc != SQLITE_OK) {
      snprintf(db->error, sizeof(db->error), "%s",
               message ? message : sqlite3_errmsg(db->sqlite));
      sqlite3_free(message);
      if (table) sqlite3_free_table(table);
      return 1;
    }
    db->pending = (SA_DB_RESULT *)calloc(1, sizeof(*db->pending));
    if (db->pending == NULL) {
      sqlite3_free_table(table);
      snprintf(db->error, sizeof(db->error), "out of memory");
      return 1;
    }
    db->pending->backend = SA_DB_SQLITE;
    db->pending->rows = (size_t)rows;
    db->pending->columns = columns;
    db->pending->sqlite_table = table;
    return 0;
  }
  if (mysql_query(&db->mysql, sql) != 0) {
    snprintf(db->error, sizeof(db->error), "%s", mysql_error(&db->mysql));
    return 1;
  }
  db->pending = (SA_DB_RESULT *)calloc(1, sizeof(*db->pending));
  if (db->pending == NULL) return 1;
  db->pending->backend = SA_DB_MYSQL;
  db->pending->mysql_result = mysql_store_result(&db->mysql);
  db->pending->rows = db->pending->mysql_result
                          ? (size_t)mysql_num_rows(db->pending->mysql_result)
                          : 0;
  return 0;
}

SA_DB_RESULT *sa_db_store_result(SA_DB *db) {
  SA_DB_RESULT *result;
  if (db == NULL) return NULL;
  result = db->pending;
  db->pending = NULL;
  return result;
}

size_t sa_db_num_rows(SA_DB_RESULT *result) {
  return result ? result->rows : 0;
}

SA_DB_ROW sa_db_fetch_row(SA_DB_RESULT *result) {
  if (result == NULL || result->cursor >= result->rows) return NULL;
  if (result->backend == SA_DB_SQLITE) {
    char **row = result->sqlite_table + result->columns * (result->cursor + 1);
    result->cursor++;
    return row;
  }
  result->cursor++;
  return mysql_fetch_row(result->mysql_result);
}

void sa_db_free_result(SA_DB_RESULT *result) {
  if (result == NULL) return;
  if (result->backend == SA_DB_SQLITE) {
    if (result->sqlite_table) sqlite3_free_table(result->sqlite_table);
  }
  else if (result->mysql_result) mysql_free_result(result->mysql_result);
  free(result);
}

const char *sa_db_error(SA_DB *db) { return db ? db->error : "no database"; }

void sa_db_close(SA_DB *db) {
  if (db == NULL) return;
  free_pending(db);
  if (db->backend == SA_DB_SQLITE) {
    if (db->sqlite) sqlite3_close(db->sqlite);
    db->sqlite = NULL;
    free(db);
    return;
  }
  mysql_close(&db->mysql);
  free(db);
}
