"""
Auto-generated wrapper for lv_dropdown_t.

Do not edit manually. Regenerate from the GDB script root with:
    python3 scripts/generate_all.py
"""

from lvglgdb.lvgl.core.lv_obj import LVObject
from ._helpers import ptr_or_none, safe_string


class LVDropdown(LVObject):
    """LVGL dropdown widget (lv_dropdown_t)."""

    def __init__(self, obj):
        super().__init__(obj)
        self._wv = self.cast("lv_dropdown_t", ptr=True) or self

    @property
    def list(self):
        """The dropped down list"""
        return ptr_or_none(self._wv.safe_field("list"))

    @property
    def text(self):
        """Text to display on the dropdown's button"""
        return safe_string(self._wv, "text")

    @property
    def symbol(self):
        """Arrow or other icon when the drop-down list is closed"""
        return ptr_or_none(self._wv.safe_field("symbol"))

    @property
    def options(self):
        """Options in a '\n' separated list"""
        return safe_string(self._wv, "options")

    @property
    def option_cnt(self):
        """Number of options"""
        return int(self._wv.safe_field("option_cnt", 0))

    @property
    def sel_opt_id(self):
        """Index of the currently selected option"""
        return int(self._wv.safe_field("sel_opt_id", 0))

    @property
    def sel_opt_id_orig(self):
        """Store the original index on focus"""
        return int(self._wv.safe_field("sel_opt_id_orig", 0))

    @property
    def pr_opt_id(self):
        """Index of the currently pressed option"""
        return int(self._wv.safe_field("pr_opt_id", 0))

    @property
    def dir(self):
        """Direction in which the list should open"""
        return int(self._wv.safe_field("dir", 0))

    @property
    def static_options(self):
        """1: Only a pointer is saved in `options`"""
        return int(self._wv.safe_field("static_options", 0))

    @property
    def selected_highlight(self):
        """1: Make the selected option highlighted in the list"""
        return int(self._wv.safe_field("selected_highlight", 0))

    @property
    def static_text(self):
        """1: Only a pointer is saved in `text`"""
        return int(self._wv.safe_field("static_text", 0))

    def snapshot(self, include_children=False, include_styles=False):
        """Snapshot with widget-specific fields in widget_data."""
        s = super().snapshot(include_children=include_children, include_styles=include_styles)
        d = s.get('widget_data') or {}
        d["list"] = self.list
        d["text"] = self.text
        d["symbol"] = self.symbol
        d["options"] = self.options
        d["option_cnt"] = self.option_cnt
        d["sel_opt_id"] = self.sel_opt_id
        d["sel_opt_id_orig"] = self.sel_opt_id_orig
        d["pr_opt_id"] = self.pr_opt_id
        d["dir"] = self.dir
        d["static_options"] = self.static_options
        d["selected_highlight"] = self.selected_highlight
        d["static_text"] = self.static_text
        s['widget_data'] = d
        return s
