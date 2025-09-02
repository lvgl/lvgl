import sys
import brotli

if __name__ == '__main__':
    while True:
        chunk = sys.stdin.buffer.read()
        if not chunk:
            break
        compressed = brotli.compress(chunk)
        sys.stdout.buffer.write(compressed)
