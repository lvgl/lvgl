"""
Auto-generated wrapper for lv_tabview_t.

Do not edit manually. Regenerate from the GDB script root with:
    python3 scripts/generate_all.py
"""

from lvglgdb.lvgl.core.lv_obj import LVObject


class LVTabview(LVObject):
    """LVGL tabview widget (lv_tabview_t)."""

    def __init__(self, obj):
        super().__init__(obj)
        self._wv = self.cast("lv_tabview_t", ptr=True) or self

    @property
    def tab_cur(self):
        return int(self._wv.safe_field("tab_cur", 0))

    @property
    def tab_pos(self):
        return int(self._wv.safe_field("tab_pos", 0))

    @property
    def tab_bar_size(self):
        return int(self._wv.safe_field("tab_bar_size", 0))

    def snapshot(self, include_children=False, include_styles=False):
        """Snapshot with widget-specific fields in widget_data."""
        s = super().snapshot(include_children=include_children, include_styles=include_styles)
        d = s.get('widget_data') or {}
        d["tab_cur"] = self.tab_cur
        d["tab_pos"] = self.tab_pos
        d["tab_bar_size"] = self.tab_bar_size
        s['widget_data'] = d
        return s
