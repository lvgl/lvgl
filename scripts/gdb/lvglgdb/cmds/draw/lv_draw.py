import gdb

from lvglgdb.lvgl import curr_inst
from lvglgdb.lvgl.draw.lv_draw_unit import LVDrawUnit
from lvglgdb.lvgl.draw.lv_draw_consts import DRAW_UNIT_TYPE_NAMES


class InfoDrawUnit(gdb.Command):
    """dump draw unit info"""

    def __init__(self):
        super(InfoDrawUnit, self).__init__(
            "info draw_unit", gdb.COMMAND_USER, gdb.COMPLETE_EXPRESSION
        )

    def invoke(self, args, from_tty):
        for unit in curr_inst().draw_units():
            self._dump_unit(unit)

    def _dump_unit(self, unit: LVDrawUnit):
        name = unit.name
        print(f"Draw Unit: {unit}, Name: {name}")

        type_name = DRAW_UNIT_TYPE_NAMES.get(name, "lv_draw_unit_t")
        casted = unit.cast(type_name, ptr=True)
        if casted is None or not casted.is_ok:
            casted = unit.cast("lv_draw_unit_t", ptr=True)
            if casted is None or not casted.is_ok:
                print(f"  (corrupted: {casted})")
                return
        deref = casted.dereference()
        if not deref.is_ok:
            print(f"  (corrupted: {deref})")
            return
        print(deref.format_string(pretty_structs=True, symbols=True))
