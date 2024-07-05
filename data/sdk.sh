#!/bin/sh
set -eu

out="$1"; shift
rm -rf "$out"
while [ "$#" -ge 2 ]; do
    name="$1"; path="$2"; shift 2
    mkdir -p "$out/${path%/*}"
    printf %s\\n "$name" > "$out/$path.txt"
done
