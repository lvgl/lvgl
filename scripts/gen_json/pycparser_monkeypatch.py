# -*- coding: utf-8 -*-

import sys
from pycparser import c_ast  # NOQA
from pycparser.c_generator import CGenerator
import tempfile

temp_directory = tempfile.mkdtemp(suffix='.lvgl_json')

import doc_builder  # NOQA

generator = CGenerator()

doc_builder.EMIT_WARNINGS = False

docs = doc_builder.XMLSearch(temp_directory)

get_enum_item_docs = docs.get_enum_item
get_enum_docs = docs.get_enum
get_func_docs = docs.get_function
get_var_docs = docs.get_variable
get_union_docs = docs.get_union
get_struct_docs = docs.get_structure
get_typedef_docs = docs.get_typedef
get_macro_docs = docs.get_macro


BASIC_TYPES = [
    'float',
    'double',
    'long',
    'ulong',
    'unsigned long',
    'long double',
    'signed long double',
    'unsigned long double',
    'long long',
    'signed long long',
    'unsigned long long',
    'int',
    'uint',
    'signed int',
    'unsigned int',
    'long int',
    'signed long int',
    'unsigned long int',
    'short'
    'ushort',
    'signed short',
    'unsigned short',
    'void',
    'char',
    'uchar',
    'signed char',
    'unsigned char',
    'bool'
]

STDLIB_TYPES = [
    'size_t',
    'uint8_t',
    'uint16_t',
    'uint32_t',
    'uint64_t',
    'int8_t',
    'int16_t',
    'int32_t',
    'int64_t',
    'va_list',
    'uintptr_t',
    'intptr_t',
]

enums = {}
functions = {}
structures = {}
unions = {}
typedefs = {}
macros = {}


def is_lib_c_node(n):
    if hasattr(n, 'coord') and n.coord is not None:
        if 'fake_libc_include' in n.coord.file:
            return True
    return False


class ArrayDecl(c_ast.ArrayDecl):

    def process(self):
        self.type.process()

    def __init__(self, *args, **kwargs):
        super().__init__(*args, **kwargs)
        self._parent = None

    @property
    def name(self):
        return None

    @property
    def parent(self):
        return self._parent

    @parent.setter
    def parent(self, value):
        self._parent = value
        self.type.parent = self

    def to_dict(self):
        if self.dim is None:
            dim = None
        else:
            dim = generator.visit(self.dim)

        if isinstance(self.type, TypeDecl):
            res = self.type.to_dict()
            res['json_type'] = 'array'
            res['dim'] = dim
            return res

        return {
            'type': self.type.to_dict(),
            'dim': dim,
            'json_type': 'array'
        }


class Constant(c_ast.Constant):

    def process(self):
        pass

    def __init__(self, *args, **kwargs):
        super().__init__(*args, **kwargs)
        self._parent = None

    @property
    def parent(self):
        return self._parent

    @parent.setter
    def parent(self, value):
        self._parent = value
        self.type.parent = self

    def to_dict(self):
        if is_lib_c_node(self):
            return None

        return self.value


collected_types = []

forward_decls = {}
used_forward_decl = []


class Decl(c_ast.Decl):

    def process(self):
        self.type.process()

        if (
            not isinstance(self.parent, (Struct, Union, FuncDecl)) and
            not isinstance(self.type, (Enum, FuncDef, FuncDecl))
        ):
            name = self.name
            t_name = self.type.name

            if name is None:
                name = t_name

            if name == t_name:
                if isinstance(self.type, (Struct, Union)):
                    if name not in forward_decls:
                        res = self.type.to_dict()
                        forward_decls[name] = res

    def __init__(self, *args, **kwargs):
        super().__init__(*args, **kwargs)
        self._parent = None

    @property
    def parent(self):
        return self._parent

    @parent.setter
    def parent(self, value):
        self._parent = value
        self.type.parent = self

    def to_dict(self):
        if self.name and self.name == '_silence_gcc_warning':
            return None

        if not self.name:
            try:
                name = self.type.name
            except AttributeError:
                name = None

            if name:
                if name == '_silence_gcc_warning':
                    return None
        else:
            name = self.name

        if isinstance(self.parent, (Struct, Union)):
            if self.bitsize:
                bitsize = self.bitsize.to_dict()
            else:
                bitsize = None

            res = {
                'name': name,
                'json_type': 'field',
                'type': self.type.to_dict(),
                'bitsize': bitsize
            }
        elif isinstance(self.parent, FuncDecl):
            res = {
                'name': name,
                'json_type': 'arg',
                'type': self.type.to_dict()
            }

        elif isinstance(self.type, Enum):
            res = self.type.to_dict()
            res['name'] = name

        elif isinstance(self.type, (FuncDef, FuncDecl)):
            res = self.type.to_dict()
            res['name'] = name

        else:
            name = self.name
            t_name = self.type.name

            if name is None:
                name = t_name

            if name == t_name:
                if isinstance(self.type, (Struct, Union)):
                    if name in used_forward_decl:
                        if name in forward_decls:
                            res = forward_decls[name]
                            del forward_decls[name]
                        else:
                            res = None
                    else:
                        used_forward_decl.append(name)
                        res = {
                            'name': self.name,
                            'json_type': 'forward_decl',
                            'type': {
                                'name': self.type.__class__.__name__.lower(),
                                'json_type': 'primitive_type'
                            }
                        }

                    return res
            # raise RuntimeError('sanity check')

            res = {
                'name': name,
                'json_type': str(type(self)),
                'type': self.type.to_dict()
            }

        return res


class EllipsisParam(c_ast.EllipsisParam):

    def process(self):
        pass

    def __init__(self, *args, **kwargs):
        super().__init__(*args, **kwargs)
        self._parent = None

    @property
    def parent(self):
        return self._parent

    @parent.setter
    def parent(self, value):
        self._parent = value

    def to_dict(self):
        if is_lib_c_node(self):
            return None

        return {
            'name': '...',
            'type': {
                'name': 'ellipsis',
                'json_type': 'special_type'
            },
            'json_type': 'arg'
        }

    @property
    def name(self):
        return '...'


class Enum(c_ast.Enum):

    def process(self):
        name = self.name
        parent = self.parent

        while parent is not None and name is None:
            try:
                name = parent.name
            except AttributeError:
                pass

            parent = parent.parent

        if name and name not in collected_types:
            collected_types.append(name)

        for item in (self.values or []):
            item.process()

    def __init__(self, *args, **kwargs):
        super().__init__(*args, **kwargs)
        self._parent = None

    @property
    def parent(self):
        return self._parent

    @parent.setter
    def parent(self, value):
        self._parent = value
        if self.values:
            self.values.parent = self

    def to_dict(self):

        if self.name:
            doc_search = get_enum_docs(self.name)

            if doc_search is None:
                doctsring = None
            else:
                docstring = doc_search.description

        else:
            docstring = None

        res = {
            'name': self.name,
            'type': {
                'name': 'int',
                'json_type': 'primitive_type'
            },
            'json_type': 'enum',
            'members': [item.to_dict() for item in (self.values or [])],
            'docstring': docstring
        }

        return res


class Enumerator(c_ast.Enumerator):

    def process(self):
        pass

    def __init__(self, *args, **kwargs):
        super().__init__(*args, **kwargs)
        self._parent = None

    @property
    def parent(self):
        return self._parent

    @parent.setter
    def parent(self, value):
        self._parent = value

    def to_dict(self):
        parent_name = self.parent.name
        parent = self.parent

        while parent is not None and parent_name is None:
            try:
                parent_name = parent.name
            except AttributeError:
                continue

            parent = parent.parent

        if parent_name and parent_name.startswith('_'):
            if parent_name[1:] in collected_types:
                type_ = {
                    'name': parent_name[1:],
                    'json_type': 'lv_type'
                }
            elif parent_name in collected_types:
                type_ = {
                    'name': parent_name,
                    'json_type': 'lv_type'
                }
            else:
                type_ = {
                    'name': 'int',
                    'json_type': 'primitive_type'
                }

        elif parent_name and parent_name in collected_types:
            type_ = {
                'name': parent_name,
                'json_type': 'lv_type'
            }
        else:
            type_ = {
                'name': 'int',
                'json_type': 'primitive_type'
            }

        doc_search = get_enum_item_docs(self.name)

        if doc_search is None:
            docstring = None
        else:
            docstring = doc_search.description

        return {
            'name': self.name,
            'type': type_,
            'json_type': 'enum_member',
            'docstring': docstring
        }


class EnumeratorList(c_ast.EnumeratorList):

    def process(self, indent):
        pass

    def __init__(self, *args, **kwargs):
        super().__init__(*args, **kwargs)
        self._parent = None

    @property
    def parent(self):
        return self._parent

    @parent.setter
    def parent(self, value):
        self._parent = value

        for item in (self.enumerators or []):
            item.parent = value

    def to_dict(self):
        pass


def is_type(obj, type_):

    if isinstance(obj, type_):
        return True

    try:
        return is_type(obj.type, type_)

    except AttributeError:
        return False


found_types = {}


class FileAST(c_ast.FileAST):

    def __init__(self, *args, **kwargs):
        super().__init__(*args, **kwargs)
        self._parent = None

    @property
    def name(self):
        return None

    @property
    def parent(self):
        return self._parent

    @parent.setter
    def parent(self, value):
        self._parent = value

    def to_dict(self):
        if is_lib_c_node(self):
            return None

        items = []

        for item in self.ext:
            if is_lib_c_node(item):
                continue
            try:
                item.parent = self
                items.append(item)
            except AttributeError:
                pass

        enums = []
        functions = []
        structures = []
        unions = []
        typedefs = []

        for item in items:
            item.process()
            if is_type(item, Enum):
                enums.append(item)
            elif is_type(item, FuncDef):
                functions.append(item)
            elif is_type(item, Struct):
                structures.append(item)
            elif is_type(item, Union):
                unions.append(item)
            elif is_type(item, Typedef):
                if item.is_struct:
                    structures.append(item.type)
                if item.is_union:
                    unions.append(item.type)
                typedefs.append(item)

            # elif is_type(item, Macro):
            #     macros = {}

        used_enum_names = []
        used_func_names = []
        used_struct_names = []
        used_union_names = []
        used_typedef_names = []

        res = {
            'enums': [],
            'functions': [],
            'structures': [],
            'unions': [],
            'typedefs': []
        }

        def _file_obj(obj):
            type_ = obj['json_type']
            print(type_)
            try:
                if type_ == 'enum':
                    if obj['name'] not in used_enum_names:
                        used_enum_names.append(obj['name'])
                        res['enums'].append(obj)
            except TypeError:
                print(obj)
                raise

            if type_ == 'function':
                if obj['name'] not in used_func_names:
                    used_func_names.append(obj['name'])
                    res['functions'].append(obj)

            if type_ == 'union':
                if obj['name'] not in used_union_names:
                    used_union_names.append(obj['name'])
                    res['unions'].append(obj)

            if type_ == 'struct':
                if obj['name'] not in used_struct_names:
                    used_struct_names.append(obj['name'])
                    res['structures'].append(obj)

            if type_ == 'typedef':
                if obj['name'] not in used_typedef_names:
                    used_typedef_names.append(obj['name'])
                    res['typedefs'].append(obj)

        for enum in enums:
            if enum is None:
                continue
            enum = enum.to_dict()
            if enum is None:
                continue

            _file_obj(enum)

        for struct in structures:
            if struct is None:
                continue
            struct = struct.to_dict()
            if struct is None:
                continue

            _file_obj(struct)

        for union in unions:
            if union is None:
                continue
            union = union.to_dict()
            if union is None:
                continue

            _file_obj(union)

        for typedef in typedefs:
            if typedef is None:
                continue

            if typedef.is_struct:
                s = typedef.get_struct_union()


                if s['name'] and s['name'] not in used_struct_names:
                    _file_obj(s)

            elif typedef.is_union:
                u = typedef.get_struct_union()

                if u['name'] and u['name'] not in used_union_names:
                    _file_obj(u)

            typedef = typedef.to_dict()
            if typedef is None:
                continue

            _file_obj(typedef)

        for function in functions:
            if function is None:
                continue
            function = function.to_dict()
            if function is None:
                continue

            _file_obj(function)

        return res


class FuncDecl(c_ast.FuncDecl):

    @property
    def name(self):
        type_ = self.type
        while isinstance(type_, PtrDecl):
            type_ = type_.type

        try:
            name = type_.name
        except AttributeError:
            name = None
            parent = self.parent
            while parent is not None and name is None:
                try:
                    name = parent.name
                except AttributeError:
                    pass

                parent = parent.parent

        return name

    def process(self):
        name = self.name
        if name and name not in collected_types:
            collected_types.append(name)

        for arg in (self.args or []):
            arg.process()

        self.type.process()

    def __init__(self, *args, **kwargs):
        super().__init__(*args, **kwargs)
        self._parent = None

    @property
    def parent(self):
        return self._parent

    @parent.setter
    def parent(self, value):
        self._parent = value

        for arg in (self.args or []):
            arg.parent = self

        self.type.parent = self

    def to_dict(self):

        if self.name:
            doc_search = get_func_docs(self.name)

            if doc_search is None:
                docstring = None
            else:
                docstring = doc_search.description
        docstring = None

        res = {
            'name': self.name,
            'args': [arg.to_dict() for arg in (self.args or [])],
            'json_type': 'function',
            'type': self.type.to_dict(),
            'docstring': docstring
        }

        return res


class FuncDef(c_ast.FuncDef):

    @property
    def name(self):
        return None

    def process(self):
        self.decl.process()

    def __init__(self, *args, **kwargs):
        super().__init__(*args, **kwargs)
        self._parent = None

    @property
    def parent(self):
        return self._parent

    @parent.setter
    def parent(self, value):
        self._parent = value
        self.decl.parent = value

    def to_dict(self):
        return self.decl.to_dict()


class IdentifierType(c_ast.IdentifierType):

    def process(self):
        pass

    def __init__(self, *args, **kwargs):
        super().__init__(*args, **kwargs)
        self._parent = None

    @property
    def name(self):
        return ' '.join(self.names)

    @property
    def parent(self):
        return self._parent

    @parent.setter
    def parent(self, value):
        self._parent = value

    def to_dict(self):
        if is_lib_c_node(self):
            return None

        name = ' '.join(self.names)

        if name in BASIC_TYPES:
            json_type = 'primitive_type'
        elif name in STDLIB_TYPES:
            json_type = 'stdlib_type'
        elif name in collected_types:
            json_type = 'lv_type'
        elif name.startswith('_') and name[1:] in collected_types:
            name = name[1:]
            json_type = 'lv_type'
        else:
            json_type = 'unknown_type'

        return {
            'name': name,
            'json_type': json_type
        }


class ParamList(c_ast.ParamList):

    def process(self):
        pass

    def __init__(self, *args, **kwargs):
        super().__init__(*args, **kwargs)
        self._parent = None

    @property
    def parent(self):
        return self._parent

    @parent.setter
    def parent(self, value):
        self._parent = value
        for param in (self.params or []):
            param.parent = value

    def to_dict(self):
        pass


class PtrDecl(c_ast.PtrDecl):

    @property
    def name(self):
        return None

    def process(self):
        self.type.process()

    def __init__(self, *args, **kwargs):
        super().__init__(*args, **kwargs)
        self._parent = None

    @property
    def parent(self):
        return self._parent

    @parent.setter
    def parent(self, value):
        self._parent = value
        self.type.parent = self

    def to_dict(self):
        res = {
            'json_type': 'pointer',
            'type': self.type.to_dict()
        }

        return res


class Struct(c_ast.Struct):

    def process(self):
        for decl in (self.decls or []):
            decl.process()

        name = self.name
        parent = self.parent
        while parent is not None and name is None:
            name = parent.name
            parent = parent.parent

        if name and name not in collected_types:
            collected_types.append(name)

    def __init__(self, *args, **kwargs):
        super().__init__(*args, **kwargs)
        self._parent = None

    @property
    def parent(self):
        return self._parent

    @parent.setter
    def parent(self, value):
        self._parent = value

        for decl in (self.decls or []):
            decl.parent = self

    def to_dict(self):

        if self.name:
            doc_search = get_struct_docs(self.name)

            if doc_search is None:
                docstring = None

            else:
                docstring = doc_search.description
        else:
            docstring = None

        if not self.decls:
            res = {
                'name': self.name,
                'json_type': 'lv_type',
            }
        else:
            res = {
                'name': self.name,
                'json_type': 'struct',
                'fields': [field.to_dict() for field in self.decls],
                'docstring': docstring
            }

        return res


class TypeDecl(c_ast.TypeDecl):

    @property
    def name(self):
        return self.declname

    def process(self):
        self.type.process()

    def __init__(self, *args, **kwargs):
        super().__init__(*args, **kwargs)
        self._parent = None

    @property
    def parent(self):
        return self._parent

    @parent.setter
    def parent(self, value):
        parent = value

        while parent is not None:
            try:
                if parent.declname == self.declname:
                    break
            except AttributeError:
                pass

            try:
                if parent.name == self.declname:
                    break
            except AttributeError:
                pass

            parent = parent.parent

        if parent is None:
            self._parent = value

            self.type.parent = self
        else:
            self.type.parent = parent

    def to_dict(self):
        if self.parent is None:
            res = self.type.to_dict()
            if self.declname is not None and not self.type.name:
                res['name'] = self.declname

        elif isinstance(self.type, (Union, Struct)):
            res = self.type.to_dict()

            if not self.type.name and self.declname:
                res['name'] = self.declname
        else:
            res = {
                'name': self.declname,
                'json_type': str(type(self)),
                'type': self.type.to_dict()
            }

        return res


class Typedef(c_ast.Typedef):

    def process(self):
        self.type.process()

        if self.name and self.name not in collected_types:
            collected_types.append(self.name)

    def __init__(self, *args, **kwargs):
        super().__init__(*args, **kwargs)
        self._parent = None

    @property
    def parent(self):
        return self._parent

    @parent.setter
    def parent(self, value):
        self._parent = value

        self.type.parent = self

    @property
    def is_struct(self):
        return isinstance(self.type, TypeDecl) and isinstance(self.type.type, Struct)

    @property
    def is_union(self):
        return isinstance(self.type, TypeDecl) and isinstance(self.type.type, Union)

    def get_struct_union(self):
        res = self.type.type.to_dict()
        if not self.type.type.name and self.type.name:
            res['name'] = self.type.name
        elif not self.type.type.name:
            res['name'] = self.name

        return res

    def to_dict(self):
        doc_search = get_typedef_docs(self.name)

        if doc_search is None:
            docstring = None
        else:
            docstring = doc_search.description

        if isinstance(self.type, TypeDecl) and isinstance(self.type.type, (Struct, Union)):
            res = {
                'name': self.name,
                'json_type': 'typedef',
                'type': {
                    'name': self.type.name,
                    'json_type': 'lv_type'
                },
                'docstring': docstring
            }
        else:
            res = {
                'name': self.name,
                'json_type': 'typedef',
                'type': self.type.to_dict(),
                'docstring': docstring
            }
        return res


class Typename(c_ast.Typename):

    def process(self):
        self.type.process()

    def __init__(self, *args, **kwargs):
        super().__init__(*args, **kwargs)
        self._parent = None

    @property
    def parent(self):
        return self._parent

    @parent.setter
    def parent(self, value):
        self._parent = value

        self.type.parent = self

    def to_dict(self):
        if not self.name and isinstance(self.type, IdentifierType):
            res = self.type.to_dict()

        elif isinstance(self.parent, FuncDecl):
            res = {
                'name': self.name,
                'json_type': 'arg',
                'type': self.type.to_dict()
            }
        else:
            res = {
                'name': self.name,
                'json_type': str(type(self)),
                'type': self.type.to_dict()
            }

        return res


class Union(c_ast.Union):

    def process(self):
        for field in (self.decls or []):
            field.process()

        name = self.name
        parent = self.parent
        while parent is not None and name is None:
            try:
                name = parent.name
            except AttributeError:
                pass

            parent = parent.parent

        if name and name not in collected_types:
            collected_types.append(name)

    def __init__(self, *args, **kwargs):
        super().__init__(*args, **kwargs)
        self._parent = None

    @property
    def parent(self):
        return self._parent

    @parent.setter
    def parent(self, value):
        self._parent = value

        for decl in (self.decls or []):
            decl.parent = self

    def to_dict(self):
        if not self.decls:
            res = {
                'name': self.name,
                'json_type': 'lv_type',
            }
        else:
            res = {
                'name': self.name,
                'json_type': 'struct',
                'fields': [field.to_dict() for field in self.decls]
            }

        return res


for cls in (
    ArrayDecl,
    Constant,
    Decl,
    EllipsisParam,
    Enum,
    Enumerator,
    EnumeratorList,
    EnumeratorList,
    FileAST,
    FuncDecl,
    FuncDef,
    IdentifierType,
    ParamList,
    PtrDecl,
    Struct,
    TypeDecl,
    Typedef,
    Typename,
    Union
):
    cls_name = cls.__name__
    setattr(getattr(sys.modules['pycparser.c_parser'], 'c_ast'), cls_name, cls)
