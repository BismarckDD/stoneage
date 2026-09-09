# Shared Lua 5.1 runtime

This directory is the single Lua runtime source used by both native programs.

- `client/lua51.vcxproj` builds `lua51-client.lib` with MSVC for Win32.
- `server/CMakeLists.txt` builds `liblua51_gmsv.a` with MinGW-w64 for GMSV.
- Client-only bindings remain in `client/stoneage/mylua`.
- Server-only bindings remain in `server/gmsv/mylua` and `server/gmsv/npc_lua`.

The two applications intentionally share source and public headers, but not
binary libraries: their architectures, ABIs, and C runtimes differ.
