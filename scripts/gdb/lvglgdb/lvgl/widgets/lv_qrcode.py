"""
Auto-generated wrapper for lv_qrcode_t.

Do not edit manually. Regenerate from the GDB script root with:
    python3 scripts/generate_all.py
"""

from .lv_canvas import LVCanvas
from ._helpers import safe_color


class LVQrcode(LVCanvas):
    """LVGL qrcode widget (lv_qrcode_t)."""

    def __init__(self, obj):
        super().__init__(obj)
        self._wv_lv_qrcode_t = self.cast("lv_qrcode_t", ptr=True) or self
        self._wv = self._wv_lv_qrcode_t

    @property
    def dark_color(self):
        return safe_color(self._wv_lv_qrcode_t, "dark_color")

    @property
    def light_color(self):
        return safe_color(self._wv_lv_qrcode_t, "light_color")

    @property
    def quiet_zone(self):
        return int(self._wv_lv_qrcode_t.safe_field("quiet_zone", 0))

    def snapshot(self, include_children=False, include_styles=False):
        """Snapshot with widget-specific fields in widget_data."""
        s = super().snapshot(include_children=include_children, include_styles=include_styles)
        d = s.get('widget_data') or {}
        d["dark_color"] = self.dark_color
        d["light_color"] = self.light_color
        d["quiet_zone"] = self.quiet_zone
        s['widget_data'] = d
        return s
