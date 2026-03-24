import functools
from typing import Any, Callable, Dict, Iterator, Optional


class Snapshot:
    """Self-describing data snapshot from a wrapper instance.

    Holds a pure Python dict (JSON-serializable), an optional reference
    to the original wrapper (_source), and an optional display spec that
    describes how to format the data for terminal output.

    A snapshot may be marked *corrupted* to indicate that the underlying
    data could not be fully read (e.g. inaccessible target memory).
    """

    __slots__ = ("_data", "_source", "_display_spec")

    def __init__(self, data: Dict[str, Any], source: Any = None,
                 display_spec: Optional[Dict] = None):
        self._data = data
        self._source = source
        self._display_spec = display_spec

    @classmethod
    def fallback(cls, addr: Callable = lambda x: int(x), **extra_fns):
        """Decorator: on CorruptedError, return a corrupted snapshot dict.

        Each extra kwarg may be a callable (called with the first arg)
        or a static value.
        """
        def decorator(fn):
            @functools.wraps(fn)
            def wrapper(item, *args, **kwargs):
                from ..value import CorruptedError
                try:
                    return fn(item, *args, **kwargs)
                except CorruptedError as e:
                    try:
                        resolved_addr = hex(addr(item))
                    except Exception:
                        resolved_addr = "0x?"
                    extra = {}
                    for k, v in extra_fns.items():
                        try:
                            extra[k] = v(item) if callable(v) else v
                        except Exception:
                            extra[k] = None
                    d = {"addr": resolved_addr,
                         "class_name": "(corrupted)",
                         "error": str(e)}
                    if extra:
                        d.update(extra)
                    return d
            return wrapper
        return decorator

    @staticmethod
    def safe_fields(source, field_specs: list) -> dict:
        """Collect fields with per-field CorruptedError handling.

        field_specs: list of (key, callable) or (key, callable, default)
        """
        from ..value import CorruptedError

        d = {}
        for spec in field_specs:
            key, fn = spec[0], spec[1]
            default = spec[2] if len(spec) > 2 else None
            try:
                d[key] = fn(source)
            except CorruptedError:
                d[key] = default
        return d

    # --- dict-like read access ---

    def __getitem__(self, key: str) -> Any:
        return self._data[key]

    def __contains__(self, key: str) -> bool:
        return key in self._data

    def __len__(self) -> int:
        return len(self._data)

    def __iter__(self) -> Iterator[str]:
        return iter(self._data)

    def keys(self):
        return self._data.keys()

    def values(self):
        return self._data.values()

    def items(self):
        return self._data.items()

    def get(self, key: str, default: Any = None) -> Any:
        return self._data.get(key, default)

    # --- serialization ---

    def as_dict(self) -> Dict[str, Any]:
        return dict(self._data)

    def __repr__(self) -> str:
        addr = self._data.get("addr", "?")
        src_type = type(self._source).__name__ if self._source else "None"
        return f"Snapshot(addr={addr}, source={src_type})"
