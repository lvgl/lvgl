import sys
import gzip

if __name__ == '__main__':
    while True:
        chunk = sys.stdin.buffer.read()
        if not chunk:
            break
        compressed = gzip.compress(chunk)
        sys.stdout.buffer.write(compressed)
