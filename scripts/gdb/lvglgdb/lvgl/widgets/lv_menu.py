"""
Auto-generated wrapper for lv_menu_t.

Do not edit manually. Regenerate from the GDB script root with:
    python3 scripts/generate_all.py
"""

from lvglgdb.lvgl.core.lv_obj import LVObject
from ._helpers import ptr_or_none, safe_wrapper


class LVMenu(LVObject):
    """LVGL menu widget (lv_menu_t)."""

    def __init__(self, obj):
        super().__init__(obj)
        self._wv = self.cast("lv_menu_t", ptr=True) or self

    @property
    def storage(self):
        """a pointer to obj that is the parent of all pages not displayed"""
        return ptr_or_none(self._wv.safe_field("storage"))

    @property
    def main(self):
        return ptr_or_none(self._wv.safe_field("main"))

    @property
    def main_page(self):
        return ptr_or_none(self._wv.safe_field("main_page"))

    @property
    def main_header(self):
        return ptr_or_none(self._wv.safe_field("main_header"))

    @property
    def main_header_title(self):
        return ptr_or_none(self._wv.safe_field("main_header_title"))

    @property
    def sidebar(self):
        return ptr_or_none(self._wv.safe_field("sidebar"))

    @property
    def sidebar_page(self):
        return ptr_or_none(self._wv.safe_field("sidebar_page"))

    @property
    def sidebar_header(self):
        return ptr_or_none(self._wv.safe_field("sidebar_header"))

    @property
    def sidebar_header_title(self):
        return ptr_or_none(self._wv.safe_field("sidebar_header_title"))

    @property
    def selected_tab(self):
        return ptr_or_none(self._wv.safe_field("selected_tab"))

    @property
    def history_ll(self):
        return safe_wrapper(self._wv, "history_ll", "lvglgdb.lvgl.misc.lv_ll", "LVList")

    @property
    def cur_depth(self):
        return int(self._wv.safe_field("cur_depth", 0))

    @property
    def prev_depth(self):
        return int(self._wv.safe_field("prev_depth", 0))

    @property
    def sidebar_generated(self):
        return int(self._wv.safe_field("sidebar_generated", 0))

    @property
    def mode_header(self):
        return int(self._wv.safe_field("mode_header", 0))

    @property
    def mode_root_back_btn(self):
        return int(self._wv.safe_field("mode_root_back_btn", 0))

    def snapshot(self, include_children=False, include_styles=False):
        """Snapshot with widget-specific fields in widget_data."""
        s = super().snapshot(include_children=include_children, include_styles=include_styles)
        d = s.get('widget_data') or {}
        d["storage"] = self.storage
        d["main"] = self.main
        d["main_page"] = self.main_page
        d["main_header"] = self.main_header
        d["main_header_title"] = self.main_header_title
        d["sidebar"] = self.sidebar
        d["sidebar_page"] = self.sidebar_page
        d["sidebar_header"] = self.sidebar_header
        d["sidebar_header_title"] = self.sidebar_header_title
        d["selected_tab"] = self.selected_tab
        d["history_ll"] = self.history_ll
        d["cur_depth"] = self.cur_depth
        d["prev_depth"] = self.prev_depth
        d["sidebar_generated"] = self.sidebar_generated
        d["mode_header"] = self.mode_header
        d["mode_root_back_btn"] = self.mode_root_back_btn
        s['widget_data'] = d
        return s
