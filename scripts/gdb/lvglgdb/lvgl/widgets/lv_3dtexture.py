"""
Auto-generated wrapper for lv_3dtexture_t.

Do not edit manually. Regenerate from the GDB script root with:
    python3 scripts/generate_all.py
"""

from lvglgdb.lvgl.core.lv_obj import LVObject


class LV3dtexture(LVObject):
    """LVGL 3dtexture widget (lv_3dtexture_t)."""

    def __init__(self, obj):
        super().__init__(obj)
        self._wv = self.cast("lv_3dtexture_t", ptr=True) or self

    @property
    def id(self):
        return int(self._wv.safe_field("id", 0))

    @property
    def h_flip(self):
        return int(self._wv.safe_field("h_flip", 0))

    @property
    def v_flip(self):
        return int(self._wv.safe_field("v_flip", 0))

    def snapshot(self, include_children=False, include_styles=False):
        """Snapshot with widget-specific fields in widget_data."""
        s = super().snapshot(include_children=include_children, include_styles=include_styles)
        d = s.get('widget_data') or {}
        d["id"] = self.id
        d["h_flip"] = self.h_flip
        d["v_flip"] = self.v_flip
        s['widget_data'] = d
        return s
