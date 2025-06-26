#!/bin/sh

set -e

ORIGINAL_PWD=$(pwd)
SCRIPT=$(readlink -f $0)
SCRIPT_PATH=$(dirname $SCRIPT)

cd $SCRIPT_PATH 
python3 ../../benchmark_results_comment.py --new no_mpk/results*.json -o no_mpk/actual.md
python3 ../../benchmark_results_comment.py --previous normal/results*.mpk --new normal/results*.json -o normal/actual.md
cd $ORIGINAL_PWD


cmp $SCRIPT_PATH/no_mpk/expected.md $SCRIPT_PATH/no_mpk/actual.md
cmp $SCRIPT_PATH/normal/expected.md $SCRIPT_PATH/normal/actual.md

