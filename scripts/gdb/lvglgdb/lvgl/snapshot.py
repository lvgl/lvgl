from typing import Any, Dict, Iterator, Optional


class Snapshot:
    """Self-describing data snapshot from a wrapper instance.

    Holds a pure Python dict (JSON-serializable), an optional reference
    to the original wrapper (_source), and an optional display spec that
    describes how to format the data for terminal output.
    """

    __slots__ = ("_data", "_source", "_display_spec")

    def __init__(self, data: Dict[str, Any], source: Any = None,
                 display_spec: Optional[Dict] = None):
        self._data = data
        self._source = source
        self._display_spec = display_spec

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
