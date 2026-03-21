import functools
from typing import Callable, Iterable, Optional

import gdb

from lvglgdb.value import Value


def fmt_cb(cb: Value) -> str:
    """Format callback pointer as resolved symbol string or '-' for NULL.
    Strips null bytes that may appear in some GDB output."""
    addr = int(cb)
    if not addr:
        return "-"
    return cb.format_string(symbols=True, address=True).replace("\x00", "")


def ptr_or_none(val: Value) -> Optional[str]:
    """Convert pointer to hex string or None if NULL."""
    addr = int(val)
    return hex(addr) if addr else None


def safe_collect(
    items_or_label,
    transform: Callable = None,
    on_mem_error: Callable = None,
):
    """Unified safe collection with two usage modes.

    Iteration mode — collect items, skipping failures:
        safe_collect(items, transform, on_mem_error=None)

    Decorator mode — wrap an entire collector function:
        @safe_collect("subsystem name")
        def collect_xxx(): ...

    In decorator mode, exceptions cause a gdb warning and return [].
    In iteration mode, gdb.MemoryError/gdb.error calls on_mem_error
    (or skips), other exceptions skip silently.
    """
    if isinstance(items_or_label, str):
        label = items_or_label

        def decorator(fn):
            @functools.wraps(fn)
            def wrapper(*args, **kwargs):
                try:
                    return fn(*args, **kwargs)
                except Exception as e:
                    import traceback
                    gdb.write(f"Warning: failed to collect {label}: {e}\n")
                    traceback.print_exc()
                    return []
            return wrapper
        return decorator

    result = []
    try:
        for item in items_or_label:
            try:
                result.append(transform(item))
            except (gdb.MemoryError, gdb.error) as e:
                if on_mem_error is not None:
                    result.append(on_mem_error(item, e))
            except Exception:
                continue
    except Exception:
        pass
    return result
