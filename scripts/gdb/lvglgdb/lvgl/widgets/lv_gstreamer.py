"""
Auto-generated wrapper for lv_gstreamer_t.

Do not edit manually. Regenerate from the GDB script root with:
    python3 scripts/generate_all.py
"""

from .lv_image import LVImage
from ._helpers import ptr_or_none


class LVGstreamer(LVImage):
    """LVGL gstreamer widget (lv_gstreamer_t)."""

    def __init__(self, obj):
        super().__init__(obj)
        self._wv = self.cast("lv_gstreamer_t", ptr=True) or self

    @property
    def last_buffer(self):
        return ptr_or_none(self._wv.safe_field("last_buffer"))

    @property
    def last_sample(self):
        return ptr_or_none(self._wv.safe_field("last_sample"))

    @property
    def pipeline(self):
        return ptr_or_none(self._wv.safe_field("pipeline"))

    @property
    def audio_convert(self):
        return ptr_or_none(self._wv.safe_field("audio_convert"))

    @property
    def video_convert(self):
        return ptr_or_none(self._wv.safe_field("video_convert"))

    @property
    def audio_volume(self):
        return ptr_or_none(self._wv.safe_field("audio_volume"))

    @property
    def gstreamer_timer(self):
        return ptr_or_none(self._wv.safe_field("gstreamer_timer"))

    @property
    def frame_queue(self):
        return ptr_or_none(self._wv.safe_field("frame_queue"))

    @property
    def is_video_info_valid(self):
        return int(self._wv.safe_field("is_video_info_valid", 0))

    def snapshot(self, include_children=False, include_styles=False):
        """Snapshot with widget-specific fields in widget_data."""
        s = super().snapshot(include_children=include_children, include_styles=include_styles)
        d = s.get('widget_data') or {}
        d["last_buffer"] = self.last_buffer
        d["last_sample"] = self.last_sample
        d["pipeline"] = self.pipeline
        d["audio_convert"] = self.audio_convert
        d["video_convert"] = self.video_convert
        d["audio_volume"] = self.audio_volume
        d["gstreamer_timer"] = self.gstreamer_timer
        d["frame_queue"] = self.frame_queue
        d["is_video_info_valid"] = self.is_video_info_valid
        s['widget_data'] = d
        return s
