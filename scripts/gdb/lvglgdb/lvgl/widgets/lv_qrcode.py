"""
Auto-generated wrapper for lv_qrcode_t.

Do not edit manually. Regenerate from the GDB script root with:
    python3 scripts/generate_all.py
"""

from .lv_canvas import LVCanvas
from ._helpers import ptr_or_none, safe_color


class LVQrcode(LVCanvas):
    """LVGL qrcode widget (lv_qrcode_t)."""

    def __init__(self, obj):
        super().__init__(obj)
        self._wv = self.cast("lv_qrcode_t", ptr=True) or self

    @property
    def dark_color(self):
        return safe_color(self._wv, "dark_color")

    @property
    def light_color(self):
        return safe_color(self._wv, "light_color")

    @property
    def data(self):
        """Copy of the payload; strings are stored with their trailing NUL, binary as-is"""
        return ptr_or_none(self._wv.safe_field("data"))

    @property
    def data_len(self):
        """Stored length in bytes (a string includes its NUL terminator)"""
        return int(self._wv.safe_field("data_len", 0))

    @property
    def quiet_zone(self):
        """Add the QR spec's blank margin around the code (boolean toggle)"""
        return int(self._wv.safe_field("quiet_zone", 0))

    @property
    def auto_update(self):
        """Regenerate the bitmap right away on every change (default: true)"""
        return int(self._wv.safe_field("auto_update", 0))

    @property
    def needs_update(self):
        """The bitmap is out of date and must be regenerated (manual mode only)"""
        return int(self._wv.safe_field("needs_update", 0))

    @property
    def render_failed(self):
        """The last bitmap generation failed (or none has run yet)"""
        return int(self._wv.safe_field("render_failed", 0))

    def snapshot(self, include_children=False, include_styles=False):
        """Snapshot with widget-specific fields in widget_data."""
        s = super().snapshot(include_children=include_children, include_styles=include_styles)
        d = s.get('widget_data') or {}
        d["dark_color"] = self.dark_color
        d["light_color"] = self.light_color
        d["data"] = self.data
        d["data_len"] = self.data_len
        d["quiet_zone"] = self.quiet_zone
        d["auto_update"] = self.auto_update
        d["needs_update"] = self.needs_update
        d["render_failed"] = self.render_failed
        s['widget_data'] = d
        return s
