"""
Auto-generated wrapper for lv_dropdown_list_t.

Do not edit manually. Regenerate from the GDB script root with:
    python3 scripts/generate_all.py
"""

from lvglgdb.lvgl.core.lv_obj import LVObject
from ._helpers import ptr_or_none


class LVDropdownList(LVObject):
    """LVGL dropdown widget (lv_dropdown_list_t)."""

    def __init__(self, obj):
        super().__init__(obj)
        self._wv = self.cast("lv_dropdown_list_t", ptr=True) or self

    @property
    def dropdown(self):
        return ptr_or_none(self._wv.safe_field("dropdown"))

    def snapshot(self, include_children=False, include_styles=False):
        """Snapshot with widget-specific fields in widget_data."""
        s = super().snapshot(include_children=include_children, include_styles=include_styles)
        d = s.get('widget_data') or {}
        d["dropdown"] = self.dropdown
        s['widget_data'] = d
        return s
