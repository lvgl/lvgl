import gdb

from lvglgdb.value import Value
from lvglgdb.lvgl import curr_inst


class InfoDrawUnit(gdb.Command):
    """dump draw unit info"""

    def __init__(self):
        super(InfoDrawUnit, self).__init__(
            "info draw_unit", gdb.COMMAND_USER, gdb.COMPLETE_EXPRESSION
        )

    def dump_draw_unit(self, draw_unit: Value):
        # Dereference to get the string content of the name from draw_unit
        name = draw_unit.name.string()

        # Print draw_unit information and the name
        print(f"Draw Unit: {draw_unit}, Name: {name}")

        # Handle different draw_units based on the name
        def lookup_type(name):
            try:
                return gdb.lookup_type(name)
            except gdb.error:
                return None

        types = {
            "DMA2D": lookup_type("lv_draw_dma2d_unit_t"),
            "NEMA_GFX": lookup_type("lv_draw_nema_gfx_unit_t"),
            "NXP_PXP": lookup_type("lv_draw_pxp_unit_t"),
            "NXP_VGLITE": lookup_type("lv_draw_vglite_unit_t"),
            "OPENGLES": lookup_type("lv_draw_opengles_unit_t"),
            "DAVE2D": lookup_type("lv_draw_dave2d_unit_t"),
            "SDL": lookup_type("lv_draw_sdl_unit_t"),
            "SW": lookup_type("lv_draw_sw_unit_t"),
            "VG_LITE": lookup_type("lv_draw_vg_lite_unit_t"),
        }

        type = types.get(name, lookup_type("lv_draw_unit_t"))
        print(
            draw_unit.cast(type, ptr=True)
            .dereference()
            .format_string(pretty_structs=True, symbols=True)
        )

    def invoke(self, args, from_tty):
        for unit in curr_inst().draw_units():
            self.dump_draw_unit(unit)
