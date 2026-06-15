import os

import gdb


def resolve_source_name(addr, prefix="lv_"):
    """Resolve a code address to a short name derived from its source file.

    Tries block_for_pc first, then find_pc_line as fallback.
    Strips common prefixes (e.g. "lv_fs_stdio.c" -> "stdio").

    Args:
        addr: Integer address of a function/callback.
        prefix: Prefix to strip from filename (default "lv_").

    Returns:
        Short name string, or None if resolution fails.
    """
    if not addr:
        return None
    fname = None
    try:
        block = gdb.block_for_pc(addr)
        if block and block.function and block.function.symtab:
            fname = block.function.symtab.filename
    except (gdb.error, RuntimeError):
        pass
    if not fname:
        try:
            sal = gdb.find_pc_line(addr)
            if sal and sal.symtab:
                fname = sal.symtab.filename
        except gdb.error:
            pass
    if not fname:
        return None
    base = os.path.basename(fname).replace(".c", "")
    if prefix and base.startswith(prefix):
        base = base[len(prefix) :]
    return base


def build_global_field_map(field_type_name):
    """Build address -> field name map for fields of a given type in lv_global_t.

    Scans lv_global_t struct fields, finds those matching the given type,
    and returns a dict mapping their runtime addresses to field names.

    Args:
        field_type_name: C type name string (e.g. "lv_fs_drv_t").

    Returns:
        Dict mapping int address -> str field name.
    """
    try:
        lv_global_val = gdb.parse_and_eval("lv_global")
        lv_global_addr = int(lv_global_val.address)
        lv_global_type = gdb.lookup_type("lv_global_t")
        target_type = gdb.lookup_type(field_type_name)
        result = {}
        for field in lv_global_type.fields():
            if field.type.strip_typedefs() == target_type:
                addr = lv_global_addr + field.bitpos // 8
                result[addr] = field.name
        return result
    except gdb.error:
        return {}


# LVGL coordinate type constants (from lv_area.h)
_COORD_TYPE_SHIFT = 29
_COORD_TYPE_MASK = 3 << _COORD_TYPE_SHIFT
_COORD_TYPE_PX = 0 << _COORD_TYPE_SHIFT
_COORD_TYPE_SPEC = 1 << _COORD_TYPE_SHIFT
_COORD_TYPE_PX_NEG = 3 << _COORD_TYPE_SHIFT
_COORD_MAX = (1 << _COORD_TYPE_SHIFT) - 1
_SIZE_CONTENT = _COORD_MAX | _COORD_TYPE_SPEC
_PCT_POS_MAX = (_COORD_MAX - 1) // 2


def format_coord(val):
    """Format an lv_coord_t value into a human-readable string.

    Decodes special LVGL coordinate encodings:
      - LV_SIZE_CONTENT       -> "CONTENT"
      - LV_PCT(x)             -> "x%"
      - LV_COORD_TYPE_PX_NEG  -> negative pixel
      - plain pixel            -> "123"
    """
    val = int(val)
    if val == _SIZE_CONTENT:
        return "CONTENT"
    coord_type = val & _COORD_TYPE_MASK
    if coord_type == _COORD_TYPE_SPEC:
        plain = val & ~_COORD_TYPE_MASK
        if plain <= _PCT_POS_MAX:
            return f"{plain}%"
        return f"{_PCT_POS_MAX - plain}%"
    if coord_type == _COORD_TYPE_PX_NEG:
        plain = val & ~_COORD_TYPE_MASK
        return str(-plain) if plain else "0"
    return str(val)
