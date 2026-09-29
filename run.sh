#!/bin/sh
set -eu
cd "$(dirname "$0")"
mkdir -p runtime
if [ ! -x runtime/logharbor ] || [ src/logharbor.cpp -nt runtime/logharbor ]; then
  "${CXX:-c++}" -std=c++17 -O2 -Wall -Wextra -Wpedantic src/logharbor.cpp -o runtime/logharbor
fi
exec python3 server.py
