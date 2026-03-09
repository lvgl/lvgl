from typing import Optional

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
