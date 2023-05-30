#!/bin/bash

show_help() {
    echo "Usage: $0 <log_file> [trace_file]"
    echo "log_file: The input log file to process."
    echo "trace_file (optional): The output trace file. If not provided, defaults to 'trace.systrace'."
}

log_file="$1"
trace_file="$2"

if [ "$1" == "-h" ] || [ "$1" == "--help" ]; then
    show_help
    exit 0
fi

if [ -z "$log_file" ]; then
    echo "Missing log_file argument."
    show_help
    exit -1
fi

if [ ! -f "$log_file" ]; then 
    echo "log_file does not exist"
    exit -2
fi

if [ -z "$trace_file" ]; then 
    trace_file="trace.systrace"
fi

echo "log file: $log_file"
echo "trace file: $trace_file"

printf "# tracer: nop\n#\n" > "$trace_file"
grep -E ".*-[0-9]+\s[0-9]*" "$log_file" >> "$trace_file"
