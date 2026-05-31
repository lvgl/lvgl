"""
Auto-generated wrapper for lv_roller_t.

Do not edit manually. Regenerate from the GDB script root with:
    python3 scripts/generate_all.py
"""

from lvglgdb.lvgl.core.lv_obj import LVObject


class LVRoller(LVObject):
    """LVGL roller widget (lv_roller_t)."""

    def __init__(self, obj):
        super().__init__(obj)
        self._wv = self.cast("lv_roller_t", ptr=True) or self

    @property
    def option_cnt(self):
        """Number of options"""
        return int(self._wv.safe_field("option_cnt", 0))

    @property
    def sel_opt_id(self):
        """Index of the current option"""
        return int(self._wv.safe_field("sel_opt_id", 0))

    @property
    def sel_opt_id_ori(self):
        """Store the original index on focus"""
        return int(self._wv.safe_field("sel_opt_id_ori", 0))

    @property
    def inf_page_cnt(self):
        """Number of extra pages added to make the roller look infinite"""
        return int(self._wv.safe_field("inf_page_cnt", 0))

    @property
    def mode(self):
        return int(self._wv.safe_field("mode", 0))

    @property
    def moved(self):
        return int(self._wv.safe_field("moved", 0))

    def snapshot(self, include_children=False, include_styles=False):
        """Snapshot with widget-specific fields in widget_data."""
        s = super().snapshot(include_children=include_children, include_styles=include_styles)
        d = s.get('widget_data') or {}
        d["option_cnt"] = self.option_cnt
        d["sel_opt_id"] = self.sel_opt_id
        d["sel_opt_id_ori"] = self.sel_opt_id_ori
        d["inf_page_cnt"] = self.inf_page_cnt
        d["mode"] = self.mode
        d["moved"] = self.moved
        s['widget_data'] = d
        return s
