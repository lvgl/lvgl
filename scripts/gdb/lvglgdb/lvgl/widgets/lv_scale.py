"""
Auto-generated wrapper for lv_scale_t.

Do not edit manually. Regenerate from the GDB script root with:
    python3 scripts/generate_all.py
"""

from lvglgdb.lvgl.core.lv_obj import LVObject
from ._helpers import ptr_or_none, safe_wrapper


class LVScale(LVObject):
    """LVGL scale widget (lv_scale_t)."""

    def __init__(self, obj):
        super().__init__(obj)
        self._wv = self.cast("lv_scale_t", ptr=True) or self

    @property
    def section_ll(self):
        """Linked list for the sections (stores lv_scale_section_t)"""
        return safe_wrapper(self._wv, "section_ll", "lvglgdb.lvgl.misc.lv_ll", "LVList")

    @property
    def txt_src(self):
        return ptr_or_none(self._wv.safe_field("txt_src"))

    @property
    def mode(self):
        """Orientation and layout of scale."""
        return int(self._wv.safe_field("mode", 0))

    @property
    def range_min(self):
        """Scale's minimum value"""
        return int(self._wv.safe_field("range_min", 0))

    @property
    def range_max(self):
        """Scale's maximum value"""
        return int(self._wv.safe_field("range_max", 0))

    @property
    def total_tick_count(self):
        """Total number of ticks (major and minor)"""
        return int(self._wv.safe_field("total_tick_count", 0))

    @property
    def major_tick_every(self):
        """Frequency of major ticks to minor ticks"""
        return int(self._wv.safe_field("major_tick_every", 0))

    @property
    def label_enabled(self):
        """Draw labels for major ticks?"""
        return int(self._wv.safe_field("label_enabled", 0))

    @property
    def post_draw(self):
        return int(self._wv.safe_field("post_draw", 0))

    @property
    def draw_ticks_on_top(self):
        """Draw ticks on top of main line?"""
        return int(self._wv.safe_field("draw_ticks_on_top", 0))

    @property
    def angle_range(self):
        """Degrees between low end and high end of scale"""
        return int(self._wv.safe_field("angle_range", 0))

    @property
    def rotation(self):
        """Clockwise angular offset from 3-o'clock position of low end of scale"""
        return int(self._wv.safe_field("rotation", 0))

    @property
    def custom_label_cnt(self):
        """Number of custom labels provided in `txt_src`"""
        return int(self._wv.safe_field("custom_label_cnt", 0))

    @property
    def last_tick_width(self):
        """Width of last tick in pixels"""
        return int(self._wv.safe_field("last_tick_width", 0))

    @property
    def first_tick_width(self):
        """Width of first tick in pixels"""
        return int(self._wv.safe_field("first_tick_width", 0))

    @property
    def needles(self):
        """Needle list of this scale"""
        return safe_wrapper(self._wv, "needles", "lvglgdb.lvgl.misc.lv_array", "LVArray")

    def snapshot(self, include_children=False, include_styles=False):
        """Snapshot with widget-specific fields in widget_data."""
        s = super().snapshot(include_children=include_children, include_styles=include_styles)
        d = s.get('widget_data') or {}
        d["section_ll"] = self.section_ll
        d["txt_src"] = self.txt_src
        d["mode"] = self.mode
        d["range_min"] = self.range_min
        d["range_max"] = self.range_max
        d["total_tick_count"] = self.total_tick_count
        d["major_tick_every"] = self.major_tick_every
        d["label_enabled"] = self.label_enabled
        d["post_draw"] = self.post_draw
        d["draw_ticks_on_top"] = self.draw_ticks_on_top
        d["angle_range"] = self.angle_range
        d["rotation"] = self.rotation
        d["custom_label_cnt"] = self.custom_label_cnt
        d["last_tick_width"] = self.last_tick_width
        d["first_tick_width"] = self.first_tick_width
        d["needles"] = self.needles
        s['widget_data'] = d
        return s
