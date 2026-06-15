"""
Auto-generated wrapper for lv_image_t.

Do not edit manually. Regenerate from the GDB script root with:
    python3 scripts/generate_all.py
"""

from lvglgdb.lvgl.core.lv_obj import LVObject
from ._helpers import ptr_or_none, safe_point


class LVImage(LVObject):
    """LVGL image widget (lv_image_t)."""

    def __init__(self, obj):
        super().__init__(obj)
        self._wv = self.cast("lv_image_t", ptr=True) or self

    @property
    def src(self):
        """Image source: Pointer to an array or a file or a symbol"""
        return ptr_or_none(self._wv.safe_field("src"))

    @property
    def bitmap_mask_src(self):
        """Pointer to an A8 bitmap mask"""
        return ptr_or_none(self._wv.safe_field("bitmap_mask_src"))

    @property
    def offset(self):
        return safe_point(self._wv, "offset")

    @property
    def w(self):
        """Width of the image (Handled by the library)"""
        return int(self._wv.safe_field("w", 0))

    @property
    def h(self):
        """Height of the image (Handled by the library)"""
        return int(self._wv.safe_field("h", 0))

    @property
    def rotation(self):
        """Rotation angle of the image"""
        return int(self._wv.safe_field("rotation", 0))

    @property
    def scale_x(self):
        """256 means no zoom, 512 double size, 128 half size"""
        return int(self._wv.safe_field("scale_x", 0))

    @property
    def scale_y(self):
        """256 means no zoom, 512 double size, 128 half size"""
        return int(self._wv.safe_field("scale_y", 0))

    @property
    def pivot(self):
        """Rotation center of the image"""
        return safe_point(self._wv, "pivot")

    @property
    def src_type(self):
        """See: lv_image_src_t"""
        return int(self._wv.safe_field("src_type", 0))

    @property
    def cf(self):
        """Color format from `lv_color_format_t`"""
        return int(self._wv.safe_field("cf", 0))

    @property
    def antialias(self):
        """Apply anti-aliasing in transformations (rotate, zoom)"""
        return int(self._wv.safe_field("antialias", 0))

    @property
    def align(self):
        """Image size mode when image size and object size is different. See lv_image_align_t"""
        return int(self._wv.safe_field("align", 0))

    @property
    def blend_mode(self):
        """Element of `lv_blend_mode_t`"""
        return int(self._wv.safe_field("blend_mode", 0))

    def snapshot(self, include_children=False, include_styles=False):
        """Snapshot with widget-specific fields in widget_data."""
        s = super().snapshot(include_children=include_children, include_styles=include_styles)
        d = s.get('widget_data') or {}
        d["src"] = self.src
        d["bitmap_mask_src"] = self.bitmap_mask_src
        d["offset"] = self.offset
        d["w"] = self.w
        d["h"] = self.h
        d["rotation"] = self.rotation
        d["scale_x"] = self.scale_x
        d["scale_y"] = self.scale_y
        d["pivot"] = self.pivot
        d["src_type"] = self.src_type
        d["cf"] = self.cf
        d["antialias"] = self.antialias
        d["align"] = self.align
        d["blend_mode"] = self.blend_mode
        s['widget_data'] = d
        return s
