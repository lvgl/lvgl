"""
Auto-generated wrapper for lv_menu_page_t.

Do not edit manually. Regenerate from the GDB script root with:
    python3 scripts/generate_all.py
"""

from lvglgdb.lvgl.core.lv_obj import LVObject
from ._helpers import safe_string


class LVMenuPage(LVObject):
    """LVGL menu widget (lv_menu_page_t)."""

    def __init__(self, obj):
        super().__init__(obj)
        self._wv_lv_menu_page_t = self.cast("lv_menu_page_t", ptr=True) or self
        self._wv = self._wv_lv_menu_page_t

    @property
    def title(self):
        return safe_string(self._wv_lv_menu_page_t, "title")

    @property
    def static_title(self):
        return int(self._wv_lv_menu_page_t.safe_field("static_title", 0))

    def snapshot(self, include_children=False, include_styles=False):
        """Snapshot with widget-specific fields in widget_data."""
        s = super().snapshot(include_children=include_children, include_styles=include_styles)
        d = s.get('widget_data') or {}
        d["title"] = self.title
        d["static_title"] = self.static_title
        s['widget_data'] = d
        return s
