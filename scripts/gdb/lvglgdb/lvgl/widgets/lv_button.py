"""
Auto-generated wrapper for lv_button_t.

Do not edit manually. Regenerate from the GDB script root with:
    python3 scripts/generate_all.py
"""

from lvglgdb.lvgl.core.lv_obj import LVObject


class LVButton(LVObject):
    """LVGL button widget (lv_button_t)."""

    def __init__(self, obj):
        super().__init__(obj)
        self._wv = self.cast("lv_button_t", ptr=True) or self

    def snapshot(self, include_children=False, include_styles=False):
        """Snapshot with widget-specific fields in widget_data."""
        s = super().snapshot(include_children=include_children, include_styles=include_styles)
        return s
