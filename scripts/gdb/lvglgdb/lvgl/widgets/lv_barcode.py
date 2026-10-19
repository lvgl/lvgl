"""
Auto-generated wrapper for lv_barcode_t.

Do not edit manually. Regenerate from the GDB script root with:
    python3 scripts/generate_all.py
"""

from .lv_canvas import LVCanvas
from ._helpers import safe_color, safe_string


class LVBarcode(LVCanvas):
    """LVGL barcode widget (lv_barcode_t)."""

    def __init__(self, obj):
        super().__init__(obj)
        self._wv_lv_barcode_t = self.cast("lv_barcode_t", ptr=True) or self
        self._wv = self._wv_lv_barcode_t

    @property
    def dark_color(self):
        return safe_color(self._wv_lv_barcode_t, "dark_color")

    @property
    def light_color(self):
        return safe_color(self._wv_lv_barcode_t, "light_color")

    @property
    def data(self):
        return safe_string(self._wv_lv_barcode_t, "data")

    @property
    def direction(self):
        return int(self._wv_lv_barcode_t.safe_field("direction", 0))

    @property
    def encoding(self):
        return int(self._wv_lv_barcode_t.safe_field("encoding", 0))

    @property
    def scale(self):
        return int(self._wv_lv_barcode_t.safe_field("scale", 0))

    def snapshot(self, include_children=False, include_styles=False):
        """Snapshot with widget-specific fields in widget_data."""
        s = super().snapshot(include_children=include_children, include_styles=include_styles)
        d = s.get('widget_data') or {}
        d["dark_color"] = self.dark_color
        d["light_color"] = self.light_color
        d["data"] = self.data
        d["direction"] = self.direction
        d["encoding"] = self.encoding
        d["scale"] = self.scale
        s['widget_data'] = d
        return s
