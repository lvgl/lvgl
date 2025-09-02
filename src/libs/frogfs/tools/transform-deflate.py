import sys
import zlib

if __name__ == '__main__':
    while True:
        chunk = sys.stdin.buffer.read()
        if not chunk:
            break
        compressed = zlib.compress(data)
        sys.stdout.buffer.write(compressed)
