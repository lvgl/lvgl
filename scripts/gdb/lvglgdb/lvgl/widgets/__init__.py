"""
Auto-generated LVGL widget wrappers.

Do not edit manually. Regenerate from the GDB script root with:
    python3 scripts/generate_all.py
"""

from .lv_3dtexture import LV3dtexture
from .lv_image import LVImage
from .lv_animimg import LVAnimimg
from .lv_arc import LVArc
from .lv_arclabel import LVArclabel
from .lv_bar import LVBar
from .lv_button import LVButton
from .lv_buttonmatrix import LVButtonmatrix
from .lv_calendar import LVCalendar
from .lv_canvas import LVCanvas
from .lv_chart import LVChart
from .lv_checkbox import LVCheckbox
from .lv_dropdown import LVDropdown
from .lv_dropdown_list import LVDropdownList
from .lv_imagebutton import LVImagebutton
from .lv_ime_pinyin import LVImePinyin
from .lv_keyboard import LVKeyboard
from .lv_label import LVLabel
from .lv_led import LVLed
from .lv_line import LVLine
from .lv_menu import LVMenu
from .lv_menu_page import LVMenuPage
from .lv_msgbox import LVMsgbox
from .lv_roller import LVRoller
from .lv_scale import LVScale
from .lv_slider import LVSlider
from .lv_spangroup import LVSpangroup
from .lv_textarea import LVTextarea
from .lv_spinbox import LVSpinbox
from .lv_spinner import LVSpinner
from .lv_switch import LVSwitch
from .lv_table import LVTable
from .lv_tabview import LVTabview
from .lv_tileview import LVTileview
from .lv_tileview_tile import LVTileviewTile
from .lv_win import LVWin

WIDGET_REGISTRY: dict[str, type] = {
    "lv_3dtexture": LV3dtexture,
    "lv_image": LVImage,
    "lv_animimg": LVAnimimg,
    "lv_arc": LVArc,
    "lv_arclabel": LVArclabel,
    "lv_bar": LVBar,
    "lv_button": LVButton,
    "lv_buttonmatrix": LVButtonmatrix,
    "lv_calendar": LVCalendar,
    "lv_canvas": LVCanvas,
    "lv_chart": LVChart,
    "lv_checkbox": LVCheckbox,
    "lv_dropdown": LVDropdown,
    "lv_dropdown_list": LVDropdownList,
    "lv_imagebutton": LVImagebutton,
    "lv_ime_pinyin": LVImePinyin,
    "lv_keyboard": LVKeyboard,
    "lv_label": LVLabel,
    "lv_led": LVLed,
    "lv_line": LVLine,
    "lv_menu": LVMenu,
    "lv_menu_page": LVMenuPage,
    "lv_msgbox": LVMsgbox,
    "lv_roller": LVRoller,
    "lv_scale": LVScale,
    "lv_slider": LVSlider,
    "lv_spangroup": LVSpangroup,
    "lv_textarea": LVTextarea,
    "lv_spinbox": LVSpinbox,
    "lv_spinner": LVSpinner,
    "lv_switch": LVSwitch,
    "lv_table": LVTable,
    "lv_tabview": LVTabview,
    "lv_tileview": LVTileview,
    "lv_tileview_tile": LVTileviewTile,
    "lv_win": LVWin,
}


import gdb


def wrap_widget(obj):
    """Wrap an LVObject into its widget class if known."""
    name = obj.class_name
    # Before 6d999331d (Feb 2025), LVGL class names had no 'lv_' prefix
    # (e.g. 'label' instead of 'lv_label'). Try both for compatibility.
    cls = WIDGET_REGISTRY.get(name) or WIDGET_REGISTRY.get('lv_' + name)
    if cls:
        try:
            return cls(obj)
        except gdb.error:
            pass  # type not available in debug info
    return None

__all__ = [
    "LV3dtexture",
    "LVImage",
    "LVAnimimg",
    "LVArc",
    "LVArclabel",
    "LVBar",
    "LVButton",
    "LVButtonmatrix",
    "LVCalendar",
    "LVCanvas",
    "LVChart",
    "LVCheckbox",
    "LVDropdown",
    "LVDropdownList",
    "LVImagebutton",
    "LVImePinyin",
    "LVKeyboard",
    "LVLabel",
    "LVLed",
    "LVLine",
    "LVMenu",
    "LVMenuPage",
    "LVMsgbox",
    "LVRoller",
    "LVScale",
    "LVSlider",
    "LVSpangroup",
    "LVTextarea",
    "LVSpinbox",
    "LVSpinner",
    "LVSwitch",
    "LVTable",
    "LVTabview",
    "LVTileview",
    "LVTileviewTile",
    "LVWin",
    "WIDGET_REGISTRY",
    "wrap_widget",
]
