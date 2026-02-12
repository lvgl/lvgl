from dataclasses import dataclass
from typing import Iterator

import gdb
from prettytable import PrettyTable
from lvglgdb.value import Value

# Style property ID to name mapping (from lv_style.h enum)
_STYLE_PROP_NAMES = {
    1: "WIDTH",
    2: "HEIGHT",
    3: "LENGTH",
    4: "TRANSFORM_WIDTH",
    5: "TRANSFORM_HEIGHT",
    8: "MIN_WIDTH",
    9: "MAX_WIDTH",
    10: "MIN_HEIGHT",
    11: "MAX_HEIGHT",
    12: "TRANSLATE_X",
    13: "TRANSLATE_Y",
    14: "RADIAL_OFFSET",
    16: "X",
    17: "Y",
    18: "ALIGN",
    24: "PAD_TOP",
    25: "PAD_BOTTOM",
    26: "PAD_LEFT",
    27: "PAD_RIGHT",
    28: "PAD_RADIAL",
    29: "PAD_ROW",
    30: "PAD_COLUMN",
    32: "MARGIN_TOP",
    33: "MARGIN_BOTTOM",
    34: "MARGIN_LEFT",
    35: "MARGIN_RIGHT",
    40: "BG_GRAD",
    41: "BG_GRAD_DIR",
    42: "BG_MAIN_OPA",
    43: "BG_GRAD_OPA",
    44: "BG_GRAD_COLOR",
    45: "BG_MAIN_STOP",
    46: "BG_GRAD_STOP",
    48: "BG_IMAGE_SRC",
    49: "BG_IMAGE_OPA",
    50: "BG_IMAGE_RECOLOR_OPA",
    51: "BG_IMAGE_TILED",
    52: "BG_IMAGE_RECOLOR",
    56: "BORDER_WIDTH",
    57: "BORDER_COLOR",
    58: "BORDER_OPA",
    59: "BORDER_POST",
    60: "BORDER_SIDE",
    64: "OUTLINE_WIDTH",
    65: "OUTLINE_COLOR",
    66: "OUTLINE_OPA",
    67: "OUTLINE_PAD",
    72: "BG_OPA",
    73: "BG_COLOR",
    74: "SHADOW_WIDTH",
    75: "LINE_WIDTH",
    76: "ARC_WIDTH",
    77: "TEXT_FONT",
    78: "IMAGE_RECOLOR_OPA",
    80: "IMAGE_OPA",
    81: "SHADOW_OPA",
    82: "LINE_OPA",
    83: "ARC_OPA",
    84: "TEXT_OPA",
    88: "SHADOW_COLOR",
    89: "IMAGE_RECOLOR",
    90: "LINE_COLOR",
    91: "ARC_COLOR",
    92: "TEXT_COLOR",
    96: "ARC_IMAGE_SRC",
    97: "SHADOW_OFFSET_X",
    98: "SHADOW_OFFSET_Y",
    99: "SHADOW_SPREAD",
    100: "LINE_DASH_WIDTH",
    101: "TEXT_ALIGN",
    102: "TEXT_LETTER_SPACE",
    103: "TEXT_LINE_SPACE",
    104: "LINE_DASH_GAP",
    105: "LINE_ROUNDED",
    106: "IMAGE_COLORKEY",
    107: "TEXT_OUTLINE_STROKE_WIDTH",
    108: "TEXT_OUTLINE_STROKE_OPA",
    109: "TEXT_OUTLINE_STROKE_COLOR",
    110: "TEXT_DECOR",
    111: "ARC_ROUNDED",
    112: "OPA",
    113: "OPA_LAYERED",
    114: "COLOR_FILTER_DSC",
    115: "COLOR_FILTER_OPA",
    116: "ANIM",
    117: "ANIM_DURATION",
    118: "TRANSITION",
    120: "RADIUS",
    121: "BITMAP_MASK_SRC",
    122: "BLEND_MODE",
    123: "ROTARY_SENSITIVITY",
    124: "TRANSLATE_RADIAL",
    128: "CLIP_CORNER",
    129: "BASE_DIR",
    130: "RECOLOR",
    131: "RECOLOR_OPA",
    132: "LAYOUT",
    136: "BLUR_RADIUS",
    137: "BLUR_BACKDROP",
    138: "BLUR_QUALITY",
    144: "DROP_SHADOW_RADIUS",
    145: "DROP_SHADOW_OFFSET_X",
    146: "DROP_SHADOW_OFFSET_Y",
    147: "DROP_SHADOW_COLOR",
    148: "DROP_SHADOW_OPA",
    149: "DROP_SHADOW_QUALITY",
    152: "TRANSFORM_SCALE_X",
    153: "TRANSFORM_SCALE_Y",
    154: "TRANSFORM_PIVOT_X",
    155: "TRANSFORM_PIVOT_Y",
    156: "TRANSFORM_ROTATION",
    157: "TRANSFORM_SKEW_X",
    158: "TRANSFORM_SKEW_Y",
    160: "FLEX_FLOW",
    161: "FLEX_MAIN_PLACE",
    162: "FLEX_CROSS_PLACE",
    163: "FLEX_TRACK_PLACE",
    164: "FLEX_GROW",
    165: "GRID_COLUMN_DSC_ARRAY",
    166: "GRID_ROW_DSC_ARRAY",
    168: "GRID_COLUMN_ALIGN",
    169: "GRID_ROW_ALIGN",
    170: "GRID_CELL_COLUMN_POS",
    171: "GRID_CELL_COLUMN_SPAN",
    172: "GRID_CELL_X_ALIGN",
    173: "GRID_CELL_ROW_POS",
    174: "GRID_CELL_ROW_SPAN",
    175: "GRID_CELL_Y_ALIGN",
}

_PART_NAMES = {
    0x00: "MAIN",
    0x01: "SCROLLBAR",
    0x02: "INDICATOR",
    0x03: "KNOB",
    0x04: "SELECTED",
    0x05: "ITEMS",
    0x06: "CURSOR",
    0x08: "CUSTOM_FIRST",
    0x0F: "ANY",
}

_STATE_FLAGS = {
    0x0001: "ALT",
    0x0004: "CHECKED",
    0x0008: "FOCUSED",
    0x0010: "FOCUS_KEY",
    0x0020: "EDITED",
    0x0040: "HOVERED",
    0x0080: "PRESSED",
    0x0100: "SCROLLED",
    0x0200: "DISABLED",
    0x1000: "USER_1",
    0x2000: "USER_2",
    0x4000: "USER_3",
    0x8000: "USER_4",
}

_COLOR_PROPS = {
    44,
    52,
    57,
    65,
    73,
    88,
    89,
    90,
    91,
    92,
    109,
    130,
    147,
}

_POINTER_PROPS = {
    40,
    48,
    77,
    96,
    114,
    116,
    118,
    121,
    165,
    166,
}


def style_prop_name(prop_id: int) -> str:
    """Resolve style property ID to human-readable name."""
    return _STYLE_PROP_NAMES.get(prop_id, f"UNKNOWN({prop_id})")


def decode_selector(selector: int) -> str:
    """Decode selector into part + state string."""
    part_val = (selector >> 16) & 0xFF
    state_val = selector & 0xFFFF

    part_str = _PART_NAMES.get(part_val, f"PART({part_val:#x})")

    if state_val == 0:
        state_str = "DEFAULT"
    elif state_val == 0xFFFF:
        state_str = "ANY"
    else:
        flags = [name for bit, name in _STATE_FLAGS.items() if state_val & bit]
        state_str = "|".join(flags) if flags else f"STATE({state_val:#x})"

    return f"{part_str}|{state_str}"


def format_style_value(prop_id: int, value: Value) -> str:
    """Format a style value based on property type."""
    try:
        if prop_id in _COLOR_PROPS:
            color = value.color
            r = int(color.red) & 0xFF
            g = int(color.green) & 0xFF
            b = int(color.blue) & 0xFF
            block = f"\033[48;2;{r};{g};{b}m  \033[0m"
            return f"#{r:02x}{g:02x}{b:02x} {block}"
        elif prop_id in _POINTER_PROPS:
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
