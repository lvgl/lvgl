import sys
from os import path

here = path.dirname(path.abspath(__file__))

if __name__ == "__main__":
    if here not in sys.path:
        sys.path.insert(0, here)

    for key in tuple(filter(lambda m: m.startswith("lvglgdb"), sys.modules.keys())):
        del sys.modules[key]

    import lvglgdb  # noqa: F401
