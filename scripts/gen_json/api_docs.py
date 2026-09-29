"""Project lvgl_api metadata onto the lookups used by the JSON serializer."""

from pathlib import Path
from types import SimpleNamespace

from lvgl_api import PublicApi


def gen_json_doxygen_predefined(lv_conf_path):
    conf = Path(lv_conf_path).resolve()
    return (
        "DOXYGEN",
        f'LV_CONF_PATH="{conf}"',
        # lv_conf_internal.h probes this setting, undefines it, then declares
        # its final generated value. Seed that probe from the active Kconfig
        # symbol so Doxygen records the final definition only.
        "LV_LIBINPUT_XKB=CONFIG_LV_LIBINPUT_XKB",
        "LV_ATTRIBUTE_TICK_INC=",
        "LV_ATTRIBUTE_TIMER_HANDLER=",
        "LV_ATTRIBUTE_FLUSH_READY=",
        "LV_ATTRIBUTE_MEM_ALIGN=",
        "LV_ATTRIBUTE_LARGE_CONST=",
        "LV_ATTRIBUTE_LARGE_RAM_ARRAY=",
        "LV_ATTRIBUTE_FAST_MEM=",
        "LV_ATTRIBUTE_EXTERN_DATA=",
        "LV_FORMAT_ATTRIBUTE(fmt,va)=",
        "FASTGLTF_EXPORT=",
    )


def parse_gen_json_api(repo_root, lv_conf_path):
    root = Path(repo_root)
    return PublicApi.parse(
        root,
        lv_conf_path=lv_conf_path,
        doxygen_predefined=gen_json_doxygen_predefined(lv_conf_path),
        doxygen_include_path=(root / "include",),
        inherit_doxygen_aliases=True,
    )


def _description(item):
    return SimpleNamespace(description=item.doc) if item is not None else None


class ApiDocs:
    def __init__(self, api):
        self.api = api
        self._macros_by_name = {}
        for macro in api.public_macros():
            self._macros_by_name.setdefault(macro.name, []).append(macro)

    def get_enum_item(self, name):
        return _description(self.api.enum_members.get(name))

    def get_enum(self, name):
        return _description(self.api.enums.get(name))

    def get_function(self, name):
        item = self.api.functions.get(name)
        if item is None:
            return None
        return SimpleNamespace(
            description=item.doc,
            res_description=item.return_doc,
            args=[
                SimpleNamespace(name=p.name, description=p.detailed_doc)
                for p in item.declaration[1]
            ],
        )

    def get_variable(self, name):
        return _description(self.api.variables.get(name))

    def _structure(self, name, kind):
        item = self.api.structs.get(name)
        if item is None or item.kind != kind:
            return None
        return SimpleNamespace(
            description=item.doc,
            fields=[SimpleNamespace(name=f.name, description=f.doc) for f in item.fields],
        )

    def get_structure(self, name):
        return self._structure(name, "struct")

    def get_union(self, name):
        return self._structure(name, "union")

    def get_typedef(self, name):
        return _description(self.api.typedefs.get(name))

    def get_macro(self, name):
        definitions = self._macros_by_name.get(name)
        if not definitions:
            return None
        if len(definitions) != 1:
            locations = ", ".join(
                f"{macro.source_file or '<unknown>'}:"
                f"{macro.source_line if macro.source_line is not None else '?'}"
                for macro in definitions
            )
            raise RuntimeError(f"duplicate macro definition for {name}: {locations}")
        (macro,) = definitions
        return SimpleNamespace(
            name=macro.name, description=macro.doc or "",
            # Preserve the historical null representation for zero arguments.
            params=macro.params or None, initializer=macro.initializer,
        )

    def get_macros(self):
        return [self.get_macro(name) for name in self._macros_by_name]
