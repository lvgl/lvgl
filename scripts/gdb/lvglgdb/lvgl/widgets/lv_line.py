"""
Auto-generated wrapper for lv_line_t.

Do not edit manually. Regenerate from the GDB script root with:
    python3 scripts/generate_all.py
"""

from lvglgdb.lvgl.core.lv_obj import LVObject


class LVLine(LVObject):
    """LVGL line widget (lv_line_t)."""

    def __init__(self, obj):
        super().__init__(obj)
        self._wv = self.cast("lv_line_t", ptr=True) or self

    @property
    def point_num(self):
        """Number of points in 'point_array'"""
        return int(self._wv.safe_field("point_num", 0))

    @property
    def y_inv(self):
        """1: y == 0 will be on the bottom"""
        return int(self._wv.safe_field("y_inv", 0))

    @property
    def point_array_is_mutable(self):
        """whether the point array is const or mutable"""
        return int(self._wv.safe_field("point_array_is_mutable", 0))

    def snapshot(self, include_children=False, include_styles=False):
        """Snapshot with widget-specific fields in widget_data."""
        s = super().snapshot(include_children=include_children, include_styles=include_styles)
        d = s.get('widget_data') or {}
        d["point_num"] = self.point_num
        d["y_inv"] = self.y_inv
        d["point_array_is_mutable"] = self.point_array_is_mutable
        s['widget_data'] = d
        return s
