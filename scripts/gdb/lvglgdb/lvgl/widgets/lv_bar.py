"""
Auto-generated wrapper for lv_bar_t.

Do not edit manually. Regenerate from the GDB script root with:
    python3 scripts/generate_all.py
"""

from lvglgdb.lvgl.core.lv_obj import LVObject
from ._helpers import safe_area


class LVBar(LVObject):
    """LVGL bar widget (lv_bar_t)."""

    def __init__(self, obj):
        super().__init__(obj)
        self._wv = self.cast("lv_bar_t", ptr=True) or self

    @property
    def cur_value(self):
        """Current value of the bar"""
        return int(self._wv.safe_field("cur_value", 0))

    @property
    def min_value(self):
        """Minimum value of the bar"""
        return int(self._wv.safe_field("min_value", 0))

    @property
    def max_value(self):
        """Maximum value of the bar"""
        return int(self._wv.safe_field("max_value", 0))

    @property
    def start_value(self):
        """Start value of the bar"""
        return int(self._wv.safe_field("start_value", 0))

    @property
    def indic_area(self):
        """Save the indicator area. Might be used by derived types"""
        return safe_area(self._wv, "indic_area")

    @property
    def val_reversed(self):
        """Whether value been reversed"""
        return int(self._wv.safe_field("val_reversed", 0))

    @property
    def mode(self):
        """Type of bar"""
        return int(self._wv.safe_field("mode", 0))

    @property
    def orientation(self):
        """Orientation of bar"""
        return int(self._wv.safe_field("orientation", 0))

    def snapshot(self, include_children=False, include_styles=False):
        """Snapshot with widget-specific fields in widget_data."""
        s = super().snapshot(include_children=include_children, include_styles=include_styles)
        d = s.get('widget_data') or {}
        d["cur_value"] = self.cur_value
        d["min_value"] = self.min_value
        d["max_value"] = self.max_value
        d["start_value"] = self.start_value
        d["indic_area"] = self.indic_area
        d["val_reversed"] = self.val_reversed
        d["mode"] = self.mode
        d["orientation"] = self.orientation
        s['widget_data'] = d
        return s
