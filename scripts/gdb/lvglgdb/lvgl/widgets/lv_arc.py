"""
Auto-generated wrapper for lv_arc_t.

Do not edit manually. Regenerate from the GDB script root with:
    python3 scripts/generate_all.py
"""

from lvglgdb.lvgl.core.lv_obj import LVObject


class LVArc(LVObject):
    """LVGL arc widget (lv_arc_t)."""

    def __init__(self, obj):
        super().__init__(obj)
        self._wv = self.cast("lv_arc_t", ptr=True) or self

    @property
    def rotation(self):
        return int(self._wv.safe_field("rotation", 0))

    @property
    def indic_angle_start(self):
        return int(self._wv.safe_field("indic_angle_start", 0))

    @property
    def indic_angle_end(self):
        return int(self._wv.safe_field("indic_angle_end", 0))

    @property
    def bg_angle_start(self):
        return int(self._wv.safe_field("bg_angle_start", 0))

    @property
    def bg_angle_end(self):
        return int(self._wv.safe_field("bg_angle_end", 0))

    @property
    def value(self):
        """Current value of the arc"""
        return int(self._wv.safe_field("value", 0))

    @property
    def min_value(self):
        """Minimum value of the arc"""
        return int(self._wv.safe_field("min_value", 0))

    @property
    def max_value(self):
        """Maximum value of the arc"""
        return int(self._wv.safe_field("max_value", 0))

    @property
    def dragging(self):
        return int(self._wv.safe_field("dragging", 0))

    @property
    def type(self):
        return int(self._wv.safe_field("type", 0))

    @property
    def min_close(self):
        """1: the last pressed angle was closer to minimum end"""
        return int(self._wv.safe_field("min_close", 0))

    @property
    def in_out(self):
        """1: The click was within the background arc angles. 0: Click outside"""
        return int(self._wv.safe_field("in_out", 0))

    @property
    def chg_rate(self):
        """Drag angle rate of change of the arc (degrees/sec)"""
        return int(self._wv.safe_field("chg_rate", 0))

    @property
    def last_tick(self):
        """Last dragging event timestamp of the arc"""
        return int(self._wv.safe_field("last_tick", 0))

    @property
    def last_angle(self):
        """Last dragging angle of the arc"""
        return int(self._wv.safe_field("last_angle", 0))

    @property
    def knob_offset(self):
        """knob offset from the main arc"""
        return int(self._wv.safe_field("knob_offset", 0))

    def snapshot(self, include_children=False, include_styles=False):
        """Snapshot with widget-specific fields in widget_data."""
        s = super().snapshot(include_children=include_children, include_styles=include_styles)
        d = s.get('widget_data') or {}
        d["rotation"] = self.rotation
        d["indic_angle_start"] = self.indic_angle_start
        d["indic_angle_end"] = self.indic_angle_end
        d["bg_angle_start"] = self.bg_angle_start
        d["bg_angle_end"] = self.bg_angle_end
        d["value"] = self.value
        d["min_value"] = self.min_value
        d["max_value"] = self.max_value
        d["dragging"] = self.dragging
        d["type"] = self.type
        d["min_close"] = self.min_close
        d["in_out"] = self.in_out
        d["chg_rate"] = self.chg_rate
        d["last_tick"] = self.last_tick
        d["last_angle"] = self.last_angle
        d["knob_offset"] = self.knob_offset
        s['widget_data'] = d
        return s
