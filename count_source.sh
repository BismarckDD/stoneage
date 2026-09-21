#!/usr/bin/env bash
set -euo pipefail

root=$(cd -- "$(dirname -- "${BASH_SOURCE[0]}")" && pwd)

printf '%-10s %10s %14s %14s\n' 'Directory' 'Files' 'All lines' 'Nonblank lines'

total_files=0
total_lines=0
total_nonblank=0

for dir in client server; do
  files=0
  lines=0
  nonblank=0

  while IFS= read -r -d '' file; do
    read -r file_lines file_nonblank < <(
      awk 'NF { nonblank++ } END { print NR+0, nonblank+0 }' "$file"
    )
    files=$((files + 1))
    lines=$((lines + file_lines))
    nonblank=$((nonblank + file_nonblank))
  done < <(find "$root/$dir" -type d -name third_party -prune -o \
    -type f \( -name '*.h' -o -name '*.c' -o -name '*.cpp' \) -print0)

  printf '%-10s %10d %14d %14d\n' "$dir" "$files" "$lines" "$nonblank"
  total_files=$((total_files + files))
  total_lines=$((total_lines + lines))
  total_nonblank=$((total_nonblank + nonblank))
done

printf '%-10s %10d %14d %14d\n' 'Total' "$total_files" "$total_lines" "$total_nonblank"
