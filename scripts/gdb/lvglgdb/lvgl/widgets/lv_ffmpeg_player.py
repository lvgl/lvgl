"""
Auto-generated wrapper for lv_ffmpeg_player_t.

Do not edit manually. Regenerate from the GDB script root with:
    python3 scripts/generate_all.py
"""

from .lv_image import LVImage
from ._helpers import ptr_or_none, safe_string


class LVFfmpegPlayer(LVImage):
    """LVGL ffmpeg widget (lv_ffmpeg_player_t)."""

    def __init__(self, obj):
        super().__init__(obj)
        self._wv = self.cast("lv_ffmpeg_player_t", ptr=True) or self

    @property
    def timer(self):
        return ptr_or_none(self._wv.safe_field("timer"))

    @property
    def auto_restart(self):
        return int(self._wv.safe_field("auto_restart", 0))

    @property
    def decoder_name(self):
        return safe_string(self._wv, "decoder_name")

    def snapshot(self, include_children=False, include_styles=False):
        """Snapshot with widget-specific fields in widget_data."""
        s = super().snapshot(include_children=include_children, include_styles=include_styles)
        d = s.get('widget_data') or {}
        d["timer"] = self.timer
        d["auto_restart"] = self.auto_restart
        d["decoder_name"] = self.decoder_name
        s['widget_data'] = d
        return s
