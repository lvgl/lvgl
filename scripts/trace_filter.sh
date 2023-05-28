#!/bin/env bash

logfile=$1
outfile=$2

echo file: $logfile
if [ ! -f $logfile ]; then 
    echo "File does not exist"
    exit -2
fi

if [ ! -f "$outfile" ]; then 
    outfile=trace.systrace
fi
echo "outfile: $outfile"

printf "# trace: nop\n#\n" > $outfile
cat $logfile | grep -E ".*-[0-9]+\s[0-9]*" >> $outfile
