#!/usr/bin/env python3
import sys

with open(sys.argv[1], 'r') as file:
    s = file.read()

b = bytearray()
b.extend(map(ord, s))
b.append(0x00)

for a in b: print(hex(a), end =", ")

