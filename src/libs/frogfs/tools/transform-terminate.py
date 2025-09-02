import sys

CHUNK_SIZE = 4096

if __name__ == '__main__':
    while True:
        chunk = sys.stdin.buffer.read()
        if not chunk:
            break
        sys.stdout.buffer.write(chunk)
    sys.stdout.buffer.write(b'\0')
