#!/usr/bin/env python

from argparse import ArgumentParser
import os


def save_array_c(src_path, dst_path, symbol):
    length = os.path.getsize(src_path)

    with open(src_path, 'rb') as src_f:
        with open(dst_path, 'w') as dst_f:
            dst_f.write('#include <stddef.h>\n')
            dst_f.write('#include <stdint.h>\n')
            dst_f.write('\n')
            dst_f.write(f'const size_t {symbol}_len = {length};\n')
            dst_f.write(f'const __attribute__((aligned(4))) uint8_t {symbol}[] = {{\n')
            while True:
                data = src_f.read(12)
                if not data:
                    break
                data = [f'0x{byte:02X}' for byte in data]
                s = ', '.join(data)
                dst_f.write(f'    {s},\n')
            dst_f.write('};\n')

def save_asm_c(src_path, dst_path, symbol):
    length = os.path.getsize(src_path)

    with open(dst_path, 'w') as dst_f:
        dst_f.write('asm (\n')
        dst_f.write('    ".section .rodata\\n"\n')
        dst_f.write('    ".balign 4\\n"\n')
        dst_f.write(f'    ".global {symbol}_len\\n"\n')
        dst_f.write(f'    "{symbol}_len:\\n"\n')
        dst_f.write(f'    ".int {length}\\n"\n')
        dst_f.write(f'    ".global {symbol}\\n"\n')
        dst_f.write(f'    "{symbol}:\\n"\n')
        dst_f.write(f'    ".incbin \\"{src_path}\\"\\n"\n')
        dst_f.write('    ".balign 4\\n"\n')
        dst_f.write('    ".section .text\\n"\n')
        dst_f.write(');\n')

if __name__ == '__main__':
    parser = ArgumentParser()
    parser.add_argument('--use-array', action='store_true',
            help='use slower but portable array method')
    parser.add_argument('binary', metavar='BINARY', help='source binary data')
    parser.add_argument('output', metavar='OUTPUT', help='destination C source')
    args = parser.parse_args()

    symbol = os.path.basename(args.binary).translate(str.maketrans('-.', '__'))
    fn = save_array_c if args.use_array else save_asm_c
    fn(args.binary, args.output, symbol)
