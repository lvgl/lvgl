#!/bin/sh
cat ../../header.py $1 > test.py
chmod +x test.py
./test.py
