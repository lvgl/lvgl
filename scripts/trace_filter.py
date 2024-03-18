#!/usr/bin/env python3

import argparse
import re
from pathlib import Path

MARK_LIST = ['tracing_mark_write']


def get_arg():
    parser = argparse.ArgumentParser(description='Filter a log file to a trace file.')
    parser.add_argument('log_file', metavar='log_file', type=str,
                        help='The input log file to process.')
    parser.add_argument('trace_file', metavar='trace_file', type=str, nargs='?',
                        help='The output trace file. If not provided, defaults to \'<log_file>.systrace\'.')

    args = parser.parse_args()
    return args


if __name__ == '__main__':
    args = get_arg()

    if not args.trace_file:
        log_file = Path(args.log_file)
        args.trace_file = log_file.with_suffix('.systrace').as_posix()

    print('log_file  :', args.log_file)
    print('trace_file:', args.trace_file)

    with open(args.log_file, 'r') as f:
        content = f.read()

    # compile regex pattern
    pattern = re.compile(r'(^.+-[0-9]+\s\[[0-9]]\s[0-9]+\.[0-9]+:\s('
                         + "|".join(MARK_LIST)
                         + r'):\s[B|E]\|[0-9]+\|.+$)', re.M)

    matches = pattern.findall(content)

    # write to args.trace_file
    with open(args.trace_file, 'w') as f:
        f.write('# tracer: nop\n#\n')
        for match in matches:
            f.write(match[0] + '\n')
