"""
Auto-generated style constants from LVGL headers.

Do not edit manually. Regenerate from the GDB script root with:
    python3 scripts/generate_all.py
"""

STYLE_PROP_NAMES = {
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
    176: "TEXT_LEADING_TRIM",
}

PART_NAMES = {
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

STATE_FLAGS = {
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

COLOR_PROPS = {
    44,  # BG_GRAD_COLOR
    52,  # BG_IMAGE_RECOLOR
    57,  # BORDER_COLOR
    65,  # OUTLINE_COLOR
    73,  # BG_COLOR
    88,  # SHADOW_COLOR
    89,  # IMAGE_RECOLOR
    90,  # LINE_COLOR
    91,  # ARC_COLOR
    92,  # TEXT_COLOR
    109,  # TEXT_OUTLINE_STROKE_COLOR
    130,  # RECOLOR
    147,  # DROP_SHADOW_COLOR
}

POINTER_PROPS = {
    40,  # BG_GRAD
    48,  # BG_IMAGE_SRC
    77,  # TEXT_FONT
    96,  # ARC_IMAGE_SRC
    106,  # IMAGE_COLORKEY
    114,  # COLOR_FILTER_DSC
    116,  # ANIM
    118,  # TRANSITION
    121,  # BITMAP_MASK_SRC
    165,  # GRID_COLUMN_DSC_ARRAY
    166,  # GRID_ROW_DSC_ARRAY
}

SRC_PROPS = {
    48,  # BG_IMAGE_SRC
    96,  # ARC_IMAGE_SRC
    121,  # BITMAP_MASK_SRC
}

COORD_PROPS = {
    1,  # WIDTH
    2,  # HEIGHT
    3,  # LENGTH
    4,  # TRANSFORM_WIDTH
    5,  # TRANSFORM_HEIGHT
    8,  # MIN_WIDTH
    9,  # MAX_WIDTH
    10,  # MIN_HEIGHT
    11,  # MAX_HEIGHT
    12,  # TRANSLATE_X
    13,  # TRANSLATE_Y
    14,  # RADIAL_OFFSET
    16,  # X
    17,  # Y
    24,  # PAD_TOP
    25,  # PAD_BOTTOM
    26,  # PAD_LEFT
    27,  # PAD_RIGHT
    28,  # PAD_RADIAL
    29,  # PAD_ROW
    30,  # PAD_COLUMN
    32,  # MARGIN_TOP
    33,  # MARGIN_BOTTOM
    34,  # MARGIN_LEFT
    35,  # MARGIN_RIGHT
    45,  # BG_MAIN_STOP
    46,  # BG_GRAD_STOP
    56,  # BORDER_WIDTH
    64,  # OUTLINE_WIDTH
    67,  # OUTLINE_PAD
    74,  # SHADOW_WIDTH
    75,  # LINE_WIDTH
    76,  # ARC_WIDTH
    97,  # SHADOW_OFFSET_X
    98,  # SHADOW_OFFSET_Y
    99,  # SHADOW_SPREAD
    100,  # LINE_DASH_WIDTH
    102,  # TEXT_LETTER_SPACE
    103,  # TEXT_LINE_SPACE
    104,  # LINE_DASH_GAP
    107,  # TEXT_OUTLINE_STROKE_WIDTH
    120,  # RADIUS
    124,  # TRANSLATE_RADIAL
    136,  # BLUR_RADIUS
    144,  # DROP_SHADOW_RADIUS
    145,  # DROP_SHADOW_OFFSET_X
    146,  # DROP_SHADOW_OFFSET_Y
    152,  # TRANSFORM_SCALE_X
    153,  # TRANSFORM_SCALE_Y
    154,  # TRANSFORM_PIVOT_X
    155,  # TRANSFORM_PIVOT_Y
    156,  # TRANSFORM_ROTATION
    157,  # TRANSFORM_SKEW_X
    158,  # TRANSFORM_SKEW_Y
    170,  # GRID_CELL_COLUMN_POS
    171,  # GRID_CELL_COLUMN_SPAN
    173,  # GRID_CELL_ROW_POS
    174,  # GRID_CELL_ROW_SPAN
}

BOOL_PROPS = {
    51,  # BG_IMAGE_TILED
    59,  # BORDER_POST
    105,  # LINE_ROUNDED
    111,  # ARC_ROUNDED
    128,  # CLIP_CORNER
    137,  # BLUR_BACKDROP
}

ENUM_PROP_VALUES = {
    18: {  # ALIGN
        0: "DEFAULT",
        1: "TOP_LEFT",
        2: "TOP_MID",
        3: "TOP_RIGHT",
        4: "BOTTOM_LEFT",
        5: "BOTTOM_MID",
        6: "BOTTOM_RIGHT",
        7: "LEFT_MID",
        8: "RIGHT_MID",
        9: "CENTER",
        10: "OUT_TOP_LEFT",
        11: "OUT_TOP_MID",
        12: "OUT_TOP_RIGHT",
        13: "OUT_BOTTOM_LEFT",
        14: "OUT_BOTTOM_MID",
        15: "OUT_BOTTOM_RIGHT",
        16: "OUT_LEFT_TOP",
        17: "OUT_LEFT_MID",
        18: "OUT_LEFT_BOTTOM",
        19: "OUT_RIGHT_TOP",
        20: "OUT_RIGHT_MID",
        21: "OUT_RIGHT_BOTTOM",
    },
    41: {  # BG_GRAD_DIR
        0: "NONE",
        1: "VER",
        2: "HOR",
        3: "LINEAR",
        4: "RADIAL",
        5: "CONICAL",
    },
    60: {  # BORDER_SIDE
        0: "NONE",
        1: "BOTTOM",
        2: "TOP",
        4: "LEFT",
        8: "RIGHT",
        15: "FULL",
        16: "INTERNAL",
    },
    101: {  # TEXT_ALIGN
        0: "AUTO",
        1: "LEFT",
        2: "CENTER",
        3: "RIGHT",
    },
    110: {  # TEXT_DECOR
        0: "NONE",
        1: "UNDERLINE",
        2: "STRIKETHROUGH",
    },
    122: {  # BLEND_MODE
        0: "NORMAL",
        1: "ADDITIVE",
        2: "SUBTRACTIVE",
        3: "MULTIPLY",
        4: "DIFFERENCE",
    },
    129: {  # BASE_DIR
        0: "LTR",
        1: "RTL",
        2: "AUTO",
        32: "NEUTRAL",
        33: "WEAK",
    },
    138: {  # BLUR_QUALITY
        0: "AUTO",
        1: "SPEED",
        2: "PRECISION",
    },
    149: {  # DROP_SHADOW_QUALITY
        0: "AUTO",
        1: "SPEED",
        2: "PRECISION",
    },
    160: {  # FLEX_FLOW
        0: "ROW",
        1: "COLUMN",
        4: "ROW_WRAP",
        5: "COLUMN_WRAP",
        8: "ROW_REVERSE",
        9: "COLUMN_REVERSE",
        12: "ROW_WRAP_REVERSE",
        13: "COLUMN_WRAP_REVERSE",
    },
    161: {  # FLEX_MAIN_PLACE
        0: "START",
        1: "END",
        2: "CENTER",
        3: "SPACE_EVENLY",
        4: "SPACE_AROUND",
        5: "SPACE_BETWEEN",
    },
    162: {  # FLEX_CROSS_PLACE
        0: "START",
        1: "END",
        2: "CENTER",
        3: "SPACE_EVENLY",
        4: "SPACE_AROUND",
        5: "SPACE_BETWEEN",
    },
    163: {  # FLEX_TRACK_PLACE
        0: "START",
        1: "END",
        2: "CENTER",
        3: "SPACE_EVENLY",
        4: "SPACE_AROUND",
        5: "SPACE_BETWEEN",
    },
    168: {  # GRID_COLUMN_ALIGN
        0: "START",
        1: "CENTER",
        2: "END",
        3: "STRETCH",
        4: "SPACE_EVENLY",
        5: "SPACE_AROUND",
        6: "SPACE_BETWEEN",
    },
    169: {  # GRID_ROW_ALIGN
        0: "START",
        1: "CENTER",
        2: "END",
        3: "STRETCH",
        4: "SPACE_EVENLY",
        5: "SPACE_AROUND",
        6: "SPACE_BETWEEN",
    },
    172: {  # GRID_CELL_X_ALIGN
        0: "START",
        1: "CENTER",
        2: "END",
        3: "STRETCH",
        4: "SPACE_EVENLY",
        5: "SPACE_AROUND",
        6: "SPACE_BETWEEN",
    },
    175: {  # GRID_CELL_Y_ALIGN
        0: "START",
        1: "CENTER",
        2: "END",
        3: "STRETCH",
        4: "SPACE_EVENLY",
        5: "SPACE_AROUND",
        6: "SPACE_BETWEEN",
    },
    176: {  # TEXT_LEADING_TRIM
        0: "NONE",
        1: "CAPITAL_BASELINE",
        2: "LOWER_BASELINE",
        3: "CAPITAL",
        4: "LOWER",
    },
}

# From LV_COORD_TYPE_SHIFT in lv_area.h
COORD_TYPE_SHIFT = 29
