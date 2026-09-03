"""
Auto-generated wrapper for lv_barcode_t.

Do not edit manually. Regenerate from the GDB script root with:
    python3 scripts/generate_all.py
"""

from .lv_canvas import LVCanvas
from ._helpers import ptr_or_none, safe_color, safe_string


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
        """Copy of the payload, kept so the bitmap can be regenerated on a property change"""
        return safe_string(self._wv_lv_barcode_t, "data")

    @property
    def pattern(self):
        return ptr_or_none(self._wv_lv_barcode_t.safe_field("pattern"))

    @property
    def bar_count(self):
        """Bars `data` encodes to; 0 when it is not known and has to be encoded"""
        return int(self._wv_lv_barcode_t.safe_field("bar_count", 0))

    @property
    def scale(self):
        """Pixel width of a single bar"""
        return int(self._wv_lv_barcode_t.safe_field("scale", 0))

    @property
    def direction(self):
        return int(self._wv_lv_barcode_t.safe_field("direction", 0))

    @property
    def encoding(self):
        return int(self._wv_lv_barcode_t.safe_field("encoding", 0))

    @property
    def tiled(self):
        """Draw a one bar wide bitmap and let the image tiling repeat it"""
        return int(self._wv_lv_barcode_t.safe_field("tiled", 0))

    @property
    def update_mode(self):
        """lv_barcode_update_mode_t: when a property change is regenerated"""
        return int(self._wv_lv_barcode_t.safe_field("update_mode", 0))

    @property
    def needs_update(self):
        """The bitmap is out of date; filled in on the next redraw (deferred mode)"""
        return int(self._wv_lv_barcode_t.safe_field("needs_update", 0))

    @property
    def render_valid(self):
        """No generation attempt is known to have failed; a change re-arms it"""
        return int(self._wv_lv_barcode_t.safe_field("render_valid", 0))

    @property
    def fitting(self):
        """Guard against the re-entrant resize our own reallocation triggers"""
        return int(self._wv_lv_barcode_t.safe_field("fitting", 0))

    def snapshot(self, include_children=False, include_styles=False):
        """Snapshot with widget-specific fields in widget_data."""
        s = super().snapshot(include_children=include_children, include_styles=include_styles)
        d = s.get('widget_data') or {}
        d["dark_color"] = self.dark_color
        d["light_color"] = self.light_color
        d["data"] = self.data
        d["pattern"] = self.pattern
        d["bar_count"] = self.bar_count
        d["scale"] = self.scale
        d["direction"] = self.direction
        d["encoding"] = self.encoding
        d["tiled"] = self.tiled
        d["update_mode"] = self.update_mode
        d["needs_update"] = self.needs_update
        d["render_valid"] = self.render_valid
        d["fitting"] = self.fitting
        s['widget_data'] = d
        return s
