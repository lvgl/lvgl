"""
Auto-generated wrapper for lv_switch_t.

Do not edit manually. Regenerate from the GDB script root with:
    python3 scripts/generate_all.py
"""

from lvglgdb.lvgl.core.lv_obj import LVObject


class LVSwitch(LVObject):
    """LVGL switch widget (lv_switch_t)."""

    def __init__(self, obj):
        super().__init__(obj)
        self._wv = self.cast("lv_switch_t", ptr=True) or self

    @property
    def anim_state(self):
        return int(self._wv.safe_field("anim_state", 0))

    @property
    def orientation(self):
        """Orientation of switch"""
        return int(self._wv.safe_field("orientation", 0))

    def snapshot(self, include_children=False, include_styles=False):
        """Snapshot with widget-specific fields in widget_data."""
        s = super().snapshot(include_children=include_children, include_styles=include_styles)
        d = s.get('widget_data') or {}
        d["anim_state"] = self.anim_state
        d["orientation"] = self.orientation
        s['widget_data'] = d
        return s
