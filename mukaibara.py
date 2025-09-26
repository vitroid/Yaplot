import sys
import numpy as np

print("a 1")
for line in sys.stdin:
    if len(line) == 1:
        print("")
        print("a 1")
    else:
        cols = line.split()
        cols[0] = "a"
        tail = 4 * np.array([int(x) for x in cols[1:4]])
        dir = 4 * np.array([int(x) for x in cols[4:7]])
        head = tail + dir
        if dir @ dir > 1:
            print("s", *tail, *head)
