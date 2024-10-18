# Usage: source lvgl.py
import argparse
from typing import Iterator, Union

import gdb

gdb.execute("set pagination off")
gdb.write("set pagination off\n")
gdb.execute("set python print-stack full")
gdb.write("set python print-stack full\n")

g_lvgl_instance = None


class LVList:
    """LVGL linked list iterator"""

    def __init__(self, ll: gdb.Value, nodetype: Union[gdb.Type, str] = None):
        if not ll:
            raise ValueError("Invalid linked list")

        self.ll = ll
        self.nodetype = (
            gdb.lookup_type(nodetype).pointer()
            if isinstance(nodetype, str)
            else nodetype
        )
        self.lv_ll_node_t = gdb.lookup_type("lv_ll_node_t").pointer()
        self.current = ll["head"]
        self._next_offset = ll["n_size"] + self.lv_ll_node_t.sizeof
        self._prev_offset = ll["n_size"]

    def _next(self, node):
        next = gdb.Value(int(node) + self._next_offset)
        return next.cast(self.lv_ll_node_t.pointer()).dereference()

    def _prev(self, node):
        prev = gdb.Value(int(node) + self._prev_offset)
        return prev.cast(self.lv_ll_node_t)

    def __iter__(self):
        return self

    def __next__(self):
        if not self.current:
            raise StopIteration

        nodetype = self.nodetype if self.nodetype else self.lv_ll_node_t
        node = self.current.cast(nodetype)

        self.current = self._next(self.current)
        return node

    @property
    def len(self):
        len = 0
        node = self.ll["head"]
        while node:
            len += 1
            node = self._next(node)
        return len

    @property
    def head(self):
        return self.ll["head"]

    @property
    def tail(self):
        return self.ll["tail"]

    @property
    def size(self):
        return self.ll["n_size"]


class LVObject(gdb.Value):
    """LVGL object"""

    def __init__(self, obj: gdb.Value):
        super().__init__(obj)
        self.obj = obj

    @property
    def class_p(self):
        return self.obj["class_p"]

    @property
    def class_name(self):
        name = self.class_p["name"]
        return name.string() if name else "unknown"

    @property
    def coords(self):
        return self.obj["coords"]

    @property
    def x1(self):
        return int(self.coords["x1"])

    @property
    def y1(self):
        return int(self.coords["y1"])

    @property
    def x2(self):
        return int(self.coords["x2"])

    @property
    def y2(self):
        return int(self.coords["y2"])

    @property
    def child_count(self):
        return self.obj["spec_attr"]["child_cnt"] if self.obj["spec_attr"] else 0

    @property
    def childs(self):
        if not self.obj["spec_attr"]:
            return

        for i in range(self.child_count):
            child = self.obj["spec_attr"]["children"][i]
            yield LVObject(child)

    @property
    def styles(self):
        LV_STYLE_PROP_INV = 0
        LV_STYLE_PROP_ANY = 0xFF
        count = self.obj["style_cnt"]
        if count == 0:
            return

        styles = self.obj["styles"]
        for i in range(count):
            style = styles[i]["style"]
            prop_cnt = style["prop_cnt"]
            values_and_props = style["values_and_props"].cast(
                gdb.lookup_type("lv_style_const_prop_t").pointer()
            )
            for j in range(prop_cnt):
                prop = values_and_props[j]["prop"]
                if prop == LV_STYLE_PROP_INV or prop == LV_STYLE_PROP_ANY:
                    continue
                yield values_and_props[j]

    def get_child(self, index: int):
        return (
            self.obj["spec_attr"]["children"][index] if self.obj["spec_attr"] else None
        )


class LVDisplay(gdb.Value):
    """LVGL display"""

    def __init__(self, disp: gdb.Value):
        super().__init__(disp)
        self.disp = disp

    @property
    def screens(self):
        for i in range(self.screen_cnt):
            yield LVObject(self.disp["screens"][i])

    @property
    def screen_cnt(self):
        return self.disp["screen_cnt"]

    @property
    def act_scr(self):
        return self.disp["act_scr"]


class LVGL:
    """LVGL instance"""

    def __init__(self, lv_global: gdb.Value):
        self.lv_global = lv_global.cast(gdb.lookup_type("lv_global_t").pointer())

    def displays(self) -> Iterator[LVDisplay]:
        ll = self.lv_global["disp_ll"]
        if not ll:
            return

        for disp in LVList(ll, "lv_display_t"):
            yield LVDisplay(disp)

    def screen_active(self):
        disp = self.lv_global["disp_default"]
        return disp["act_scr"] if disp else None

    def draw_units(self):
        unit = self.lv_global["draw_info"]["unit_head"]

        # Iterate through all draw units
        while unit: 
            yield unit
            unit = unit["next"]

def set_lvgl_instance(lv_global: gdb.Value):
    global g_lvgl_instance

    if not lv_global:
        try:
            lv_global = gdb.parse_and_eval("lv_global").address
        except gdb.error as e:
            print(f"Failed to get lv_global: {e}")
            return

    g_lvgl_instance = LVGL(lv_global)


def dump_obj_info(obj: LVObject):
    clzname = obj.class_name
    coords = f"{obj.x1},{obj.y1},{obj.x2},{obj.y2}"
    print(f"{clzname}@{hex(obj)} {coords}")


#  Dump lv_style_const_prop_t
def dump_style_info(style: gdb.Value):
    prop = int(style["prop"])
    value = style["value"]
    print(f"{prop} = {value}")


class DumpObj(gdb.Command):
    """dump obj tree from specified obj"""

    def __init__(self):
        super(DumpObj, self).__init__(
            "dump obj", gdb.COMMAND_USER, gdb.COMPLETE_EXPRESSION
        )

    def dump_obj(self, obj: LVObject, depth=0, limit=None):
        if not obj:
            return

        # dump self
        print("  " * depth, end="")
        dump_obj_info(obj)

        if limit is not None and depth >= limit:
            return

        # dump children
        for child in obj.childs:
            self.dump_obj(child, depth + 1, limit=limit)

    def invoke(self, args, from_tty):
        parser = argparse.ArgumentParser(description="Dump lvgl obj tree.")
        parser.add_argument(
            "-L",
            "--level",
            type=int,
            default=None,
            help="Limit the depth of the tree.",
        )
        parser.add_argument(
            "root",
            type=str,
            nargs="?",
            default=None,
            help="Optional root obj to dump.",
        )
        try:
            args = parser.parse_args(gdb.string_to_argv(args))
        except SystemExit:
            return

        if args.root:
            root = gdb.parse_and_eval(args.root)
            root = LVObject(root)
            self.dump_obj(root, limit=args.level)
        else:
            # dump all displays
            depth = 0
            for disp in g_lvgl_instance.displays():
                print(f"Display {hex(disp)}")
                for screen in disp.screens:
                    print(f'{"  " * (depth + 1)}Screen@{hex(screen)}')
                    self.dump_obj(screen, depth=depth + 1, limit=args.level)


class InfoStyle(gdb.Command):
    """dump obj style value for specified obj"""

    def __init__(self):
        super(InfoStyle, self).__init__(
            "info style", gdb.COMMAND_USER, gdb.COMPLETE_EXPRESSION
        )

    def invoke(self, args, from_tty):
        parser = argparse.ArgumentParser(description="Dump lvgl obj local style.")
        parser.add_argument(
            "obj",
            type=str,
            help="obj to show style.",
        )

        try:
            args = parser.parse_args(gdb.string_to_argv(args))
        except SystemExit:
            return

        obj = gdb.parse_and_eval(args.obj)
        if not obj:
            print("Invalid obj: ", args.obj)
            return

        obj = obj.cast(gdb.lookup_type("lv_obj_t").pointer())

        # show all styles applied to this obj
        for style in LVObject(obj).styles:
            print("  ", end="")
            dump_style_info(style)

class InfoDrawUnit(gdb.Command):
    """dump draw unit info"""

    def __init__(self):
        super(InfoDrawUnit, self).__init__(
            "info draw_unit", gdb.COMMAND_USER, gdb.COMPLETE_EXPRESSION
        )

    def dump_draw_unit(self, draw_unit:gdb.Value):
        # Dereference to get the string content of the name from draw_unit
        name = draw_unit["name"].string()

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

        type = types.get(name) or lookup_type("lv_draw_unit_t")
        print(draw_unit.cast(type.pointer()).dereference().format_string(pretty_structs=True, symbols=True))

    def invoke(self, args, from_tty):
        for unit in g_lvgl_instance.draw_units():
            self.dump_draw_unit(unit)

DumpObj()
InfoStyle()
InfoDrawUnit()
set_lvgl_instance(None)
