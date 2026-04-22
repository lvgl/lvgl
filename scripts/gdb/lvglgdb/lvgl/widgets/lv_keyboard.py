"""
Auto-generated wrapper for lv_keyboard_t.

Do not edit manually. Regenerate from the GDB script root with:
    python3 scripts/generate_all.py
"""

from .lv_buttonmatrix import LVButtonmatrix
from ._helpers import ptr_or_none


class LVKeyboard(LVButtonmatrix):
    """LVGL keyboard widget (lv_keyboard_t)."""

    def __init__(self, obj):
        super().__init__(obj)
        self._wv = self.cast("lv_keyboard_t", ptr=True) or self

    @property
    def ta(self):
        """Pointer to the assigned text area"""
        return ptr_or_none(self._wv.safe_field("ta"))

    @property
    def mode(self):
        """Key map type"""
        return int(self._wv.safe_field("mode", 0))

    @property
    def popovers(self):
        """Show button titles in popovers on press"""
        return int(self._wv.safe_field("popovers", 0))

    def snapshot(self, include_children=False, include_styles=False):
        """Snapshot with widget-specific fields in widget_data."""
        s = super().snapshot(include_children=include_children, include_styles=include_styles)
        d = s.get('widget_data') or {}
        d["ta"] = self.ta
        d["mode"] = self.mode
        d["popovers"] = self.popovers
        s['widget_data'] = d
        return s
