import sys

if __name__ == '__main__':
    destname = sys.stdin.buffer.read()
    if len(sys.argv) == 3:
        name = destname.decode()
        args = sys.argv[1].split('=')
        # Append extension
        if args[0] == '--ext':
            sys.stdout.buffer.write(str.encode(name+"."+sys.argv[2]))
        # Or prepend some value
        elif args[0] == '--prepend':
            sys.stdout.buffer.write(str.encode(sys.argv[2]+name))
