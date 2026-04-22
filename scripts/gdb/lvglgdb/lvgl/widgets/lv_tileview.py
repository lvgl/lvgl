"""
Auto-generated wrapper for lv_tileview_t.

Do not edit manually. Regenerate from the GDB script root with:
    python3 scripts/generate_all.py
"""

from lvglgdb.lvgl.core.lv_obj import LVObject
from ._helpers import ptr_or_none


class LVTileview(LVObject):
    """LVGL tileview widget (lv_tileview_t)."""

    def __init__(self, obj):
        super().__init__(obj)
        self._wv = self.cast("lv_tileview_t", ptr=True) or self

    @property
    def tile_act(self):
        return ptr_or_none(self._wv.safe_field("tile_act"))

    def snapshot(self, include_children=False, include_styles=False):
        """Snapshot with widget-specific fields in widget_data."""
        s = super().snapshot(include_children=include_children, include_styles=include_styles)
        d = s.get('widget_data') or {}
        d["tile_act"] = self.tile_act
        s['widget_data'] = d
        return s
