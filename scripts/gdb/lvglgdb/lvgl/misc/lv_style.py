from dataclasses import dataclass
from typing import Iterator

import gdb
from lvglgdb.value import CorruptedError, Value, ValueInput
from .lv_style_consts import (
    STYLE_PROP_NAMES,
    PART_NAMES,
    STATE_FLAGS,
    COLOR_PROPS,
    POINTER_PROPS,
    SRC_PROPS,
    COORD_PROPS,
    BOOL_PROPS,
    ENUM_PROP_VALUES,
    COORD_TYPE_SHIFT,
)

_COORD_TYPE_MASK = 3 << COORD_TYPE_SHIFT
_COORD_TYPE_SPEC = 1 << COORD_TYPE_SHIFT
_COORD_MAX = (1 << COORD_TYPE_SHIFT) - 1
_PCT_STORED_MAX = _COORD_MAX - 1
_PCT_POS_MAX = _PCT_STORED_MAX // 2

# lv_image_src_get_type(): the first byte of an image source says what it is.
_IMAGE_HEADER_MAGIC = 0x19
_IMAGE_HEADER_LEGACY = 0x00

_symbol_cache: dict[int, "str | None"] = {}


def style_prop_name(prop_id: int) -> str:
    """Resolve style property ID to human-readable name."""
    return STYLE_PROP_NAMES.get(prop_id, f"UNKNOWN({prop_id})")


def decode_selector(selector: int) -> str:
    """Decode selector into part + state string."""
    part_val = (selector >> 16) & 0xFF
    state_val = selector & 0xFFFF

    part_str = PART_NAMES.get(part_val, f"PART({part_val:#x})")

    if state_val == 0:
        state_str = "DEFAULT"
    elif state_val == 0xFFFF:
        state_str = "ANY"
    else:
        flags = [name for bit, name in STATE_FLAGS.items() if state_val & bit]
        state_str = "|".join(flags) if flags else f"STATE({state_val:#x})"

    return f"{part_str}|{state_str}"


def decode_coord(raw: int) -> str:
    """Render an int32 coordinate the way the source wrote it.

    LV_PCT() and LV_SIZE_CONTENT survive in the stored value as a type tag in
    the top bits, so a percentage never has to be inferred from geometry.
    """
    bits = raw & 0xFFFFFFFF
    # Only LV_COORD_TYPE_SPEC carries an encoding. LV_COORD_TYPE_PX_NEG is not a
    # second one: there is no LV_COORD_SET_PX_NEG, and in two's complement every
    # negative int32 already has those top bits, so a "PX_NEG" value is just a
    # negative number and prints as itself.
    if bits & _COORD_TYPE_MASK != _COORD_TYPE_SPEC:
        return str(raw)
    plain = bits & ~_COORD_TYPE_MASK
    if plain == _COORD_MAX:
        return "content"
    if plain > _PCT_STORED_MAX:
        return str(raw)
    pct = plain if plain <= _PCT_POS_MAX else _PCT_POS_MAX - plain
    return f"{pct}%"


def decode_enum(prop_id: int, raw: int) -> str:
    """Name an enum value, decomposing bitmasks such as BORDER_SIDE.

    Only single-bit members are combined, and only when they account for every
    set bit. Otherwise a sequential enum like FLEX_FLOW would render an
    out-of-range value as a meaningless list of its low members.
    """
    names = ENUM_PROP_VALUES[prop_id]
    if raw in names:
        return names[raw]
    parts, covered = [], 0
    for bit in sorted(b for b in names if b and not b & (b - 1)):
        if raw & bit:
            parts.append(names[bit])
            covered |= bit
    return "|".join(parts) if parts and covered == raw else str(raw)


def _read_symbol(addr: int) -> "str | None":
    """Resolve an address to its C symbol name, or None."""
    if addr not in _symbol_cache:
        try:
            out = gdb.execute(f"info symbol {addr:#x}", to_string=True).strip()
        except gdb.error:
            out = ""
        name = out.split(" in section ")[0] if " in section " in out else None
        _symbol_cache[addr] = name
    return _symbol_cache[addr]


def _forget_symbols(_event=None):
    """Addresses move when the program is re-run, so the cache must not outlive it."""
    _symbol_cache.clear()


try:
    gdb.events.exited.connect(_forget_symbols)
    gdb.events.new_objfile.connect(_forget_symbols)
except AttributeError:
    pass  # older GDB, or imported outside a debug session


def _read_image_src(addr: int) -> "str | None":
    """Read an image source pointer the way lv_image_src_get_type() does."""
    try:
        first = int(gdb.selected_inferior().read_memory(addr, 1)[0][0])
        if first == _IMAGE_HEADER_MAGIC or first == _IMAGE_HEADER_LEGACY:
            return None  # an lv_image_dsc_t, so the symbol name is the useful part
        return gdb.Value(addr).cast(gdb.lookup_type("char").pointer()).string()
    except (gdb.error, gdb.MemoryError, UnicodeDecodeError):
        return None


def _format_pointer(prop_id: int, addr: int) -> str:
    """Name a pointer property: its symbol, the string it points at, or its address."""
    if not addr:
        return "NULL"
    symbol = _read_symbol(addr)
    if symbol:
        return symbol
    if prop_id in SRC_PROPS:
        text = _read_image_src(addr)
        if text:
            return text
    return f"{addr:#x}"


def _style_value_data(prop_id: int, value: Value) -> dict:
    """Extract style value as pure data dict (no ANSI codes).

    Returns dict with 'value_str', optional 'color_rgb' and, for pointers,
    'ptr' so the raw address is never lost behind a resolved name.
    """
    try:
        if prop_id in COLOR_PROPS:
            color = value.color
            r = int(color.red) & 0xFF
            g = int(color.green) & 0xFF
            b = int(color.blue) & 0xFF
            return {
                "value_str": f"#{r:02x}{g:02x}{b:02x}",
                "color_rgb": {"r": r, "g": g, "b": b},
            }
        elif prop_id in POINTER_PROPS:
            ptr = int(value.ptr)
            data = {"value_str": _format_pointer(prop_id, ptr)}
            if ptr:
                data["ptr"] = f"{ptr:#x}"
            return data
        elif prop_id in ENUM_PROP_VALUES:
            return {"value_str": decode_enum(prop_id, int(value.num))}
        elif prop_id in BOOL_PROPS:
            return {"value_str": "true" if int(value.num) else "false"}
        elif prop_id in COORD_PROPS:
            return {"value_str": decode_coord(int(value.num))}
        else:
            return {"value_str": str(int(value.num))}
    except CorruptedError:
        return {"value_str": str(value)}


def format_style_value(prop_id: int, value: Value) -> str:
    """Format a style value based on property type (with ANSI color block)."""
    data = _style_value_data(prop_id, value)
    rgb = data.get("color_rgb")
    if rgb:
        block = f"\033[48;2;{rgb['r']};{rgb['g']};{rgb['b']}m  \033[0m"
        return f"{data['value_str']} {block}"
    return data["value_str"]


@dataclass
class StyleEntry:
    """A single resolved style property."""

    prop_id: int
    value: Value

    @property
    def prop_name(self) -> str:
        return style_prop_name(self.prop_id)

    @property
    def value_str(self) -> str:
        return format_style_value(self.prop_id, self.value)


class LVStyle(Value):
    """LVGL style wrapper for lv_style_t."""

    def __init__(self, style: ValueInput):
        super().__init__(Value.normalize(style, "lv_style_t"))

    def __iter__(self) -> Iterator[StyleEntry]:
        prop_cnt = int(self.prop_cnt)
        if prop_cnt == 0xFF:
            # Constant style: lv_style_const_prop_t array terminated by prop==0
            const_props = self.values_and_props.cast("lv_style_const_prop_t", ptr=True)
            j = 0
            while True:
                prop_id = int(const_props[j].prop)
                if prop_id == 0 or prop_id == 0xFF:
                    break
                yield StyleEntry(prop_id, const_props[j].value)
                j += 1
        elif prop_cnt > 0:
            # Normal style: values[prop_cnt] then props[prop_cnt]
            # C code: (lv_style_prop_t*)vp + prop_cnt * sizeof(lv_style_value_t)
            # The pointer arithmetic uses lv_style_prop_t element size as stride.
            base = self.values_and_props
            value_t = gdb.lookup_type("lv_style_value_t")
            prop_t = gdb.lookup_type("lv_style_prop_t")
            values_ptr = base.cast(value_t, ptr=True)
            props_offset = prop_cnt * value_t.sizeof * prop_t.sizeof
            props_ptr = Value(int(base) + props_offset).cast(prop_t, ptr=True)

            for j in range(prop_cnt):
                prop_id = int(props_ptr[j])
                if prop_id == 0:
                    continue
                yield StyleEntry(prop_id, values_ptr[j])

    def snapshots(self):
        from lvglgdb.lvgl.snapshot import Snapshot

        result = []
        for entry in self.__iter__():
            vdata = _style_value_data(entry.prop_id, entry.value)
            d = {
                "prop_id": entry.prop_id,
                "prop_name": entry.prop_name,
                **vdata,
            }
            result.append(Snapshot(d, source=entry))
        return result


