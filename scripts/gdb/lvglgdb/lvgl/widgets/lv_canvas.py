"""
Auto-generated wrapper for lv_canvas_t.

Do not edit manually. Regenerate from the GDB script root with:
    python3 scripts/generate_all.py
"""

from .lv_image import LVImage
from ._helpers import ptr_or_none, safe_wrapper


class LVCanvas(LVImage):
    """LVGL canvas widget (lv_canvas_t)."""

    def __init__(self, obj):
        super().__init__(obj)
        self._wv = self.cast("lv_canvas_t", ptr=True) or self

    @property
    def draw_buf(self):
        return ptr_or_none(self._wv.safe_field("draw_buf"))

    @property
    def static_buf(self):
        return safe_wrapper(self._wv, "static_buf", "lvglgdb.lvgl.draw.lv_draw_buf", "LVDrawBuf")

    def snapshot(self, include_children=False, include_styles=False):
        """Snapshot with widget-specific fields in widget_data."""
        s = super().snapshot(include_children=include_children, include_styles=include_styles)
        d = s.get('widget_data') or {}
        d["draw_buf"] = self.draw_buf
        d["static_buf"] = self.static_buf
        s['widget_data'] = d
        return s
