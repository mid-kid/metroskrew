#!/bin/sh
set -eu

BINS="${BINS:-.}"

parse_ver() {
    sed -n 's/^Version \([0-9\.]*\) build \([0-9]*\).*$/\1-\2/p'
}

for x in $(cd "$BINS"; find -name '*.exe'); do
    ver="${x%/*}"
    ver="${x#./}"
    name="${x##*/}"
    name="${name%.*}"
    sum="$( cd "$BINS"; sha1sum "$x" | cut -f1 -d\ )"
    build="$(wine "$BINS/$x" | parse_ver)"
    echo "$name-$build $ver $sum"
done | LANG=C sort | xargs printf "  ['%s', '%s',\n    '%s'],\n"
