"""
Auto-generated wrapper for lv_ime_pinyin_t.

Do not edit manually. Regenerate from the GDB script root with:
    python3 scripts/generate_all.py
"""

from lvglgdb.lvgl.core.lv_obj import LVObject
from ._helpers import ptr_or_none, safe_string, safe_wrapper


class LVImePinyin(LVObject):
    """LVGL ime widget (lv_ime_pinyin_t)."""

    def __init__(self, obj):
        super().__init__(obj)
        self._wv = self.cast("lv_ime_pinyin_t", ptr=True) or self

    @property
    def kb(self):
        return ptr_or_none(self._wv.safe_field("kb"))

    @property
    def cand_panel(self):
        return ptr_or_none(self._wv.safe_field("cand_panel"))

    @property
    def dict(self):
        return ptr_or_none(self._wv.safe_field("dict"))

    @property
    def k9_legal_py_ll(self):
        return safe_wrapper(self._wv, "k9_legal_py_ll", "lvglgdb.lvgl.misc.lv_ll", "LVList")

    @property
    def cand_str(self):
        """Candidate string"""
        return safe_string(self._wv, "cand_str")

    @property
    def k9_py_ll_pos(self):
        """Current pinyin map pages(k9)"""
        return int(self._wv.safe_field("k9_py_ll_pos", 0))

    @property
    def k9_legal_py_count(self):
        """Count of legal Pinyin numbers(k9)"""
        return int(self._wv.safe_field("k9_legal_py_count", 0))

    @property
    def k9_input_str_len(self):
        """9-key input(k9) mode input string max len"""
        return int(self._wv.safe_field("k9_input_str_len", 0))

    @property
    def ta_count(self):
        """The number of characters entered in the text box this time"""
        return int(self._wv.safe_field("ta_count", 0))

    @property
    def cand_num(self):
        """Number of candidates"""
        return int(self._wv.safe_field("cand_num", 0))

    @property
    def py_page(self):
        """Current pinyin map pages(k26)"""
        return int(self._wv.safe_field("py_page", 0))

    @property
    def mode(self):
        """Set mode, 1: 26-key input(k26), 0: 9-key input(k9). Default: 1."""
        return int(self._wv.safe_field("mode", 0))

    def snapshot(self, include_children=False, include_styles=False):
        """Snapshot with widget-specific fields in widget_data."""
        s = super().snapshot(include_children=include_children, include_styles=include_styles)
        d = s.get('widget_data') or {}
        d["kb"] = self.kb
        d["cand_panel"] = self.cand_panel
        d["dict"] = self.dict
        d["k9_legal_py_ll"] = self.k9_legal_py_ll
        d["cand_str"] = self.cand_str
        d["k9_py_ll_pos"] = self.k9_py_ll_pos
        d["k9_legal_py_count"] = self.k9_legal_py_count
        d["k9_input_str_len"] = self.k9_input_str_len
        d["ta_count"] = self.ta_count
        d["cand_num"] = self.cand_num
        d["py_page"] = self.py_page
        d["mode"] = self.mode
        s['widget_data'] = d
        return s
