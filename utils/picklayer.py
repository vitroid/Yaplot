#!/usr/bin/env python

import sys

layers = [int(x) for x in sys.argv[1:]]

layer = 1
color = 0
while True:
    line = sys.stdin.readline()
    if len(line) == 0:
        break
    if line[0] == "#":
        continue
    # global command
    if line[0] in ("r", "@"):
        print(line, end="")
        continue
    cols = line.split()
    if len(cols) == 0:
        #new page; reset layer and color
        layer = 1
        color = 0
        continue
    if line[0] == "y":
        layer = int(cols[1])
        print(line, end="")
        continue
    else:
        if layer in layers:
            print(line, end="")

