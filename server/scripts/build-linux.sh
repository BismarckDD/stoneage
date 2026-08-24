#!/bin/bash
set -e

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
SERVER_DIR="$(cd "$SCRIPT_DIR/.." && pwd)"
ROOT_DIR="/home/franklin/"

BUILD_TARGET="all"
BUILD_TYPE="release"
JOBS="$(nproc 2>/dev/null || echo 4)"

usage() {
    echo "Usage: $0 [OPTIONS]"
    echo ""
    echo "Options:"
    echo "  -t, --target TARGET   Build target: all, common, gmsv, saac (default: all)"
    echo "  -b, --build-type TYPE Build type: debug, release (default: release)"
    echo "  -j, --jobs N          Parallel jobs (default: $JOBS)"
    echo "  -c, --clean           Clean before build"
    echo "  -h, --help            Show this help"
}

clean() {
    echo "=== Cleaning ==="
    make -C "$SERVER_DIR/common" clean 2>/dev/null || true
    make -C "$SERVER_DIR/gmsv" clean 2>/dev/null || true
    make -C "$SERVER_DIR/saac" clean 2>/dev/null || true
    echo "Clean complete."
}

check_deps() {
    local missing=0
    for cmd in gcc make ar ranlib; do
        if ! command -v "$cmd" &>/dev/null; then
            echo "Error: $cmd not found. Please install build essentials." >&2
            missing=1
        fi
    done
    if [ ! -f "/usr/lib/x86_64-linux-gnu/libmysqlclient.so" ] && \
       [ ! -f "/usr/lib/x86_64-linux-gnu/libmysqlclient.a" ]; then
        echo "Error: libmysqlclient not found. Install: sudo apt install libmysqlclient-dev" >&2
        missing=1
    fi
    if [ ! -f "$SERVER_DIR/lua/libluajit.a" ]; then
        echo "Error: LuaJIT library not found at $SERVER_DIR/lua/libluajit.a" >&2
        missing=1
    fi
    if [ $missing -eq 1 ]; then
        exit 1
    fi
    echo "=== Dependencies OK ==="
}

build_common() {
    echo "=== Building common libraries ($BUILD_TYPE) ==="
    make -C "$SERVER_DIR/common" BUILD="$BUILD_TYPE" -j"$JOBS"
    echo "=== Common libraries done ==="
}

build_gmsv() {
    echo "=== Building GMSV ($BUILD_TYPE) ==="
    make -C "$SERVER_DIR/gmsv" BUILD="$BUILD_TYPE" -j"$JOBS"
    echo "=== GMSV done: $ROOT_DIR/csa/gmsv/gmsv ==="
}

build_saac() {
    echo "=== Building SAAC ($BUILD_TYPE) ==="
    build_common
    make -C "$SERVER_DIR/saac" BUILD="$BUILD_TYPE" -j"$JOBS"
    echo "=== SAAC done: $ROOT_DIR/csa/saac/saac ==="
}

while [[ $# -gt 0 ]]; do
    case "$1" in
        -t|--target) BUILD_TARGET="$2"; shift 2 ;;
        -b|--build-type) BUILD_TYPE="$2"; shift 2 ;;
        -j|--jobs)   JOBS="$2"; shift 2 ;;
        -c|--clean)  DO_CLEAN=1; shift ;;
        -h|--help)   usage; exit 0 ;;
        *)           echo "Unknown option: $1"; usage; exit 1 ;;
    esac
done

case "$BUILD_TYPE" in
    debug|release) ;;
    *) echo "Error: Build type must be 'debug' or 'release'." >&2; exit 1 ;;
esac

# Rebuilding with a different BUILD_TYPE on top of old objects mixes
# debug/release code. Auto-clean when the type changes.
STAMP_FILE="$SERVER_DIR/.last-build-type"
LAST_BUILD_TYPE="$(cat "$STAMP_FILE" 2>/dev/null || true)"
if [ -n "$LAST_BUILD_TYPE" ] && [ "$LAST_BUILD_TYPE" != "$BUILD_TYPE" ]; then
    echo "=== Build type changed ($LAST_BUILD_TYPE -> $BUILD_TYPE), auto clean ==="
    DO_CLEAN=1
fi
printf '%s\n' "$BUILD_TYPE" > "$STAMP_FILE"

if [ "${DO_CLEAN:-0}" -eq 1 ]; then
    clean
fi

check_deps

case "$BUILD_TARGET" in
    all)
        build_common
        build_gmsv
        build_saac
        ;;
    common)
        build_common
        ;;
    gmsv)
        build_common
        build_gmsv
        ;;
    saac)
        build_saac
        ;;
    *)
        echo "Error: Unknown target '$BUILD_TARGET'" >&2
        usage
        exit 1
        ;;
esac

echo ""
echo "=== Build complete ==="
ls -lh "$ROOT_DIR/csa/gmsv/gmsv" 2>/dev/null && echo "  GMSV: csa/gmsv/gmsv" || true
ls -lh "$ROOT_DIR/csa/saac/saac" 2>/dev/null && echo "  SAAC: csa/saac/saac" || true
