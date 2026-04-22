"""
Auto-generated wrapper for lv_label_t.

Do not edit manually. Regenerate from the GDB script root with:
    python3 scripts/generate_all.py
"""

from lvglgdb.lvgl.core.lv_obj import LVObject
from ._helpers import safe_point, safe_string


class LVLabel(LVObject):
    """LVGL label widget (lv_label_t)."""

    def __init__(self, obj):
        super().__init__(obj)
        self._wv = self.cast("lv_label_t", ptr=True) or self

    @property
    def text(self):
        return safe_string(self._wv, "text")

    @property
    def translation_tag(self):
        return safe_string(self._wv, "translation_tag")

    @property
    def dot_begin(self):
        """Offset where bytes have been replaced with dots"""
        return int(self._wv.safe_field("dot_begin", 0))

    @property
    def sel_start(self):
        return int(self._wv.safe_field("sel_start", 0))

    @property
    def sel_end(self):
        return int(self._wv.safe_field("sel_end", 0))

    @property
    def size_cache(self):
        """Text size cache"""
        return safe_point(self._wv, "size_cache")

    @property
    def offset(self):
        """Text draw position offset"""
        return safe_point(self._wv, "offset")

    @property
    def long_mode(self):
        """Determine what to do with the long texts"""
        return int(self._wv.safe_field("long_mode", 0))

    @property
    def static_txt(self):
        """Flag to indicate the text is static"""
        return int(self._wv.safe_field("static_txt", 0))

    @property
    def recolor(self):
        """Enable in-line letter re-coloring"""
        return int(self._wv.safe_field("recolor", 0))

    @property
    def expand(self):
        """Ignore real width (used by the library with LV_LABEL_LONG_MODE_SCROLL)"""
        return int(self._wv.safe_field("expand", 0))

    @property
    def invalid_size_cache(self):
        """1: Recalculate size and update cache"""
        return int(self._wv.safe_field("invalid_size_cache", 0))

    @property
    def need_refr_text(self):
        """1: Refresh text after layout update completion"""
        return int(self._wv.safe_field("need_refr_text", 0))

    @property
    def text_size(self):
        return safe_point(self._wv, "text_size")

    def snapshot(self, include_children=False, include_styles=False):
        """Snapshot with widget-specific fields in widget_data."""
        s = super().snapshot(include_children=include_children, include_styles=include_styles)
        d = s.get('widget_data') or {}
        d["text"] = self.text
        d["translation_tag"] = self.translation_tag
        d["dot_begin"] = self.dot_begin
        d["sel_start"] = self.sel_start
        d["sel_end"] = self.sel_end
        d["size_cache"] = self.size_cache
        d["offset"] = self.offset
        d["long_mode"] = self.long_mode
        d["static_txt"] = self.static_txt
        d["recolor"] = self.recolor
        d["expand"] = self.expand
        d["invalid_size_cache"] = self.invalid_size_cache
        d["need_refr_text"] = self.need_refr_text
        d["text_size"] = self.text_size
        s['widget_data'] = d
        return s
