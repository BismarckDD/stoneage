#include <cassert>
#include <cstdint>
#include <cstdio>
#include <cstring>
const char *copyUtf8CharByNum(const char *, char *, int);
int getUtf8CharNum(const char *);

static void check(const char *src, int limit, const char *expected, size_t consumed) {
    char output[128];
    memset(output, 0x7f, sizeof(output));
    const char *next = copyUtf8CharByNum(src, output, limit);
    assert(next == src + consumed);
    assert(strcmp(output, expected) == 0);
    assert(getUtf8CharNum(output) <= (limit > 0 ? limit : 0));
    assert(output[strlen(expected) + 1] == 0x7f);
}

int main() {
    check("abc", 2, "ab", 2);
    check("abc", 3, "abc", 3);
    check("abc", 9, "abc", 3);
    check("", 2, "", 0);
    check("abc", 0, "", 0);
    check("abc", -1, "", 0);
    check(u8"中文A", 1, "", 0);
    check(u8"中文A", 3, u8"中", 3);
    check(u8"中文A", 5, u8"中文A", 7);
    check(u8"A中B", 2, "A", 1);
    check(u8"A中B", 3, u8"A中", 4);
    check(u8"Ａ，A", 3, u8"Ａ", 3);
    check(u8"é😀中", 2, u8"é😀", 6); // Other code points count as one, as in getUtf8CharNum.
    check("A\xE4\xB8", 9, "A", 1); // Incomplete final character.
    check("A\xFF", 9, "A", 1);
    const char *text = u8"中文AB中";
    const char *next = text;
    char part[32], joined[64] = "";
    while (*next) {
        const char *previous = next;
        next = copyUtf8CharByNum(next, part, 3);
        assert(next > previous);
        strcat(joined, part);
    }
    assert(next == text + strlen(text) && *next == '\0');
    assert(strcmp(joined, text) == 0);
    puts("UTF-8 copy regression tests passed.");
}
