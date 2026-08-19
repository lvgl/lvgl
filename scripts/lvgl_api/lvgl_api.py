"""
Model of LVGL's public API, built from Doxygen's XML output.

From this file you can query these per function:

1. The return type
2. The parameter list
3. The Doxygen comment
4. The header the function is declared in
5. The file and line range where the body of the function resides

    from lvgl_api import PublicApi

    api = PublicApi.parse(repo_root)
    for func in api.public_functions():
        for param in func.params:
            if param.is_pointer_like:
                ...
        body = func.body(repo_root)

Requires the `doxygen` executable.
"""

from __future__ import annotations

import os
import re
import shutil
import subprocess
import tempfile
import xml.etree.ElementTree as ET
from dataclasses import dataclass, field
from pathlib import Path

PUBLIC_INCLUDE_DIR = "include/lvgl"

DOXYFILE_TEMPLATE = """\
@INCLUDE = {docs_doxyfile}

PROJECT_NAME         = lvgl-api
OUTPUT_DIRECTORY     = {out_dir}
STRIP_FROM_PATH      = {repo_root}
INPUT                = {inputs}
RECURSIVE            = YES
FILE_PATTERNS        = lv*.h lv*.c lv*.cpp lv*.hpp
EXTRACT_ALL          = YES
EXTRACT_STATIC       = YES
GENERATE_HTML        = NO
GENERATE_LATEX       = NO
GENERATE_XML         = YES
XML_PROGRAMLISTING   = NO
SOURCE_BROWSER       = NO
ALIASES              =
QUIET                = YES
WARNINGS             = NO
WARN_IF_UNDOCUMENTED = NO
WARN_IF_DOC_ERROR    = NO
WARN_AS_ERROR        = NO
WARN_LOGFILE         = {out_dir}/doxygen_warnings.txt
"""


def _text(el) -> str:
    """Flattened text of an element, including nested <ref> nodes."""
    if el is None:
        return ""
    return " ".join("".join(el.itertext()).split())


def _split_args(argsstring: str) -> list[str] | None:
    """Split Doxygen's normalised `(a, b, c)` into its parameters."""
    s = (argsstring or "").strip()
    if not s.startswith("("):
        return None
    end = s.rfind(")")
    if end == -1:
        return None
    parts, cur, depth = [], "", 0
    for ch in s[1:end]:
        if ch in "([{":
            depth += 1
        elif ch in ")]}":
            depth -= 1
        if ch == "," and depth == 0:
            parts.append(cur)
            cur = ""
        else:
            cur += ch
    parts.append(cur)
    return parts


def _name_of_arg(arg: str) -> str | None:
    """The identifier a Doxygen-normalised parameter declares, if any."""
    a = arg.strip()
    if not a or a in ("void", "..."):
        return None
    fp = re.search(r"\(\s*\*+\s*(\w+)\s*\)", a)  # function pointer
    if fp:
        return fp.group(1)
    a = re.sub(r"\[[^\]]*\]\s*$", "", a).strip()  # trailing []
    m = re.search(r"(\w+)\s*$", a)
    return m.group(1) if m else None


@dataclass
class Param:
    name: str | None
    type: str
    doc: str = ""

    @property
    def is_varargs(self) -> bool:
        return self.type.strip() == "..."

    @property
    def is_array(self) -> bool:
        return bool(re.search(r"\[[^\]]*\]\s*$", self.type))

    @property
    def is_function_pointer(self) -> bool:
        return bool(re.search(r"\(\s*\*+\s*\w*\s*\)\s*\(", self.type))

    @property
    def is_pointer(self) -> bool:
        return "*" in self.type and not self.is_function_pointer

    @property
    def is_pointer_like(self) -> bool:
        """Passed as an address the callee may dereference."""
        if self.is_varargs:
            return False
        return self.is_pointer or self.is_array or self.is_function_pointer


@dataclass
class Function:
    name: str
    return_type: str
    params: list[Param]
    header: str | None  # repo-relative, None when not declared in a header
    header_line: int | None
    body_file: str | None  # repo-relative
    body_start: int | None
    body_end: int | None
    brief: str = ""
    detail: str = ""
    from_definition: bool = False
    decl_params: list[Param] = field(default_factory=list)
    decl_return_type: str = ""

    @property
    def declaration(self) -> tuple[str, list[Param]]:
        """The public signature: (return type, parameters) as declared."""
        return (
            self.decl_return_type or self.return_type,
            self.decl_params or self.params,
        )

    @property
    def is_public(self) -> bool:
        return bool(self.header and self.header.startswith(PUBLIC_INCLUDE_DIR))

    @property
    def has_body(self) -> bool:
        return bool(self.body_file and self.body_start and self.body_end)

    def body(self, repo_root: Path) -> str:
        """The implementation, sliced out of the source by line range."""
        if not self.has_body:
            return ""
        path = repo_root / self.body_file
        try:
            lines = path.read_text(errors="ignore").splitlines()
        except OSError:
            return ""
        return "\n".join(lines[self.body_start - 1 : self.body_end])

    def param(self, name: str) -> Param | None:
        for p in self.params:
            if p.name == name:
                return p
        return None


@dataclass
class EnumMember:
    name: str
    value: str = ""


@dataclass
class Enum:
    name: str
    members: list[EnumMember]
    header: str | None

    @property
    def is_public(self) -> bool:
        return bool(self.header and self.header.startswith(PUBLIC_INCLUDE_DIR))


@dataclass
class Field:
    name: str
    type: str


@dataclass
class Struct:
    name: str
    kind: str  # "struct" or "union"
    fields: list[Field]
    header: str | None

    @property
    def is_public(self) -> bool:
        return bool(self.header and self.header.startswith(PUBLIC_INCLUDE_DIR))


@dataclass
class Typedef:
    name: str
    type: str
    header: str | None

    @property
    def is_public(self) -> bool:
        return bool(self.header and self.header.startswith(PUBLIC_INCLUDE_DIR))


@dataclass
class PublicApi:
    functions: dict[str, Function] = field(default_factory=dict)
    enums: dict[str, Enum] = field(default_factory=dict)
    structs: dict[str, Struct] = field(default_factory=dict)
    typedefs: dict[str, Typedef] = field(default_factory=dict)

    def public_functions(self) -> list[Function]:
        return [f for f in self.functions.values() if f.is_public]

    def public_enums(self) -> list[Enum]:
        return [e for e in self.enums.values() if e.is_public]

    def public_structs(self) -> list[Struct]:
        return [s for s in self.structs.values() if s.is_public]

    def public_typedefs(self) -> list[Typedef]:
        return [t for t in self.typedefs.values() if t.is_public]

    # -- construction ----------------------------------------------------

    @classmethod
    def parse(
        cls,
        repo_root: Path,
        xml_dir: Path | None = None,
        inputs: tuple[str, ...] = (PUBLIC_INCLUDE_DIR, "src"),
    ) -> "PublicApi":
        """Build the model, running Doxygen unless `xml_dir` is supplied."""
        if xml_dir is not None:
            return cls._from_xml(Path(xml_dir), repo_root)
        with tempfile.TemporaryDirectory(prefix="lvgl-api-") as tmp:
            out_dir = Path(tmp)
            root = Path(repo_root)
            run_doxygen(root, out_dir, inputs)
            return cls._from_xml(out_dir / "xml", root)

    @classmethod
    def _from_xml(cls, xml_dir: Path, repo_root: Path) -> "PublicApi":
        api = cls()
        root = repo_root.resolve()
        if not xml_dir.is_dir():
            raise RuntimeError(f"not a Doxygen XML directory: {xml_dir}")

        paths = [
            path
            for path in sorted(xml_dir.glob("*.xml"))
            if path.name not in ("index.xml", "Doxyfile.xml")
        ]
        if not paths:
            raise RuntimeError(f"no Doxygen XML files in {xml_dir}")

        for path in paths:
            try:
                tree = ET.parse(path)
            except ET.ParseError as e:
                raise RuntimeError(f"{path}: malformed Doxygen XML: {e}") from e
            for member in tree.iter("memberdef"):
                kind = member.get("kind")
                if kind == "function":
                    func = _function_from_memberdef(member, root)
                    if func is not None:
                        api.functions[func.name] = _merge(
                            api.functions.get(func.name), func
                        )
                elif kind == "enum":
                    en = _enum_from_memberdef(member, root)
                    if en is not None:
                        api.enums.setdefault(en.name, en)
                elif kind == "typedef":
                    td = _typedef_from_memberdef(member, root)
                    if td is not None:
                        api.typedefs.setdefault(td.name, td)

            for compound in tree.iter("compounddef"):
                st = _struct_from_compounddef(compound, root)
                if st is not None:
                    api.structs.setdefault(st.name, st)
        return api


def _relative(path: str | None, root: Path) -> str | None:
    """Normalise a Doxygen path to one relative to `root`.

    STRIP_FROM_PATH makes Doxygen emit paths already relative to the tree, so
    they must be resolved against `root` -- not against the process working
    directory, which is a different tree when auditing a worktree.
    """
    if not path:
        return None
    p = Path(path)
    if not p.is_absolute():
        p = root / p
    try:
        return os.path.relpath(p, root).replace(os.sep, "/")
    except ValueError:
        return str(path)


def _function_from_memberdef(member, root: Path) -> Function | None:
    name = member.findtext("name")
    if not name:
        return None
    loc = member.find("location")
    if loc is None:
        return None

    arg_parts = _split_args(member.findtext("argsstring") or "")
    decl_params = member.findall("param")
    docs = _param_docs(member)

    params: list[Param] = []
    for i, p in enumerate(decl_params):
        ptype = _text(p.find("type"))
        pname = p.findtext("declname") or p.findtext("defname")
        if arg_parts and i < len(arg_parts):
            from_args = _name_of_arg(arg_parts[i])
            if from_args:
                pname = from_args
            if p.find("array") is not None or re.search(
                r"\[[^\]]*\]\s*$", arg_parts[i].strip()
            ):
                ptype += "[]"
        params.append(Param(name=pname, type=ptype, doc=docs.get(pname or "", "")))

    declfile = _relative(loc.get("declfile") or loc.get("file"), root)
    declline = loc.get("declline") or loc.get("line")
    file_attr = loc.get("file")
    bodyfile = loc.get("bodyfile")
    return Function(
        name=name,
        return_type=_text(member.find("type")),
        params=params,
        header=declfile if (declfile or "").endswith((".h", ".hpp")) else None,
        header_line=int(declline) if declline and declline.isdigit() else None,
        body_file=_relative(loc.get("bodyfile"), root),
        body_start=(
            int(loc.get("bodystart"))
            if (loc.get("bodystart") or "").isdigit()
            else None
        ),
        body_end=(
            int(loc.get("bodyend")) if (loc.get("bodyend") or "").isdigit() else None
        ),
        brief=_text(member.find("briefdescription")),
        detail=_text(member.find("detaileddescription")),
        from_definition=bool(file_attr and bodyfile and file_attr == bodyfile),
    )


def _header_of(member, root: Path) -> str | None:
    loc = member.find("location")
    if loc is None:
        return None
    path = _relative(loc.get("declfile") or loc.get("file"), root)
    return path if (path or "").endswith((".h", ".hpp")) else None


def _enum_from_memberdef(member, root: Path) -> Enum | None:
    name = member.findtext("name")
    if not name or name.startswith("@"):  # anonymous
        return None
    members = [
        EnumMember(
            name=v.findtext("name") or "",
            value=_text(v.find("initializer")).lstrip("= ").strip(),
        )
        for v in member.findall("enumvalue")
    ]
    return Enum(name=name, members=members, header=_header_of(member, root))


def _typedef_from_memberdef(member, root: Path) -> Typedef | None:
    name = member.findtext("name")
    if not name:
        return None
    type_ = _text(member.find("type"))
    args = (member.findtext("argsstring") or "").strip()
    return Typedef(
        name=name, type=(type_ + args).strip(), header=_header_of(member, root)
    )


def _struct_from_compounddef(compound, root: Path) -> Struct | None:
    kind = compound.get("kind")
    if kind not in ("struct", "union"):
        return None
    name = compound.findtext("compoundname")
    if not name or "@" in name:
        return None
    loc = compound.find("location")
    header = None
    if loc is not None:
        path = _relative(loc.get("file"), root)
        header = path if (path or "").endswith((".h", ".hpp")) else None
    fields = []
    for m in compound.iter("memberdef"):
        if m.get("kind") != "variable":
            continue
        fname = m.findtext("name")
        if not fname:
            continue
        ftype = _text(m.find("type")) + (m.findtext("argsstring") or "")
        fields.append(Field(name=fname, type=" ".join(ftype.split())))
    return Struct(name=name, kind=kind, fields=fields, header=header)


def _combine_docs(have: str, extra: str) -> str:
    """Fold `extra` into `have`, keeping whichever text says more."""
    if not extra or extra in have:
        return have
    if have and have in extra:
        return extra
    return f"{have} {extra}".strip()


def _param_docs(member) -> dict[str, str]:
    """Per-parameter documentation, keyed by the name used in the comment."""
    out: dict[str, str] = {}
    detail = member.find("detaileddescription")
    if detail is None:
        return out
    for item in detail.iter("parameteritem"):
        desc = _text(item.find("parameterdescription"))
        for nm in item.iter("parametername"):
            name = _text(nm)
            if name:
                out[name] = _combine_docs(out.get(name, ""), desc)
    return out


def _merge(prev: Function | None, new: Function) -> Function:
    """Doxygen emits a function once per file that mentions it; combine them.

    The declaration's entry knows the public header and carries the Doxygen
    comment; the definition's entry carries the parameter names the body uses.
    Take each from whichever entry actually has it.
    """
    if prev is None:
        return new

    # the public header always wins over a private one
    def _better_header(a: Function, b: Function):
        for cand in (a, b):
            if cand.header and cand.header.startswith(PUBLIC_INCLUDE_DIR):
                return cand.header, cand.header_line
        for cand in (a, b):
            if cand.header:
                return cand.header, cand.header_line
        return None, None

    header, header_line = _better_header(prev, new)

    definition = next((f for f in (prev, new) if f.from_definition), None)
    declaration = next(
        (
            f
            for f in (prev, new)
            if not f.from_definition
            and f.header
            and f.header.startswith(PUBLIC_INCLUDE_DIR)
        ),
        None,
    )
    documented = next((f for f in (prev, new) if f.detail), None) or prev
    with_body = next((f for f in (prev, new) if f.has_body), prev)

    params = definition.params if definition else (prev.params or new.params)
    decl_params = (
        declaration.params
        if declaration
        else (prev.decl_params or new.decl_params or params)
    )
    decl_return_type = (
        declaration.return_type
        if declaration
        else (prev.decl_return_type or new.decl_return_type)
    )

    for src in (declaration, documented):
        if src is None or src is definition:
            continue
        for i, p in enumerate(params):
            if i < len(src.params):
                p.doc = _combine_docs(p.doc, src.params[i].doc)

    return Function(
        name=prev.name,
        return_type=(definition or prev).return_type or new.return_type,
        params=params,
        header=header,
        header_line=header_line,
        body_file=with_body.body_file,
        body_start=with_body.body_start,
        body_end=with_body.body_end,
        brief=documented.brief or prev.brief or new.brief,
        detail=documented.detail or prev.detail or new.detail,
        from_definition=bool(definition),
        decl_params=decl_params,
        decl_return_type=decl_return_type,
    )


def doxygen_available() -> bool:
    return shutil.which("doxygen") is not None


def run_doxygen(repo_root: Path, out_dir: Path, inputs) -> None:
    """Generate Doxygen XML for `repo_root` into `out_dir`."""
    if not doxygen_available():
        raise RuntimeError(
            "doxygen not found on PATH. Install it (apt install doxygen) or pass "
            "an existing XML directory."
        )
    docs_doxyfile = repo_root / "docs" / "Doxyfile"
    if not docs_doxyfile.is_file():
        raise RuntimeError(f"missing {docs_doxyfile}")

    doxyfile = out_dir / "Doxyfile"
    doxyfile.write_text(
        DOXYFILE_TEMPLATE.format(
            docs_doxyfile=docs_doxyfile,
            out_dir=out_dir,
            repo_root=repo_root,
            inputs=" ".join(str(repo_root / i) for i in inputs),
        )
    )
    proc = subprocess.run(
        ["doxygen", str(doxyfile)],
        cwd=repo_root,
        capture_output=True,
        text=True,
    )
    if proc.returncode != 0:
        raise RuntimeError(f"doxygen failed:\n{proc.stderr[-2000:]}")
