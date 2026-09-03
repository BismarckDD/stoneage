// Compiled in every Client configuration. No runtime dependencies.
// Both the source and ordinary narrow literals must be UTF-8: these literals
// are concatenated with UTF-8 protocol text before the UTF-16 display boundary.
static_assert(sizeof("转") == 4, "Client requires /utf-8");
static_assert((unsigned char)"转"[0] == 0xE8 &&
              (unsigned char)"转"[1] == 0xBD &&
              (unsigned char)"转"[2] == 0xAC,
              "Client execution character set must be UTF-8");
static_assert(sizeof("技 %d: ") == 9, "Skill labels must be UTF-8");
static_assert((unsigned char)"技"[0] == 0xE6 &&
              (unsigned char)"技"[1] == 0x8A &&
              (unsigned char)"技"[2] == 0x80,
              "Skill prefixes must use the protocol character set");
