#!/bin/sh

set -e

ORIGINAL_PWD=$(pwd)
SCRIPT=$(readlink -f $0)
SCRIPT_PATH=$(dirname $SCRIPT)

cd $SCRIPT_PATH 
rm -rf output
mkdir output
python3 ../../serialize_results.py --input input --output output --commit-hash abc123456
cd $ORIGINAL_PWD


OUTPUT_DIR=$SCRIPT_PATH/output
EXPECTED_DIR=$SCRIPT_PATH/expected_output

OUTPUT_COUNT=$(ls -1q "$OUTPUT_DIR" | wc -l)
EXPECTED_COUNT=$(ls -1q "$EXPECTED_DIR" | wc -l)

if [ "$OUTPUT_COUNT" -ne "$EXPECTED_COUNT" ]; then
    echo "[TEST] Mismatch in number of files: Expected $EXPECTED_COUNT. Found $OUTPUT_COUNT"
    exit 1
fi

for file in "$EXPECTED_DIR"/*; do
    filename=$(basename "$file")
    output_file="$OUTPUT_DIR/$filename"

    if [ ! -f "$output_file" ]; then
        echo "[TEST] Missing file: $filename in output directory"
        exit 1
    fi

    if ! cmp -s "$file" "$output_file"; then
        echo "[TEST] File mismatch: $filename"
        exit 1
    fi
done

echo "[TEST] Test Passed"
exit 0
