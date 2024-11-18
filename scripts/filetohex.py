#!/usr/bin/env python3
import sys
import textwrap
import re

with open(sys.argv[1], 'r') as file:
    s = file.read()

b = bytearray()

if '--filter-character' in sys.argv:
    s = re.sub(r'[^\x00-\xff]', '', s)
if '--null-terminate' in sys.argv:
    s += '\x00'

b.extend(map(ord, s))

print(textwrap.fill(', '.join([hex(a) for a in b]), 96))