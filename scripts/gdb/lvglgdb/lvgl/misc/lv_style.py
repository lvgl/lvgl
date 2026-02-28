from dataclasses import dataclass
from typing import Iterator

import gdb
from prettytable import PrettyTable
from lvglgdb.value import Value
from .lv_style_consts import (
    STYLE_PROP_NAMES,
    PART_NAMES,
    STATE_FLAGS,
    COLOR_PROPS,
    POINTER_PROPS,
)


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


def format_style_value(prop_id: int, value: Value) -> str:
    """Format a style value based on property type."""
    try:
        if prop_id in COLOR_PROPS:
            color = value.color
            r = int(color.red) & 0xFF
            g = int(color.green) & 0xFF
            b = int(color.blue) & 0xFF
            block = f"\033[48;2;{r};{g};{b}m  \033[0m"
            return f"#{r:02x}{g:02x}{b:02x} {block}"
        elif prop_id in POINTER_PROPS:
            ptr = int(value.ptr)
            return f"{ptr:#x}" if ptr else "NULL"
        else:
            return str(int(value.num))
    except gdb.error:
        return str(value)


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

    def __init__(self, style: Value):
        # Ensure we always hold a lv_style_t* pointer, like LVObject does
        typ = style.type.strip_typedefs()
        if typ.code != gdb.TYPE_CODE_PTR:
            style = Value(style.address)
        super().__init__(style.cast("lv_style_t", ptr=True))

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
            # Normal style: values[prop_cnt] then props[prop_cnt] (uint8_t)
            base = self.values_and_props
            value_t = gdb.lookup_type("lv_style_value_t")
            values_ptr = base.cast(value_t, ptr=True)
            props_offset = prop_cnt * value_t.sizeof
            props_ptr = Value(int(base) + props_offset).cast("uint8_t", ptr=True)

            for j in range(prop_cnt):
                prop_id = int(props_ptr[j])
                if prop_id == 0:
                    continue
                yield StyleEntry(prop_id, values_ptr[j])

    def print_entries(self):
        """Print style properties as a table."""
        entries = list(self.__iter__())
        if not entries:
            print("Empty style.")
            return

        table = PrettyTable()
        table.field_names = ["prop", "value"]
        table.align = "l"
        for e in entries:
            table.add_row([e.prop_name, e.value_str])
        print(table)


def dump_style_info(entry: StyleEntry):
    """Print a single style property."""
    print(f"{entry.prop_name}({entry.prop_id}) = {entry.value_str}")
