"""
Auto-generated wrapper for lv_slider_t.

Do not edit manually. Regenerate from the GDB script root with:
    python3 scripts/generate_all.py
"""

from .lv_bar import LVBar
from ._helpers import ptr_or_none, safe_area, safe_point


class LVSlider(LVBar):
    """LVGL slider widget (lv_slider_t)."""

    def __init__(self, obj):
        super().__init__(obj)
        self._wv = self.cast("lv_slider_t", ptr=True) or self

    @property
    def left_knob_area(self):
        return safe_area(self._wv, "left_knob_area")

    @property
    def right_knob_area(self):
        return safe_area(self._wv, "right_knob_area")

    @property
    def pressed_point(self):
        return safe_point(self._wv, "pressed_point")

    @property
    def value_to_set(self):
        """Which bar value to set"""
        return ptr_or_none(self._wv.safe_field("value_to_set"))

    @property
    def dragging(self):
        """1: the slider is being dragged"""
        return int(self._wv.safe_field("dragging", 0))

    @property
    def left_knob_focus(self):
        """1: with encoder now the right knob can be adjusted"""
        return int(self._wv.safe_field("left_knob_focus", 0))

    def snapshot(self, include_children=False, include_styles=False):
        """Snapshot with widget-specific fields in widget_data."""
        s = super().snapshot(include_children=include_children, include_styles=include_styles)
        d = s.get('widget_data') or {}
        d["left_knob_area"] = self.left_knob_area
        d["right_knob_area"] = self.right_knob_area
        d["pressed_point"] = self.pressed_point
        d["value_to_set"] = self.value_to_set
        d["dragging"] = self.dragging
        d["left_knob_focus"] = self.left_knob_focus
        s['widget_data'] = d
        return s
