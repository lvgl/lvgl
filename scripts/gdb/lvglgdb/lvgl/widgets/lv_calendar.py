"""
Auto-generated wrapper for lv_calendar_t.

Do not edit manually. Regenerate from the GDB script root with:
    python3 scripts/generate_all.py
"""

from lvglgdb.lvgl.core.lv_obj import LVObject
from ._helpers import ptr_or_none


class LVCalendar(LVObject):
    """LVGL calendar widget (lv_calendar_t)."""

    def __init__(self, obj):
        super().__init__(obj)
        self._wv = self.cast("lv_calendar_t", ptr=True) or self

    @property
    def btnm(self):
        return ptr_or_none(self._wv.safe_field("btnm"))

    @property
    def highlighted_dates(self):
        """Apply different style on these days (pointer to user-defined array)"""
        return ptr_or_none(self._wv.safe_field("highlighted_dates"))

    @property
    def highlighted_dates_num(self):
        """Number of elements in `highlighted_days`"""
        return int(self._wv.safe_field("highlighted_dates_num", 0))

    @property
    def use_chinese_calendar(self):
        return int(self._wv.safe_field("use_chinese_calendar", 0))

    def snapshot(self, include_children=False, include_styles=False):
        """Snapshot with widget-specific fields in widget_data."""
        s = super().snapshot(include_children=include_children, include_styles=include_styles)
        d = s.get('widget_data') or {}
        d["btnm"] = self.btnm
        d["highlighted_dates"] = self.highlighted_dates
        d["highlighted_dates_num"] = self.highlighted_dates_num
        d["use_chinese_calendar"] = self.use_chinese_calendar
        s['widget_data'] = d
        return s
