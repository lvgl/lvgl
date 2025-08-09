import argparse
from typing import Iterator, Union, Optional
from PIL import Image
import numpy as np
from pathlib import Path

import gdb

from .value import Value

gdb.execute("set pagination off")
gdb.write("set pagination off\n")
gdb.execute("set python print-stack full")
gdb.write("set python print-stack full\n")

g_lvgl_instance = None


class LVList(Value):
    """LVGL linked list iterator"""

    def __init__(self, ll: Value, nodetype: Union[gdb.Type, str] = None):
        if not ll:
            raise ValueError("Invalid linked list")
        super().__init__(ll)

        self.nodetype = (
            gdb.lookup_type(nodetype).pointer()
            if isinstance(nodetype, str)
            else nodetype
        )
        self.lv_ll_node_t = gdb.lookup_type("lv_ll_node_t").pointer()
        self.current = self.head
        self._next_offset = self.n_size + self.lv_ll_node_t.sizeof
        self._prev_offset = self.n_size

    def _next(self, node):
        next_value = Value(int(node) + self._next_offset)
        return next_value.cast(self.lv_ll_node_t, ptr=True).dereference()

    def _prev(self, node):
        prev_value = Value(int(node) + self._prev_offset)
        return prev_value.cast(self.lv_ll_node_t, ptr=True).dereference()

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
        node = self.head
        while node:
            len += 1
            node = self._next(node)
        return len


class LVObject(Value):
    """LVGL object"""

    def __init__(self, obj: Value):
        super().__init__(obj.cast("lv_obj_t", ptr=True))

    @property
    def class_name(self):
        name = self.class_p.name
        return name.string() if name else "unknown"

    @property
    def x1(self):
        return int(self.coords.x1)

    @property
    def y1(self):
        return int(self.coords.y1)

    @property
    def x2(self):
        return int(self.coords.x2)

    @property
    def y2(self):
        return int(self.coords.y2)

    @property
    def child_count(self):
        return self.spec_attr.child_cnt if self.spec_attr else 0

    @property
    def children(self):
        if not self.spec_attr:
            return

        for i in range(self.child_count):
            child = self.spec_attr.children[i]
            yield LVObject(child)

    @property
    def styles(self):
        LV_STYLE_PROP_INV = 0
        LV_STYLE_PROP_ANY = 0xFF
        count = self.style_cnt
        if count == 0:
            return

        styles = self.super_value("styles")
        for i in range(count):
            style = styles[i].style
            prop_cnt = style.prop_cnt
            values_and_props = style.values_and_props.cast(
                "lv_style_const_prop_t", ptr=True
            )
            for j in range(prop_cnt):
                prop = values_and_props[j].prop
                if prop == LV_STYLE_PROP_INV or prop == LV_STYLE_PROP_ANY:
                    continue
                yield values_and_props[j]

    def get_child(self, index: int):
        return self.spec_attr.children[index] if self.spec_attr else None


class LVDisplay(Value):
    """LVGL display"""

    def __init__(self, disp: Value):
        super().__init__(disp)

    @property
    def hor_res(self) -> int:
        """Get horizontal resolution in pixels"""
        return int(self.super_value("hor_res"))

    @property
    def ver_res(self) -> int:
        """Get vertical resolution in pixels"""
        return int(self.super_value("ver_res"))

    @property
    def screens(self):
        screens = self.super_value("screens")
        for i in range(self.screen_cnt):
            yield LVObject(screens[i])

    # Buffer-related properties
    @property
    def buf_1(self):
        """Get first draw buffer (may be None)"""
        buf_ptr = self.super_value("buf_1")
        return LVDrawBuf(buf_ptr) if buf_ptr else None

    @property
    def buf_2(self):
        """Get second draw buffer (may be None)"""
        buf_ptr = self.super_value("buf_2")
        return LVDrawBuf(buf_ptr) if buf_ptr else None

    @property
    def buf_act(self):
        """Get currently active draw buffer (may be None)"""
        buf_ptr = self.super_value("buf_act")
        return LVDrawBuf(buf_ptr) if buf_ptr else None


class LVDrawBuf(Value):
    """LVGL draw buffer"""

    _color_formats = {
        "RGB565": None,
        "RGB888": None,
        "ARGB8888": None,
        "XRGB8888": None,
    }

    _bpp = {
        "RGB565": 16,
        "RGB888": 24,
        "ARGB8888": 32,
        "XRGB8888": 32,
    }

    def __init__(self, draw_buf: Value):
        super().__init__(draw_buf)
        self._init_color_formats()

    def _init_color_formats(self):
        """init color formats from enum"""
        for fmt in self._color_formats.keys():
            if self._color_formats[fmt] is None:
                try:
                    self._color_formats[fmt] = int(
                        gdb.parse_and_eval(f"LV_COLOR_FORMAT_{fmt}")
                    )
                except:
                    print(f"Warning: Failed to get LV_COLOR_FORMAT_{fmt}")
                    self._color_formats[fmt] = -1

    def color_format_info(self) -> dict:
        """get color format info"""
        header = self.super_value("header")
        cf = int(header["cf"])

        fmt_name = "UNKNOWN"
        for name, val in self._color_formats.items():
            if val == cf:
                fmt_name = name
                break

        return {"value": cf, "name": fmt_name, "bpp": self._bpp.get(fmt_name, 0)}

    @property
    def header(self) -> dict:
        """Get the image header information as a dictionary"""
        header = self.super_value("header")
        return {
            "w": header["w"],
            "h": header["h"],
            "stride": header["stride"],
            "cf": header["cf"],  # Color format
        }

    @property
    def data_size(self) -> int:
        """Get the total buffer size in bytes"""
        return self.super_value("data_size")

    @property
    def data(self) -> bytes:
        """Get the buffer data ptr"""
        return self.super_value("data")

    def data_dump(self, filename: str, format: str = None) -> bool:
        """
        Dump the buffer data to an image file.

        Args:
            filename: Output file path
            format: Image format (None for auto-detection from filename)

        Returns:
            bool: True if successful, False otherwise
        """
        try:
            # Validate input parameters
            if not filename:
                raise ValueError("Output filename cannot be empty")

            # Get buffer metadata
            header = self.super_value("header")
            width = int(header["w"])
            height = int(header["h"])
            cf_info = self.color_format_info()
            data_ptr = self.super_value("data")
            data_size = int(self.super_value("data_size"))

            # Validate buffer data
            if not data_ptr:
                raise ValueError("Data pointer is NULL")
            if width <= 0 or height <= 0:
                raise ValueError(f"Invalid dimensions: {width}x{height}")
            if data_size <= 0:
                raise ValueError(f"Invalid data size: {data_size}")

            # Read pixel data
            pixel_data = (
                gdb.selected_inferior().read_memory(int(data_ptr), data_size).tobytes()
            )
            if not pixel_data:
                raise ValueError("Failed to read pixel data")

            # Process based on color format
            img = self._convert_to_image(pixel_data, width, height, cf_info["value"])
            if img is None:
                return False

            # Determine output format
            output_format = (
                format.upper() if format else Path(filename).suffix[1:].upper() or "BMP"
            )

            # Save image
            img.save(filename, format=output_format)
            print(
                f"Successfully saved {cf_info['name']} buffer as {output_format} to {filename}"
            )
            return True

        except gdb.MemoryError:
            print("Error: Failed to access memory")
            return False
        except ValueError as e:
            print(f"Validation error: {str(e)}")
            return False
        except Exception as e:
            print(f"Unexpected error: {str(e)}")
            return False

    def _convert_to_image(
        self, pixel_data: bytes, width: int, height: int, color_format: int
    ) -> Optional[Image.Image]:
        """
        Convert raw pixel data to PIL Image based on color format.

        Args:
            pixel_data: Raw pixel bytes
            width: Image width
            height: Image height
            color_format: LVGL color format value

        Returns:
            PIL.Image or None if conversion fails
        """
        try:
            if color_format == self._color_formats["RGB565"]:
                # Convert RGB565 to RGB888
                arr = np.frombuffer(pixel_data, dtype=np.uint8)
                arr = arr.reshape((height, width, 2))
                rgb565 = np.frombuffer(pixel_data, dtype=np.uint16).reshape((height, width))
                r = (((rgb565 & 0xF800) >> 11) << 3).astype(np.uint8)
                g = ((rgb565 & 0x07E0) >> 3).astype(np.uint8)
                b = ((rgb565 & 0x001F) << 3).astype(np.uint8)
                return Image.fromarray(np.dstack((r, g, b)), "RGB")

            elif color_format == self._color_formats["RGB888"]:
                arr = np.frombuffer(pixel_data, dtype=np.uint8).reshape(height, width, 3)
                rgb_arr = arr[:, :, [2, 1, 0]]  # BGR -> RGB
                return Image.fromarray(rgb_arr, "RGB")

            elif color_format == self._color_formats["ARGB8888"]:
                arr = np.frombuffer(pixel_data, dtype=np.uint8).reshape(-1, 4)
                rgba = np.column_stack((arr[:, 1:4], arr[:, 0]))  # ARGB -> RGBA
                return Image.frombytes("RGBA", (width, height), rgba.tobytes())

            elif color_format == self._color_formats["XRGB8888"]:
                arr = np.frombuffer(pixel_data, dtype=np.uint8).reshape(-1, 4)
                rgb = arr[:, [2, 1, 0]]  # BGR -> RGB
                return Image.fromarray(rgb.reshape(height, width, 3), "RGB")

            raise ValueError(f"Unsupported color format: 0x{color_format:x}")

        except Exception as e:
            print(f"Conversion error: {str(e)}")
            return None


class LVGL:
    """LVGL instance"""

    def __init__(self, lv_global: Value):
        self.lv_global = lv_global.cast("lv_global_t", ptr=True)

    def displays(self) -> Iterator[LVDisplay]:
        ll = self.lv_global.disp_ll
        if not ll:
            return

        for disp in LVList(ll, "lv_display_t"):
            yield LVDisplay(disp)

    def disp_default(self):
        disp_default = self.lv_global.disp_default
        return LVDisplay(disp_default) if disp_default else None

    def screen_active(self):
        disp = self.lv_global.disp_default
        return disp.act_scr if disp else None

    def draw_units(self):
        unit = self.lv_global.draw_info.unit_head

        # Iterate through all draw units
        while unit:
            yield unit
            unit = unit.next


def set_lvgl_instance(lv_global: Union[gdb.Value, Value, None]):
    global g_lvgl_instance

    if not lv_global:
        try:
            lv_global = Value(gdb.parse_and_eval("lv_global").address)
        except gdb.error as e:
            print(f"Failed to get lv_global: {e}")
            return

    if not isinstance(lv_global, Value):
        lv_global = Value(lv_global)

    inited = lv_global.inited
    if not inited:
        print(
            "\x1b[31mlvgl is not initialized yet. Please call `set_lvgl_instance(None)` later.\x1b[0m"
        )
        return

    g_lvgl_instance = LVGL(lv_global)


def dump_obj_info(obj: LVObject):
    clzname = obj.class_name
    coords = f"{obj.x1},{obj.y1},{obj.x2},{obj.y2}"
    print(f"{clzname}@{hex(obj)} {coords}")


#  Dump lv_style_const_prop_t
def dump_style_info(style: Value):
    prop = int(style.prop)
    value = style.value
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
        for child in obj.children:
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


class DumpDisplayBuf(gdb.Command):
    """dump display buf to image"""

    def __init__(self):
        super(DumpDisplayBuf, self).__init__(
            "dump display", gdb.COMMAND_USER, gdb.COMPLETE_EXPRESSION
        )

    def invoke(self, args, from_tty):
        parser = argparse.ArgumentParser(description="Dump display draw buffer.")
        parser.add_argument(
            "-p",
            "--prefix",
            type=str,
            default="",
            help="prefix of dump file path",
        )
        parser.add_argument(
            "-f",
            "--format",
            type=str,
            choices=["bmp", "png"],
            default=None,
            help="dump file format (bmp or png)",
        )

        try:
            args = parser.parse_args(gdb.string_to_argv(args))
        except SystemExit:
            return

        display = g_lvgl_instance.disp_default()
        if not display:
            print("Error: Invalid display pointer")
            return
        buffers = {
            "buf_1": display.buf_1,
            "buf_2": display.buf_2,
        }

        for buf_name, buf_ptr in buffers.items():
            if buf_ptr is not None:
                draw_buf = LVDrawBuf(buf_ptr)
                filename = f"{args.prefix}{buf_name}.{args.format.lower() if args.format else 'bmp'}"
                draw_buf.data_dump(filename, args.format)
            else:
                print(f"Warning: {buf_name} buffer is None, skipping.")


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

        obj = Value(obj)

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
        for unit in g_lvgl_instance.draw_units():
            self.dump_draw_unit(unit)
