cmake_minimum_required(VERSION 3.20)

project(stoneage_server_windows LANGUAGES C)

if(NOT WIN32)
  message(FATAL_ERROR
    "This CMake entry point is for native Windows builds. Use the existing "
    "GNU makefiles when building on Linux.")
endif()

if(NOT MINGW)
  message(FATAL_ERROR
    "The legacy server requires GNU C extensions. Configure it with the "
    "MSYS2 MinGW-w64 (UCRT64) compiler, not MSVC.")
endif()

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS ON)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin")

string(TIMESTAMP STONEAGE_BUILD_TIME "%Y-%m-%d %H:%M:%S")
file(MAKE_DIRECTORY "${CMAKE_BINARY_DIR}/generated/gmsv")
file(MAKE_DIRECTORY "${CMAKE_BINARY_DIR}/generated/saac")
configure_file(cmake/copyright_gmsv.h.in
               generated/gmsv/copyright.h @ONLY)
configure_file(cmake/copyright_saac.h.in
               generated/saac/copyright.h @ONLY)

set(COMMON_UTIL_SOURCES
  common/utils/md5.c
  common/utils/util_file.c
  common/utils/util_string.c
  common/utils/util_time.c
  common/server_util.c
  common/tcp_buffer.c
  common/tcp_transport.c
  common/workspace.c
  common/windows_compat.c)

set(GMSV_ROOT_SOURCES
  gmsv/main.c
  gmsv/init.c
  gmsv/config_file.c
  gmsv/buf.c
  gmsv/msignal.c
  gmsv/shop.c
  gmsv/object.c
  gmsv/handletime.c
  gmsv/function.c
  gmsv/log.c
  gmsv/autil.c
  gmsv/descrypt.c
  gmsv/legacy_util.c
  gmsv/optional_features.c
  gmsv/const.c)

# The Lua binding layers are feature-gated in version.h.  They must not be
# compiled merely because their source directories exist: with the feature
# macros disabled most entry points are omitted, leaving unresolved symbols.
set(GMSV_COMPONENT_SOURCES)
foreach(component char npc net map item battle magic)
  file(GLOB component_sources CONFIGURE_DEPENDS
       "${CMAKE_CURRENT_SOURCE_DIR}/gmsv/${component}/*.c")
  list(APPEND GMSV_COMPONENT_SOURCES ${component_sources})
endforeach()
# ABLUA (gmsv/mylua) is feature-gated by _ALLBLUES_LUA* in version.h and is
# deliberately kept out of the loop above.  Opt in explicitly so these sources
# are only pulled in when the corresponding macros are actually enabled.
option(GMSV_ABLUA "Build the ABLUA (gmsv/mylua) Lua binding layer" ON)
if(GMSV_ABLUA)
  file(GLOB gmsv_mylua_sources CONFIGURE_DEPENDS
       "${CMAKE_CURRENT_SOURCE_DIR}/gmsv/mylua/*.c")
  list(APPEND GMSV_COMPONENT_SOURCES ${gmsv_mylua_sources})
endif()
# epoll is Linux-only. The default select-based network loop is used on Windows.
list(FILTER GMSV_COMPONENT_SOURCES EXCLUDE REGEX "[/\\\\]epollnet\\.c$")

add_executable(gmsv
  ${GMSV_ROOT_SOURCES}
  ${GMSV_COMPONENT_SOURCES}
  ${COMMON_UTIL_SOURCES}
  common/saac_client_send.c
  common/saac_client_recv.c
  common/gmsv_server_send.c
  common/gmsv_server_recv.c)

add_executable(saac
  saac/main.c
  saac/saac_config.c
  saac/tcp_struct.c
  saac/char.c
  saac/db.c
  saac/mail.c
  saac/acfamily.c
  saac/lock.c
  saac/sasql_cygwin.c
  ${COMMON_UTIL_SOURCES}
  common/saac_server_send.c
  common/saac_server_recv.c)

set(STONEAGE_WINDOWS_DEFINITIONS
  WIN32_LEAN_AND_MEAN
  NOMINMAX
  FD_SETSIZE=4096
  _WIN32_WINNT=0x0601
  _FILE_OFFSET_BITS=64
  _LARGEFILE64_SOURCE)

set(STONEAGE_GNU_OPTIONS
  -fcommon
  -fgnu89-inline
  -Wno-error
  -Wno-implicit-function-declaration
  -Wno-incompatible-pointer-types
  -Wno-int-conversion)

# --------------------------------------------------------------------------
# Lua
# --------------------------------------------------------------------------
# Client and GMSV consume one Lua 5.1 source tree, but each toolchain builds
# its own static library.  This avoids mixing the Client's MSVC/Win32 ABI with
# GMSV's MinGW-w64 ABI while keeping headers and runtime behavior in sync.
set(LUA_ROOT "${CMAKE_CURRENT_SOURCE_DIR}/../shared/lua51")
set(LUA_INCLUDE_DIR "${LUA_ROOT}")
set(LUA_SOURCES
  lapi.c lauxlib.c lbaselib.c lcode.c ldblib.c ldebug.c ldo.c ldump.c
  lfunc.c lgc.c linit.c liolib.c llex.c lmathlib.c lmem.c loadlib.c
  lobject.c lopcodes.c loslib.c lparser.c lstate.c lstring.c lstrlib.c
  ltable.c ltablib.c ltm.c lundump.c lvm.c lzio.c)
list(TRANSFORM LUA_SOURCES PREPEND "${LUA_ROOT}/")
add_library(lua51_gmsv STATIC ${LUA_SOURCES})
target_include_directories(lua51_gmsv PUBLIC "${LUA_INCLUDE_DIR}")

target_include_directories(gmsv PRIVATE
  "${CMAKE_BINARY_DIR}/generated/gmsv"
  "${CMAKE_CURRENT_SOURCE_DIR}/gmsv/include"
  "${CMAKE_CURRENT_SOURCE_DIR}/gmsv"
  "${CMAKE_CURRENT_SOURCE_DIR}"
  "${CMAKE_CURRENT_SOURCE_DIR}/common"
  "${CMAKE_CURRENT_SOURCE_DIR}/common/utils"
  "${LUA_INCLUDE_DIR}")
target_include_directories(saac PRIVATE
  "${CMAKE_BINARY_DIR}/generated/saac"
  "${CMAKE_CURRENT_SOURCE_DIR}/saac/include"
  "${CMAKE_CURRENT_SOURCE_DIR}/saac"
  "${CMAKE_CURRENT_SOURCE_DIR}/common"
  "${CMAKE_CURRENT_SOURCE_DIR}/common/utils")

foreach(target gmsv saac)
  target_compile_definitions(${target} PRIVATE ${STONEAGE_WINDOWS_DEFINITIONS})
  target_compile_options(${target} PRIVATE ${STONEAGE_GNU_OPTIONS})
  target_link_libraries(${target} PRIVATE ws2_32)
endforeach()

# The Windows SAAC implementation links directly to MySQL via libmysqlclient.
# Install via MSYS2: pacman -S mingw-w64-ucrt-x86_64-libmysqlclient
find_package(PkgConfig QUIET)
if(PKG_CONFIG_FOUND)
  pkg_check_modules(MYSQL IMPORTED_TARGET mysqlclient)
endif()
if(MYSQL_FOUND)
  target_link_libraries(saac PRIVATE PkgConfig::MYSQL)
else()
  # Fallback: assume headers are in ucrt64/include/mysql and libs in ucrt64/lib
  set(MYSQL_ROOT "C:/msys64/ucrt64")
  if(DEFINED ENV{MSYS2_ROOT})
    set(MYSQL_ROOT "$ENV{MSYS2_ROOT}/ucrt64")
  endif()
  target_include_directories(saac PRIVATE "${MYSQL_ROOT}/include")
  target_link_directories(saac PRIVATE "${MYSQL_ROOT}/lib")
  target_link_libraries(saac PRIVATE mysqlclient)
endif()
target_link_libraries(saac PRIVATE z)
target_link_libraries(gmsv PRIVATE z m)
# Lua must come after the objects that reference it.
target_link_libraries(gmsv PRIVATE lua51_gmsv)

# Legacy SAAC handlers place several CHARDATASIZE (1 MiB) work buffers on the
# stack; some paths have four of them live at once.  GNU ld's Windows default
# reserve is only 2 MiB, which terminates the process with STATUS_STACK_OVERFLOW
# (0xC00000FD) after a game server starts sending database/family requests.
target_link_options(saac PRIVATE "LINKER:--stack,33554432")

# MinGW links zlib dynamically. Keep the runtime DLL next to gmsv.exe so the
# server can be launched from a deployment directory without an MSYS2 PATH.
get_filename_component(MINGW_RUNTIME_DIRECTORY "${CMAKE_C_COMPILER}" DIRECTORY)
set(MINGW_ZLIB_DLL "${MINGW_RUNTIME_DIRECTORY}/zlib1.dll")
if(EXISTS "${MINGW_ZLIB_DLL}")
  add_custom_command(TARGET gmsv POST_BUILD
    COMMAND "${CMAKE_COMMAND}" -E copy_if_different
            "${MINGW_ZLIB_DLL}" "$<TARGET_FILE_DIR:gmsv>/zlib1.dll"
    COMMENT "Copying the MinGW zlib runtime")
else()
  message(WARNING
    "zlib1.dll was not found beside the compiler. Copy the MinGW zlib runtime "
    "next to gmsv.exe before running it.")
endif()

install(TARGETS gmsv saac RUNTIME DESTINATION bin)
if(EXISTS "${MINGW_ZLIB_DLL}")
  install(FILES "${MINGW_ZLIB_DLL}" DESTINATION bin)
endif()

include(CTest)
if(BUILD_TESTING)
  add_executable(tcp_buffer_test
    tests/tcp_buffer_test.c
    common/tcp_buffer.c)
  target_include_directories(tcp_buffer_test PRIVATE
    "${CMAKE_CURRENT_SOURCE_DIR}/common")
  target_compile_definitions(tcp_buffer_test PRIVATE
    ${STONEAGE_WINDOWS_DEFINITIONS})
  target_compile_options(tcp_buffer_test PRIVATE ${STONEAGE_GNU_OPTIONS})
  add_test(NAME tcp_buffer_test COMMAND tcp_buffer_test)

  add_executable(protocol_security_test
    tests/protocol_security_test.c
    common/server_util.c
    common/workspace.c)
  target_include_directories(protocol_security_test PRIVATE
    "${CMAKE_CURRENT_SOURCE_DIR}/common"
    "${CMAKE_CURRENT_SOURCE_DIR}/common/utils")
  target_compile_definitions(protocol_security_test PRIVATE
    ${STONEAGE_WINDOWS_DEFINITIONS})
  target_compile_options(protocol_security_test PRIVATE ${STONEAGE_GNU_OPTIONS})
  add_test(NAME protocol_security_test COMMAND protocol_security_test)

  add_executable(windows_socket_test
    tests/windows_socket_test.c
    common/tcp_transport.c
    common/windows_compat.c)
  target_include_directories(windows_socket_test PRIVATE
    "${CMAKE_CURRENT_SOURCE_DIR}/common")
  target_compile_definitions(windows_socket_test PRIVATE
    ${STONEAGE_WINDOWS_DEFINITIONS})
  target_compile_options(windows_socket_test PRIVATE ${STONEAGE_GNU_OPTIONS})
  target_link_libraries(windows_socket_test PRIVATE ws2_32)
  add_test(NAME windows_socket_test COMMAND windows_socket_test)

  add_executable(saac_db_test
    tests/saac_db_test.c
    saac/db.c
    common/server_util.c
    common/workspace.c
    common/utils/util_file.c
    common/utils/util_string.c
    common/windows_compat.c)
  target_include_directories(saac_db_test PRIVATE
    "${CMAKE_CURRENT_SOURCE_DIR}/saac/include"
    "${CMAKE_CURRENT_SOURCE_DIR}/saac"
    "${CMAKE_CURRENT_SOURCE_DIR}/common"
    "${CMAKE_CURRENT_SOURCE_DIR}/common/utils")
  target_compile_definitions(saac_db_test PRIVATE
    ${STONEAGE_WINDOWS_DEFINITIONS})
  target_compile_options(saac_db_test PRIVATE ${STONEAGE_GNU_OPTIONS})
  target_link_libraries(saac_db_test PRIVATE ws2_32)
  add_test(NAME saac_db_test COMMAND saac_db_test)
endif()
