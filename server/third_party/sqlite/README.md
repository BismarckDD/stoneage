# SQLite amalgamation

This directory vendors the official SQLite 3.51.2 amalgamation (`sqlite3.c`
and `sqlite3.h`) from <https://www.sqlite.org/2026/sqlite-amalgamation-3510200.zip>.
SQLite is in the public domain; its source header contains the upstream
dedication and warranty notice.

SAAC compiles this source directly so its SQLite backend has no runtime SQLite
library dependency.
