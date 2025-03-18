#!/bin/sh

set -e

ORIGINAL_PWD=$(pwd)
SCRIPT=$(readlink -f $0)
SCRIPT_PATH=$(dirname $SCRIPT)

cd $SCRIPT_PATH 
cat docker-logs-example.txt | python3 ../../filter_docker_benchmark_logs.py actual.json
cd $ORIGINAL_PWD


cmp $SCRIPT_PATH/expected.json $SCRIPT_PATH/actual.json

