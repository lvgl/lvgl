"""
Auto-generated wrapper for lv_arclabel_t.

Do not edit manually. Regenerate from the GDB script root with:
    python3 scripts/generate_all.py
"""

from lvglgdb.lvgl.core.lv_obj import LVObject
from ._helpers import safe_point, safe_string


class LVArclabel(LVObject):
    """LVGL arclabel widget (lv_arclabel_t)."""

    def __init__(self, obj):
        super().__init__(obj)
        self._wv = self.cast("lv_arclabel_t", ptr=True) or self

    @property
    def text(self):
        return safe_string(self._wv, "text")

    @property
    def dot_begin(self):
        """Offset where bytes have been replaced with dots"""
        return int(self._wv.safe_field("dot_begin", 0))

    @property
    def angle_start(self):
        return int(self._wv.safe_field("angle_start", 0))

    @property
    def angle_size(self):
        return int(self._wv.safe_field("angle_size", 0))

    @property
    def offset(self):
        return int(self._wv.safe_field("offset", 0))

    @property
    def radius(self):
        return int(self._wv.safe_field("radius", 0))

    @property
    def center_offset(self):
        return safe_point(self._wv, "center_offset")

    @property
    def dir(self):
        return int(self._wv.safe_field("dir", 0))

    @property
    def text_align_v(self):
        """Vertical text alignment"""
        return int(self._wv.safe_field("text_align_v", 0))

    @property
    def text_align_h(self):
        """Horizontal text alignment"""
        return int(self._wv.safe_field("text_align_h", 0))

    @property
    def static_txt(self):
        """Flag to indicate the text is static"""
        return int(self._wv.safe_field("static_txt", 0))

    @property
    def recolor(self):
        """Enable in-line letter re-coloring"""
        return int(self._wv.safe_field("recolor", 0))

    @property
    def overflow(self):
        """Overflow mode: 0=visible, 1=ellipsis, 2=clip"""
        return int(self._wv.safe_field("overflow", 0))

    @property
    def end_overlap(self):
        """End overlap flag, false if prevent end overlap"""
        return int(self._wv.safe_field("end_overlap", 0))

    def snapshot(self, include_children=False, include_styles=False):
        """Snapshot with widget-specific fields in widget_data."""
        s = super().snapshot(include_children=include_children, include_styles=include_styles)
        d = s.get('widget_data') or {}
        d["text"] = self.text
        d["dot_begin"] = self.dot_begin
        d["angle_start"] = self.angle_start
        d["angle_size"] = self.angle_size
        d["offset"] = self.offset
        d["radius"] = self.radius
        d["center_offset"] = self.center_offset
        d["dir"] = self.dir
        d["text_align_v"] = self.text_align_v
        d["text_align_h"] = self.text_align_h
        d["static_txt"] = self.static_txt
        d["recolor"] = self.recolor
        d["overflow"] = self.overflow
        d["end_overlap"] = self.end_overlap
        s['widget_data'] = d
        return s
