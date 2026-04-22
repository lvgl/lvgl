"""
Auto-generated wrapper for lv_tileview_tile_t.

Do not edit manually. Regenerate from the GDB script root with:
    python3 scripts/generate_all.py
"""

from lvglgdb.lvgl.core.lv_obj import LVObject


class LVTileviewTile(LVObject):
    """LVGL tileview widget (lv_tileview_tile_t)."""

    def __init__(self, obj):
        super().__init__(obj)
        self._wv = self.cast("lv_tileview_tile_t", ptr=True) or self

    @property
    def dir(self):
        return int(self._wv.safe_field("dir", 0))

    def snapshot(self, include_children=False, include_styles=False):
        """Snapshot with widget-specific fields in widget_data."""
        s = super().snapshot(include_children=include_children, include_styles=include_styles)
        d = s.get('widget_data') or {}
        d["dir"] = self.dir
        s['widget_data'] = d
        return s
