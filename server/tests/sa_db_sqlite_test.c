#include "sa_db.h"

#include <assert.h>
#include <stdio.h>

int main(void) {
  SA_DB *db;
  SA_DB_RESULT *result;
  SA_DB_ROW row;

  remove("sa_db_sqlite_test.sqlite3");
  sa_db_configure(SA_DB_SQLITE, "sa_db_sqlite_test.sqlite3");
  db = sa_db_init(NULL);
  assert(db != NULL);
  assert(sa_db_connect(db, NULL, NULL, NULL, NULL, 0) != NULL);
  assert(sa_db_query(db, "CREATE TABLE users(name TEXT, LoginTime TEXT)") == 0);
  assert(sa_db_query(db, "INSERT INTO users VALUES(BINARY'alice', NOW())") == 0);
  assert(sa_db_query(db, "SELECT name, TO_DAYS(NOW())-TO_DAYS(LoginTime) "
                         "FROM users WHERE name=BINARY'alice'") == 0);
  result = sa_db_store_result(db);
  assert(result != NULL);
  assert(sa_db_num_rows(result) == 1);
  row = sa_db_fetch_row(result);
  assert(row != NULL && row[0] != NULL && row[1] != NULL);
  assert(row[0][0] == 'a' && row[1][0] == '0');
  sa_db_free_result(result);
  sa_db_close(db);
  remove("sa_db_sqlite_test.sqlite3");
  puts("SAAC SQLite compatibility test passed");
  return 0;
}
