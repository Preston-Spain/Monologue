#!/usr/bin/env bash
# monologue.sh -- build and launch the Monologue project.
# The program itself now asks whether to run or test, and for how many ticks.
#   ./monologue.sh        - build, then launch
#   ./monologue.sh build  - just compile, don't launch
#   ./monologue.sh clean  - remove build artifacts

set -euo pipefail

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"

# EDIT THIS if main.cpp / Util.cpp / core_ai.cpp / core_ai.h don't live next
# to this script (e.g. change to "$SCRIPT_DIR/Test/Main").
SRC_DIR="$SCRIPT_DIR"

BUILD_DIR="$SCRIPT_DIR/build"
BIN="$BUILD_DIR/monologue"

CXX=g++
CXXFLAGS="-std=c++17 -Wall -Wextra"

build() {
    mkdir -p "$BUILD_DIR"
    echo "[build] compiling..."
    "$CXX" $CXXFLAGS "$SRC_DIR/main.cpp" "$SRC_DIR/core_ai.cpp" "$SRC_DIR/Util.cpp" -o "$BIN"
    echo "[build] done -> $BIN"
}

clean() {
    echo "[clean] removing build artifacts..."
    rm -rf "$BUILD_DIR"
    rm -f "$SRC_DIR"/*.o "$SRC_DIR"/*.obj "$SRC_DIR"/*.exe "$SRC_DIR"/*.ilk "$SRC_DIR"/*.pdb
    echo "[clean] done."
}

case "${1:-}" in
    build) build ;;
    clean) clean ;;
    "") build; echo; "$BIN" ;;
    *)
        echo "Usage: $0 [build|clean]"
        echo "  (no argument) - build, then launch (it asks run/test and ticks)"
        echo "  build - just compile"
        echo "  clean - remove build artifacts"
        exit 1
        ;;
esac