"""
Auto-generated wrapper for lv_buttonmatrix_t.

Do not edit manually. Regenerate from the GDB script root with:
    python3 scripts/generate_all.py
"""

from lvglgdb.lvgl.core.lv_obj import LVObject
from ._helpers import ptr_or_none


class LVButtonmatrix(LVObject):
    """LVGL buttonmatrix widget (lv_buttonmatrix_t)."""

    def __init__(self, obj):
        super().__init__(obj)
        self._wv = self.cast("lv_buttonmatrix_t", ptr=True) or self

    @property
    def map_p(self):
        """Pointer to the current map"""
        return ptr_or_none(self._wv.safe_field("map_p"))

    @property
    def button_areas(self):
        """Array of areas of buttons"""
        return ptr_or_none(self._wv.safe_field("button_areas"))

    @property
    def ctrl_bits(self):
        """Array of control bytes"""
        return ptr_or_none(self._wv.safe_field("ctrl_bits"))

    @property
    def btn_cnt(self):
        """Number of button in 'map_p'(Handled by the library)"""
        return int(self._wv.safe_field("btn_cnt", 0))

    @property
    def row_cnt(self):
        """Number of rows in 'map_p'(Handled by the library)"""
        return int(self._wv.safe_field("row_cnt", 0))

    @property
    def btn_id_sel(self):
        """Index of the active button (being pressed/released etc) or LV_BUTTONMATRIX_BUTTON_NONE"""
        return int(self._wv.safe_field("btn_id_sel", 0))

    @property
    def one_check(self):
        """1: Single button toggled at once"""
        return int(self._wv.safe_field("one_check", 0))

    @property
    def auto_free_map(self):
        """1: Automatically free the map when the widget is deleted"""
        return int(self._wv.safe_field("auto_free_map", 0))

    def snapshot(self, include_children=False, include_styles=False):
        """Snapshot with widget-specific fields in widget_data."""
        s = super().snapshot(include_children=include_children, include_styles=include_styles)
        d = s.get('widget_data') or {}
        d["map_p"] = self.map_p
        d["button_areas"] = self.button_areas
        d["ctrl_bits"] = self.ctrl_bits
        d["btn_cnt"] = self.btn_cnt
        d["row_cnt"] = self.row_cnt
        d["btn_id_sel"] = self.btn_id_sel
        d["one_check"] = self.one_check
        d["auto_free_map"] = self.auto_free_map
        s['widget_data'] = d
        return s
