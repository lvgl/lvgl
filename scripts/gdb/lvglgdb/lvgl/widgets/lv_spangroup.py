"""
Auto-generated wrapper for lv_spangroup_t.

Do not edit manually. Regenerate from the GDB script root with:
    python3 scripts/generate_all.py
"""

from lvglgdb.lvgl.core.lv_obj import LVObject
from ._helpers import safe_wrapper


class LVSpangroup(LVObject):
    """LVGL span widget (lv_spangroup_t)."""

    def __init__(self, obj):
        super().__init__(obj)
        self._wv = self.cast("lv_spangroup_t", ptr=True) or self

    @property
    def lines(self):
        return int(self._wv.safe_field("lines", 0))

    @property
    def indent(self):
        """first line indent"""
        return int(self._wv.safe_field("indent", 0))

    @property
    def cache_w(self):
        """the cache automatically calculates the width"""
        return int(self._wv.safe_field("cache_w", 0))

    @property
    def cache_h(self):
        """similar cache_w"""
        return int(self._wv.safe_field("cache_h", 0))

    @property
    def child_ll(self):
        return safe_wrapper(self._wv, "child_ll", "lvglgdb.lvgl.misc.lv_ll", "LVList")

    @property
    def overflow(self):
        """details see lv_span_overflow_t"""
        return int(self._wv.safe_field("overflow", 0))

    @property
    def refresh(self):
        """the spangroup need refresh cache_w and cache_h"""
        return int(self._wv.safe_field("refresh", 0))

    def snapshot(self, include_children=False, include_styles=False):
        """Snapshot with widget-specific fields in widget_data."""
        s = super().snapshot(include_children=include_children, include_styles=include_styles)
        d = s.get('widget_data') or {}
        d["lines"] = self.lines
        d["indent"] = self.indent
        d["cache_w"] = self.cache_w
        d["cache_h"] = self.cache_h
        d["child_ll"] = self.child_ll
        d["overflow"] = self.overflow
        d["refresh"] = self.refresh
        s['widget_data'] = d
        return s
