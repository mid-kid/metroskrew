#!/bin/sh

cd "$(echo "$(dirname "$0")/../subprojects"/mw-executables-*/)"
files="$(find -type f \! -type l -name "*.exe")"
rafind2 "$@" $files | sort
