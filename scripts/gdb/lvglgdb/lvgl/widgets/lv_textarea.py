"""
Auto-generated wrapper for lv_textarea_t.

Do not edit manually. Regenerate from the GDB script root with:
    python3 scripts/generate_all.py
"""

from lvglgdb.lvgl.core.lv_obj import LVObject
from ._helpers import ptr_or_none, safe_point, safe_string


class LVTextarea(LVObject):
    """LVGL textarea widget (lv_textarea_t)."""

    def __init__(self, obj):
        super().__init__(obj)
        self._wv = self.cast("lv_textarea_t", ptr=True)

    @property
    def label(self):
        """Label of the text area"""
        return ptr_or_none(self._wv.safe_field("label"))

    @property
    def placeholder_txt(self):
        """Place holder label. only visible if text is an empty string"""
        return safe_string(self._wv, "placeholder_txt")

    @property
    def placeholder_txt_size(self):
        """Size of the placeholder text"""
        return safe_point(self._wv, "placeholder_txt_size")

    @property
    def pwd_tmp(self):
        """Used to store the original text in password mode"""
        return safe_string(self._wv, "pwd_tmp")

    @property
    def pwd_bullet(self):
        """Replacement characters displayed in password mode"""
        return safe_string(self._wv, "pwd_bullet")

    @property
    def accepted_chars(self):
        """Only these characters will be accepted. NULL: accept all"""
        return safe_string(self._wv, "accepted_chars")

    @property
    def max_length(self):
        """The max. number of characters. 0: no limit"""
        return int(self._wv.safe_field("max_length", 0))

    @property
    def pwd_show_time(self):
        """Time to show characters in password mode before change them to '*'"""
        return int(self._wv.safe_field("pwd_show_time", 0))

    def snapshot(self, include_children=False, include_styles=False):
        """Snapshot with widget-specific fields in widget_data."""
        s = super().snapshot(include_children=include_children, include_styles=include_styles)
        d = s.get('widget_data') or {}
        d["label"] = self.label
        d["placeholder_txt"] = self.placeholder_txt
        d["placeholder_txt_size"] = self.placeholder_txt_size
        d["pwd_tmp"] = self.pwd_tmp
        d["pwd_bullet"] = self.pwd_bullet
        d["accepted_chars"] = self.accepted_chars
        d["max_length"] = self.max_length
        d["pwd_show_time"] = self.pwd_show_time
        s['widget_data'] = d
        return s
