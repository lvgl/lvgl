"""
Auto-generated wrapper for lv_spinbox_t.

Do not edit manually. Regenerate from the GDB script root with:
    python3 scripts/generate_all.py
"""

from .lv_textarea import LVTextarea


class LVSpinbox(LVTextarea):
    """LVGL spinbox widget (lv_spinbox_t)."""

    def __init__(self, obj):
        super().__init__(obj)
        self._wv = self.cast("lv_spinbox_t", ptr=True) or self

    @property
    def value(self):
        return int(self._wv.safe_field("value", 0))

    @property
    def range_max(self):
        return int(self._wv.safe_field("range_max", 0))

    @property
    def range_min(self):
        return int(self._wv.safe_field("range_min", 0))

    @property
    def step(self):
        return int(self._wv.safe_field("step", 0))

    @property
    def digit_count(self):
        return int(self._wv.safe_field("digit_count", 0))

    @property
    def dec_point_pos(self):
        """if 0, there is no separator and the number is an integer"""
        return int(self._wv.safe_field("dec_point_pos", 0))

    @property
    def rollover(self):
        """Set to true for rollover functionality"""
        return int(self._wv.safe_field("rollover", 0))

    @property
    def digit_step_dir(self):
        """the direction the digit will step on encoder button press when editing"""
        return int(self._wv.safe_field("digit_step_dir", 0))

    def snapshot(self, include_children=False, include_styles=False):
        """Snapshot with widget-specific fields in widget_data."""
        s = super().snapshot(include_children=include_children, include_styles=include_styles)
        d = s.get('widget_data') or {}
        d["value"] = self.value
        d["range_max"] = self.range_max
        d["range_min"] = self.range_min
        d["step"] = self.step
        d["digit_count"] = self.digit_count
        d["dec_point_pos"] = self.dec_point_pos
        d["rollover"] = self.rollover
        d["digit_step_dir"] = self.digit_step_dir
        s['widget_data'] = d
        return s
