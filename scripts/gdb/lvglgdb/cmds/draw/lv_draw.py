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
        try:
            target_type = gdb.lookup_type(type_name)
        except gdb.error:
            target_type = gdb.lookup_type("lv_draw_unit_t")

        casted = unit.cast(target_type, ptr=True)
        if casted is None:
            casted = unit.cast("lv_draw_unit_t", ptr=True)
        print(casted.dereference().format_string(pretty_structs=True, symbols=True))
