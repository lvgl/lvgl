import gdb

from lvglgdb.value import Value, ValueInput
from ..misc.lv_ll import LVList


class LVGroup(Value):
    """LVGL focus group wrapper"""

    _DISPLAY_SPEC = {
        "info": [
            ("objects", "obj_count"),
            ("frozen", "frozen"),
            ("editing", "editing"),
            ("wrap", "wrap"),
            ("focused", "focused"),
        ],
        "table": [],
        "empty_msg": "No focus groups.",
    }

    def __init__(self, group: ValueInput):
        super().__init__(Value.normalize(group, "lv_group_t"))

    @property
    def frozen(self) -> bool:
        return bool(int(self.super_value("frozen")))

    @property
    def editing(self) -> bool:
        return bool(int(self.super_value("editing")))

    @property
    def wrap(self) -> bool:
        return bool(int(self.super_value("wrap")))

    @property
    def obj_focus(self) -> Value:
        focus_pp = self.super_value("obj_focus")
        if not focus_pp:
            return None
        return focus_pp.dereference()

    @property
    def focus_cb(self) -> Value:
        return self.super_value("focus_cb")

    @property
    def edge_cb(self) -> Value:
        return self.super_value("edge_cb")

    @property
    def user_data(self) -> Value:
        return self.super_value("user_data")

    @property
    def refocus_policy(self) -> bool:
        return bool(int(self.super_value("refocus_policy")))

    @property
    def obj_count(self) -> int:
        return LVList(self.obj_ll, "lv_obj_t").len

    def _members(self):
        """The objects in the group.

        lv_group_init() sizes obj_ll for `lv_obj_t *`, so each node holds a
        pointer to the object rather than the object itself. Reading the node as
        an lv_obj_t would report the list's own memory as a widget.
        """
        obj_ptr_t = gdb.lookup_type("lv_obj_t").pointer()
        for node in LVList(self.obj_ll):
            # The node's payload is the pointer, so read through it the same way
            # LVList reads its own next/prev links.
            yield Value(int(node)).cast(obj_ptr_t, ptr=True).dereference()

    def __iter__(self):
        from .lv_obj import LVObject

        for obj_ptr in self._members():
            yield LVObject(obj_ptr)

    def snapshot(self):
        from lvglgdb.lvgl.snapshot import Snapshot

        focus = self.obj_focus
        if focus:
            from .lv_obj import LVObject

            focus_obj = LVObject(focus)
            focus_str = f"{focus_obj.class_name}@{int(focus):x}"
            focused_addr = hex(int(focus))
        else:
            focus_str = "(none)"
            focused_addr = None

        member_addrs = []
        for obj_ptr in self._members():
            addr = int(obj_ptr)
            if addr:
                member_addrs.append(hex(addr))

        d = {
            "addr": hex(int(self)),
            "obj_count": self.obj_count,
            "frozen": self.frozen,
            "editing": self.editing,
            "wrap": self.wrap,
            "focused": focus_str,
            "focused_addr": focused_addr,
            "member_addrs": member_addrs,
        }
        return Snapshot(d, source=self, display_spec=self._DISPLAY_SPEC)

    @staticmethod
    def snapshots(groups):
        return [group.snapshot() for group in groups]
