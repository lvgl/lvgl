"""
Auto-generated wrapper for lv_msgbox_t.

Do not edit manually. Regenerate from the GDB script root with:
    python3 scripts/generate_all.py
"""

from lvglgdb.lvgl.core.lv_obj import LVObject
from ._helpers import ptr_or_none


class LVMsgbox(LVObject):
    """LVGL msgbox widget (lv_msgbox_t)."""

    def __init__(self, obj):
        super().__init__(obj)
        self._wv = self.cast("lv_msgbox_t", ptr=True) or self

    @property
    def header(self):
        return ptr_or_none(self._wv.safe_field("header"))

    @property
    def content(self):
        return ptr_or_none(self._wv.safe_field("content"))

    @property
    def footer(self):
        return ptr_or_none(self._wv.safe_field("footer"))

    @property
    def title(self):
        return ptr_or_none(self._wv.safe_field("title"))

    def snapshot(self, include_children=False, include_styles=False):
        """Snapshot with widget-specific fields in widget_data."""
        s = super().snapshot(include_children=include_children, include_styles=include_styles)
        d = s.get('widget_data') or {}
        d["header"] = self.header
        d["content"] = self.content
        d["footer"] = self.footer
        d["title"] = self.title
        s['widget_data'] = d
        return s
