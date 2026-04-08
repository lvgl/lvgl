import functools
from typing import Callable, Optional

import gdb


def fmt_cb(cb) -> str:
    """Format callback pointer as resolved symbol string or '-' for NULL.
    Strips null bytes that may appear in some GDB output."""
    if not cb.is_ok:
        return str(cb)
    addr = int(cb)
    if not addr:
        return "-"
    return cb.format_string(symbols=True, address=True).replace("\x00", "")


def ptr_or_none(val) -> Optional[str]:
    """Convert pointer to hex string or None if NULL."""
    if not val.is_ok:
        return None
    addr = int(val)
    return hex(addr) if addr else None


def safe_collect(
    items_or_label,
    transform: Callable = None,
):
    """Unified safe collection with two usage modes.

    Iteration mode — collect items, skipping failures:
        safe_collect(items, transform)

    Decorator mode — wrap an entire collector function:
        @safe_collect("subsystem name")
        def collect_xxx(): ...

    In decorator mode, exceptions cause a gdb warning and return [].
    In iteration mode, per-item exceptions are skipped silently.
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
            except Exception:
                continue
    except Exception:
        pass
    return result
