"""
Auto-generated wrapper for lv_chart_t.

Do not edit manually. Regenerate from the GDB script root with:
    python3 scripts/generate_all.py
"""

from lvglgdb.lvgl.core.lv_obj import LVObject
from ._helpers import safe_wrapper


class LVChart(LVObject):
    """LVGL chart widget (lv_chart_t)."""

    def __init__(self, obj):
        super().__init__(obj)
        self._wv = self.cast("lv_chart_t", ptr=True) or self

    @property
    def series_ll(self):
        """Linked list for series (stores lv_chart_series_t)"""
        return safe_wrapper(self._wv, "series_ll", "lvglgdb.lvgl.misc.lv_ll", "LVList")

    @property
    def cursor_ll(self):
        """Linked list for cursors (stores lv_chart_cursor_t)"""
        return safe_wrapper(self._wv, "cursor_ll", "lvglgdb.lvgl.misc.lv_ll", "LVList")

    @property
    def pressed_point_id(self):
        return int(self._wv.safe_field("pressed_point_id", 0))

    @property
    def hdiv_cnt(self):
        """Number of horizontal division lines"""
        return int(self._wv.safe_field("hdiv_cnt", 0))

    @property
    def vdiv_cnt(self):
        """Number of vertical division lines"""
        return int(self._wv.safe_field("vdiv_cnt", 0))

    @property
    def point_cnt(self):
        """Number of points in all series"""
        return int(self._wv.safe_field("point_cnt", 0))

    @property
    def type(self):
        """Chart type"""
        return int(self._wv.safe_field("type", 0))

    @property
    def update_mode(self):
        return int(self._wv.safe_field("update_mode", 0))

    def snapshot(self, include_children=False, include_styles=False):
        """Snapshot with widget-specific fields in widget_data."""
        s = super().snapshot(include_children=include_children, include_styles=include_styles)
        d = s.get('widget_data') or {}
        d["series_ll"] = self.series_ll
        d["cursor_ll"] = self.cursor_ll
        d["pressed_point_id"] = self.pressed_point_id
        d["hdiv_cnt"] = self.hdiv_cnt
        d["vdiv_cnt"] = self.vdiv_cnt
        d["point_cnt"] = self.point_cnt
        d["type"] = self.type
        d["update_mode"] = self.update_mode
        s['widget_data'] = d
        return s
