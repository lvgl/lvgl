#!/usr/bin/env python3
"""
Generate widget wrapper classes from LVGL widget _private.h headers.

Each widget gets its own Python file under lvglgdb/lvgl/widgets/,
with a class inheriting from LVObject (or its parent widget wrapper).

Output structure:
    lvglgdb/lvgl/widgets/
        __init__.py          ← re-exports + WIDGET_REGISTRY + wrap_widget()
        _helpers.py          ← shared field-reading helpers
        lv_label.py          ← class LVLabel(LVObject)
        lv_slider.py         ← class LVSlider(LVBar)
        ...

Usage (from the GDB script root):
    python3 scripts/generators/gen_widget_wrappers.py
"""

import re
import sys
from pathlib import Path
from dataclasses import dataclass, field as dc_field

sys.path.insert(0, str(Path(__file__).resolve().parent.parent))

LVGL_SRC = Path(__file__).parent.parent.parent.parent.parent / "src"
WIDGETS_DIR = LVGL_SRC / "widgets"
OUTPUT_DIR = Path(__file__).parent.parent.parent / "lvglgdb" / "lvgl" / "widgets"

SIMPLE_INT_TYPES = {
    "int8_t", "int16_t", "int32_t", "int64_t",
    "uint8_t", "uint16_t", "uint32_t", "uint64_t",
    "int", "bool", "size_t",
    "lv_value_precise_t",
}


def _scan_enum_types() -> set[str]:
    """Scan LVGL headers to find all typedef enum and int-like alias type names."""
    result = set()
    for h in LVGL_SRC.rglob("*.h"):
        text = h.read_text(errors="ignore")
        # typedef enum { ... } lv_xxx_t;
        for m in re.finditer(
            r"typedef\s+enum\s*\{[^}]*\}\s*(lv_\w+_t)", text, re.DOTALL
        ):
            result.add(m.group(1))
        # typedef <int-like> lv_xxx_t;
        for m in re.finditer(
            r"typedef\s+((?:unsigned\s+)?\w+)\s+(lv_\w+_t)\s*;", text
        ):
            base = m.group(1).strip()
            if base in (
                "unsigned int", "unsigned char", "unsigned short",
                "unsigned long", "int", "char", "short", "long",
                "uint8_t", "uint16_t", "uint32_t", "uint64_t",
                "int8_t", "int16_t", "int32_t", "int64_t", "size_t",
            ):
                result.add(m.group(2))
    return result


# Pre-scan: types safe to cast to int (enums + int-like typedefs)
_INT_SAFE_TYPES = _scan_enum_types()


@dataclass
class StructField:
    name: str
    c_type: str
    is_bitfield: bool = False
    bitfield_width: int = 0
    is_pointer: bool = False
    is_obj_pointer: bool = False
    is_string: bool = False
    is_array: bool = False
    comment: str = ""


@dataclass
class WidgetDef:
    struct_name: str
    c_type: str
    parent_type: str
    fields: list[StructField] = dc_field(default_factory=list)
    widget_dir: str = ""

    @property
    def class_name(self) -> str:
        inner = self.c_type.removeprefix("lv_").removesuffix("_t")
        return "LV" + "".join(w.capitalize() for w in inner.split("_"))

    @property
    def parent_class_name(self) -> str:
        if self.parent_type == "lv_obj_t":
            return "LVObject"
        inner = self.parent_type.removeprefix("lv_").removesuffix("_t")
        return "LV" + "".join(w.capitalize() for w in inner.split("_"))

    @property
    def module_name(self) -> str:
        """Python module filename without .py, e.g. 'lv_label'."""
        return self.c_type.removesuffix("_t")

    @property
    def c_class_name(self) -> str:
        """C class name for registry, e.g. 'lv_label'."""
        return self.c_type.removesuffix("_t")


def parse_struct_fields(body: str) -> list[StructField]:
    fields = []
    depth = 0
    clean_lines = []
    for line in body.splitlines():
        stripped = line.strip()
        if re.match(r"(struct|union)\s*\{", stripped):
            depth += 1
            continue
        if stripped.startswith("}") and depth > 0:
            depth -= 1
            continue
        if depth > 0:
            continue
        clean_lines.append(stripped)

    for line in clean_lines:
        line = line.rstrip(";").strip()
        if not line or line.startswith("/*") or line.startswith("//") or line.startswith("*") or line.startswith("#"):
            continue

        comment = ""
        cm = re.search(r"/\*\*?<?\s*(.*?)\s*\*/", line)
        if cm:
            comment = cm.group(1)
            line = line[:cm.start()].strip().rstrip(";").strip()
        if not line:
            continue

        if re.search(r"\[.*\]", line):
            am = re.match(r"(?:const\s+)?(\w[\w\s\*]*?)\s+(\w+)\s*\[", line)
            if am:
                fields.append(StructField(
                    name=am.group(2), c_type=am.group(1).strip(),
                    is_array=True, comment=comment,
                ))
            continue

        bm = re.match(r"(?:const\s+)?(\w[\w\s\*]*?)\s+(\w+)\s*:\s*(\d+)", line)
        if bm:
            fields.append(StructField(
                name=bm.group(2), c_type=bm.group(1).strip(),
                is_bitfield=True, bitfield_width=int(bm.group(3)),
                comment=comment,
            ))
            continue

        fm = re.match(r"((?:const\s+)?[\w][\w\s]*?(?:\s*\*\s*(?:const\s*)?)*\*?)\s+(\*?\w+)", line)
        if fm:
            c_type = fm.group(1).strip()
            name = fm.group(2).strip().lstrip("*")
            is_ptr = "*" in c_type or fm.group(2).startswith("*")
            fields.append(StructField(
                name=name, c_type=c_type,
                is_pointer=is_ptr,
                is_obj_pointer=is_ptr and "lv_obj_t" in c_type,
                is_string=is_ptr and "char" in c_type and c_type.count("*") <= 1,
                comment=comment,
            ))

    return fields


def _find_structs(text: str):
    """Find top-level struct _lv_*_t definitions with brace-balanced matching."""
    for m in re.finditer(r"struct\s+_lv_(\w+)_t\s*\{", text):
        name = m.group(1)
        start = m.end()
        depth = 1
        i = start
        while i < len(text) and depth > 0:
            if text[i] == "{":
                depth += 1
            elif text[i] == "}":
                depth -= 1
            i += 1
        if depth == 0:
            yield name, text[start:i - 1]


def parse_widgets() -> dict[str, WidgetDef]:
    # Pass 1: collect all structs with parent type
    candidates = []
    for private_h in sorted(WIDGETS_DIR.glob("*/lv_*_private.h")):
        text = private_h.read_text()
        widget_dir = private_h.parent.name
        for raw_name, body in _find_structs(text):
            struct_name = f"lv_{raw_name}_t"
            first_line = ""
            for line in body.splitlines():
                line = line.strip()
                if line and not line.startswith(("/*", "//", "*", "#")):
                    first_line = line.rstrip(";").strip()
                    break
            parent_match = re.match(r"(lv_\w+_t)\s+\w+", first_line)
            if parent_match:
                candidates.append((struct_name, parent_match.group(1), body, widget_dir))

    # Pass 2: resolve inheritance from lv_obj_t (order-independent)
    widget_types = {"lv_obj_t"}
    changed = True
    while changed:
        changed = False
        for name, parent, _, _ in candidates:
            if name not in widget_types and parent in widget_types:
                widget_types.add(name)
                changed = True

    # Pass 3: build WidgetDef for discovered widgets
    widgets = {}
    for struct_name, parent_type, body, widget_dir in candidates:
        if struct_name in widget_types and struct_name != "lv_obj_t":
            all_fields = parse_struct_fields(body)
            widgets[struct_name] = WidgetDef(
                struct_name=struct_name, c_type=struct_name,
                parent_type=parent_type,
                fields=all_fields[1:] if all_fields else [],
                widget_dir=widget_dir,
            )
    return widgets


def _field_expr(f: StructField) -> str | None:
    """Return snapshot expression for a field, or None to skip."""
    if f.is_array:
        return None
    if f.is_obj_pointer:
        return f'ptr_or_none(self._wv.safe_field("{f.name}"))'
    if f.is_string:
        return f'safe_string(self._wv, "{f.name}")'
    if f.is_pointer:
        return f'ptr_or_none(self._wv.safe_field("{f.name}"))'
    if f.c_type == "lv_color_t":
        return f'safe_color(self._wv, "{f.name}")'
    if f.c_type == "lv_area_t":
        return f'safe_area(self._wv, "{f.name}")'
    if f.c_type == "lv_point_t":
        return f'safe_point(self._wv, "{f.name}")'
    # Known wrapper types — use snapshot() for rich output
    _WRAPPER_TYPES = {
        "lv_draw_buf_t": ("lvglgdb.lvgl.draw.lv_draw_buf", "LVDrawBuf"),
        "lv_ll_t": ("lvglgdb.lvgl.misc.lv_ll", "LVList"),
        "lv_anim_t": ("lvglgdb.lvgl.misc.lv_anim", "LVAnim"),
        "lv_array_t": ("lvglgdb.lvgl.misc.lv_array", "LVArray"),
    }
    if f.c_type in _WRAPPER_TYPES:
        mod, cls = _WRAPPER_TYPES[f.c_type]
        return f'safe_wrapper(self._wv, "{f.name}", "{mod}", "{cls}")'
    if f.is_bitfield or f.c_type in SIMPLE_INT_TYPES or f.c_type.startswith(("uint", "int")):
        return f'int(self._wv.safe_field("{f.name}", 0))'
    # TODO: implement generic struct expansion (Value.to_dict) for
    # non-enum lv_*_t types like lv_calendar_date_t.
    if f.c_type in _INT_SAFE_TYPES:
        return f'int(self._wv.safe_field("{f.name}", 0))'
    return None


def _topo_sort(widgets: dict[str, WidgetDef]) -> list[WidgetDef]:
    result, visited = [], set()
    def visit(w):
        if w.c_type in visited:
            return
        visited.add(w.c_type)
        if w.parent_type in widgets:
            visit(widgets[w.parent_type])
        result.append(w)
    for w in widgets.values():
        visit(w)
    return result


def gen_helpers() -> str:
    return '''\
"""Shared field-reading helpers for widget wrappers.

All helpers return a safe value (or None) without raising exceptions.
Protection is provided by Value.safe_field() and Value.string(fallback=).
"""

from lvglgdb.lvgl.data_utils import ptr_or_none  # noqa: F401


def safe_string(obj, field_name):
    """Read a char* field as string, corrupted marker, or None (NULL/missing)."""
    from lvglgdb.value import CorruptedValue
    val = obj.safe_field(field_name)
    if val is None:
        return None
    if not getattr(val, 'is_ok', True):
        return str(val)
    addr = int(val)
    if not addr:
        return None
    return val.string(fallback=str(CorruptedValue(addr, MemoryError("unreadable"))))


def safe_color(obj, field_name):
    """Read lv_color_t {red, green, blue} as hex string."""
    val = obj.safe_field(field_name)
    if val is None or not getattr(val, 'is_ok', True):
        return None
    r = int(val.safe_field("red", 0))
    g = int(val.safe_field("green", 0))
    b = int(val.safe_field("blue", 0))
    return f"#{r:02x}{g:02x}{b:02x}"


def safe_area(obj, field_name):
    """Read lv_area_t as dict."""
    val = obj.safe_field(field_name)
    if val is None or not getattr(val, 'is_ok', True):
        return None
    return {
        "x1": int(val.safe_field("x1", 0)),
        "y1": int(val.safe_field("y1", 0)),
        "x2": int(val.safe_field("x2", 0)),
        "y2": int(val.safe_field("y2", 0)),
    }


def safe_point(obj, field_name):
    """Read lv_point_t as dict."""
    val = obj.safe_field(field_name)
    if val is None or not getattr(val, 'is_ok', True):
        return None
    return {
        "x": int(val.safe_field("x", 0)),
        "y": int(val.safe_field("y", 0)),
    }


def safe_wrapper(obj, field_name, module_path, class_name):
    """Read a struct field using its known Value wrapper, return snapshot dict."""
    val = obj.safe_field(field_name)
    if val is None or not getattr(val, 'is_ok', True):
        return None
    import importlib
    mod = importlib.import_module(module_path)
    cls = getattr(mod, class_name)
    wrapper = cls(val)
    return wrapper.snapshot().as_dict()
'''


def gen_widget_file(wdef: WidgetDef, widgets: dict[str, WidgetDef]) -> str:
    """Generate a single widget module file."""
    lines = [
        '"""',
        f"Auto-generated wrapper for {wdef.c_type}.",
        "",
        "Do not edit manually. Regenerate from the GDB script root with:",
        "    python3 scripts/generate_all.py",
        '"""',
        "",
    ]

    # Import parent
    if wdef.parent_type == "lv_obj_t":
        lines.append("from lvglgdb.lvgl.core.lv_obj import LVObject")
    else:
        parent_def = widgets[wdef.parent_type]
        lines.append(f"from .{parent_def.module_name} import {parent_def.class_name}")

    # Import helpers only if needed
    needs = set()
    for f in wdef.fields:
        expr = _field_expr(f)
        if expr is None:
            continue
        if "ptr_or_none" in expr:
            needs.add("ptr_or_none")
        if "safe_string" in expr:
            needs.add("safe_string")
        if "safe_color" in expr:
            needs.add("safe_color")
        if "safe_area" in expr:
            needs.add("safe_area")
        if "safe_point" in expr:
            needs.add("safe_point")
        if "safe_wrapper" in expr:
            needs.add("safe_wrapper")

    if needs:
        imports = ", ".join(sorted(needs))
        lines.append(f"from ._helpers import {imports}")

    lines.append("")
    lines.append("")

    parent_cls = wdef.parent_class_name
    lines.append(f"class {wdef.class_name}({parent_cls}):")
    lines.append(f'    """LVGL {wdef.widget_dir} widget ({wdef.c_type})."""')
    lines.append("")
    lines.append(f"    def __init__(self, obj):")
    lines.append(f"        super().__init__(obj)")
    lines.append(f'        self._wv = self.cast("{wdef.c_type}", ptr=True) or self')
    lines.append("")

    # Properties
    snapshot_fields = []
    for f in wdef.fields:
        expr = _field_expr(f)
        if expr is None:
            continue
        lines.append(f"    @property")
        lines.append(f"    def {f.name}(self):")
        if f.comment:
            lines.append(f'        """{f.comment}"""')
        lines.append(f"        return {expr}")
        lines.append("")
        snapshot_fields.append(f.name)

    # snapshot — override to include widget-specific fields in widget_data
    lines.append(f"    def snapshot(self, include_children=False, include_styles=False):")
    lines.append(f'        """Snapshot with widget-specific fields in widget_data."""')
    lines.append(f"        s = super().snapshot(include_children=include_children, include_styles=include_styles)")

    if snapshot_fields:
        lines.append(f"        d = s.get('widget_data') or {{}}")
        for fname in snapshot_fields:
            lines.append(f'        d["{fname}"] = self.{fname}')
        lines.append(f"        s['widget_data'] = d")

    lines.append(f"        return s")
    lines.append("")

    return "\n".join(lines)


def gen_init(ordered: list[WidgetDef]) -> str:
    """Generate __init__.py with imports, registry, and wrap_widget()."""
    lines = [
        '"""',
        "Auto-generated LVGL widget wrappers.",
        "",
        "Do not edit manually. Regenerate from the GDB script root with:",
        "    python3 scripts/generate_all.py",
        '"""',
        "",
    ]

    # Imports
    for w in ordered:
        lines.append(f"from .{w.module_name} import {w.class_name}")
    lines.append("")

    # Registry
    lines.append("WIDGET_REGISTRY: dict[str, type] = {")
    for w in ordered:
        lines.append(f'    "{w.c_class_name}": {w.class_name},')
    lines.append("}")
    lines.append("")
    lines.append("")

    # wrap_widget
    lines.append("import gdb")
    lines.append("")
    lines.append("")
    lines.append("def wrap_widget(obj):")
    lines.append('    """Wrap an LVObject into its widget class if known."""')
    lines.append("    name = obj.class_name")
    lines.append("    # Before 6d999331d (Feb 2025), LVGL class names had no 'lv_' prefix")
    lines.append("    # (e.g. 'label' instead of 'lv_label'). Try both for compatibility.")
    lines.append("    cls = WIDGET_REGISTRY.get(name) or WIDGET_REGISTRY.get('lv_' + name)")
    lines.append("    if cls:")
    lines.append("        try:")
    lines.append("            return cls(obj)")
    lines.append("        except gdb.error:")
    lines.append("            pass  # type not available in debug info")
    lines.append("    return None")
    lines.append("")

    # __all__
    names = [w.class_name for w in ordered]
    lines.append("__all__ = [")
    for n in names:
        lines.append(f'    "{n}",')
    lines.append('    "WIDGET_REGISTRY",')
    lines.append('    "wrap_widget",')
    lines.append("]")
    lines.append("")

    return "\n".join(lines)


def main():
    widgets = parse_widgets()
    ordered = _topo_sort(widgets)
    print(f"Parsed {len(widgets)} widget types")

    for w in ordered:
        print(f"  {w.module_name}.py: {w.class_name}({w.parent_class_name}) — {len(w.fields)} fields")

    OUTPUT_DIR.mkdir(parents=True, exist_ok=True)

    # _helpers.py
    (OUTPUT_DIR / "_helpers.py").write_text(gen_helpers())

    # Per-widget files
    for w in ordered:
        src = gen_widget_file(w, widgets)
        (OUTPUT_DIR / f"{w.module_name}.py").write_text(src)

    # __init__.py
    (OUTPUT_DIR / "__init__.py").write_text(gen_init(ordered))

    print(f"\nGenerated {len(ordered) + 2} files in {OUTPUT_DIR}/")


if __name__ == "__main__":
    main()
