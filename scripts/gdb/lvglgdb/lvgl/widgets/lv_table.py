"""
Auto-generated wrapper for lv_table_t.

Do not edit manually. Regenerate from the GDB script root with:
    python3 scripts/generate_all.py
"""

from lvglgdb.lvgl.core.lv_obj import LVObject
from ._helpers import ptr_or_none


class LVTable(LVObject):
    """LVGL table widget (lv_table_t)."""

    def __init__(self, obj):
        super().__init__(obj)
        self._wv = self.cast("lv_table_t", ptr=True) or self

    @property
    def col_cnt(self):
        return int(self._wv.safe_field("col_cnt", 0))

    @property
    def row_cnt(self):
        return int(self._wv.safe_field("row_cnt", 0))

    @property
    def cell_data(self):
        return ptr_or_none(self._wv.safe_field("cell_data"))

    @property
    def row_h(self):
        return ptr_or_none(self._wv.safe_field("row_h"))

    @property
    def col_w(self):
        return ptr_or_none(self._wv.safe_field("col_w"))

    @property
    def col_act(self):
        return int(self._wv.safe_field("col_act", 0))

    @property
    def row_act(self):
        return int(self._wv.safe_field("row_act", 0))

    def snapshot(self, include_children=False, include_styles=False):
        """Snapshot with widget-specific fields in widget_data."""
        s = super().snapshot(include_children=include_children, include_styles=include_styles)
        d = s.get('widget_data') or {}
        d["col_cnt"] = self.col_cnt
        d["row_cnt"] = self.row_cnt
        d["cell_data"] = self.cell_data
        d["row_h"] = self.row_h
        d["col_w"] = self.col_w
        d["col_act"] = self.col_act
        d["row_act"] = self.row_act
        s['widget_data'] = d
        return s
