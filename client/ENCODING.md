# Client text encoding

All configurations in `stoneage.vcxproj` compile C/C++ with `/utf-8`:
both source text and ordinary narrow string literals use UTF-8. `/we4828`
rejects invalid source bytes instead of silently accepting legacy encodings.
Save new source/header files as UTF-8 (with or without BOM). The resource
script also explicitly selects code page 65001.

Write readable literals such as `"技 %d: "` and `"[%d转]"`. Do not concatenate
GBK bytes with UTF-8 protocol text. `tests/utf8_literals.cpp` is compiled in
every configuration and asserts the actual bytes of these Chinese literals;
switching the execution character set back to CP936 fails compilation.

Game/protocol strings remain UTF-8. Convert at Win32 display boundaries and
use wide APIs such as `TextOutW` and `CreateFontW`. `CharacterSet=MultiByte`
is retained for compatibility with existing API declarations; it is not the
encoding setting for C/C++ literals. Do not switch the whole project to
`UNICODE` merely to obtain UTF-8 strings.

Remember that UTF-8 byte capacity and display width are different. Size
storage for multibyte text, and do not use byte truncation for visible labels.
Old data files or strings already corrupted before loading are not repaired
by the compiler option; they require a separate data migration.

After changing compiler encoding, rebuild the Client and restart it. Do not
reuse objects compiled with an older execution character set.
