#!/usr/bin/env python3
import sys, os.path
print(os.path.relpath(*sys.argv[1:3]))
