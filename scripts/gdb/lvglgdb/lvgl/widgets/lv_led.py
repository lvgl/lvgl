"""
Auto-generated wrapper for lv_led_t.

Do not edit manually. Regenerate from the GDB script root with:
    python3 scripts/generate_all.py
"""

from lvglgdb.lvgl.core.lv_obj import LVObject
from ._helpers import safe_color


class LVLed(LVObject):
    """LVGL led widget (lv_led_t)."""

    def __init__(self, obj):
        super().__init__(obj)
        self._wv = self.cast("lv_led_t", ptr=True) or self

    @property
    def color(self):
        return safe_color(self._wv, "color")

    @property
    def bright(self):
        """Current brightness of the LED (0..255)"""
        return int(self._wv.safe_field("bright", 0))

    def snapshot(self, include_children=False, include_styles=False):
        """Snapshot with widget-specific fields in widget_data."""
        s = super().snapshot(include_children=include_children, include_styles=include_styles)
        d = s.get('widget_data') or {}
        d["color"] = self.color
        d["bright"] = self.bright
        s['widget_data'] = d
        return s
