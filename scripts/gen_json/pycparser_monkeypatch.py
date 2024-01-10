# -*- coding: utf-8 -*-

import sys
from pycparser import c_ast  # NOQA
from pycparser.c_generator import CGenerator
from collections import OrderedDict

import doc_builder  # NOQA

generator = CGenerator()

doc_builder.EMIT_WARNINGS = False
# doc_builder.DOXYGEN_OUTPUT = False


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

        res = OrderedDict([
            ('type', self.type.to_dict()),
            ('json_type', 'array'),
            ('dim', dim)
        ])

        return res


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


class Decl(c_ast.Decl):

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

            res = OrderedDict([
                ('name', name),
                ('type', self.type.to_dict()),
                ('json_type', 'field'),
                ('bitsize', bitsize)
            ])
        elif isinstance(self.parent, FuncDecl):
            res = OrderedDict([
                ('name', name),
                ('type', self.type.to_dict()),
                ('json_type', 'arg'),
            ])
        elif isinstance(self.type, Enum):
            res = self.type.to_dict()
            res['name'] = name

        elif isinstance(self.type, (FuncDef, FuncDecl)):
            res = self.type.to_dict()
            res['name'] = name

        else:
            if isinstance(self.type, (Struct, Union)):
                res = self.type.to_dict()

                if res['json_type'] == 'forward_decl':
                    if res['name'] and res['name'] not in forward_decls:
                        forward_decls[res['name']] = res

                    return None

                return res

            if self.name:
                name = self.name
            else:
                name = self.type.name

            doc_search = get_var_docs(name)

            if doc_search is None:
                docstring = None
            else:
                docstring = doc_search.description

            res = OrderedDict([
                ('name', name),
                ('type', self.type.to_dict()),
                ('json_type', 'variable'),
                ('docstring', docstring),
                ('quals', self.quals),
                ('storage', self.storage)
            ])

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

        res = OrderedDict([
            ('name', '...'),
            ('type', OrderedDict([('name', 'ellipsis'), ('json_type', 'special_type')])),
            ('json_type', 'arg'),
            ('docstring', None)
        ])

        return res

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
                docstring = None
            else:
                docstring = doc_search.description
        else:
            docstring = None

        res = OrderedDict([
            ('name', self.name),
            ('type', OrderedDict([('name', 'int'), ('json_type', 'primitive_type')])),
            ('json_type', 'enum'),
            ('docstring', docstring),
            ('members', [item.to_dict() for item in (self.values or [])])
        ])

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
                type_ = OrderedDict([('name', parent_name[1:]), ('json_type', 'lvgl_type')])
            elif parent_name in collected_types:
                type_ = OrderedDict([('name', parent_name), ('json_type', 'lvgl_type')])
            else:
                type_ = OrderedDict([('name', 'int'), ('json_type', 'primitive_type')])

        elif parent_name and parent_name in collected_types:
            type_ = OrderedDict([('name',parent_name), ('json_type', 'lvgl_type')])
        else:
            type_ = OrderedDict([('name', 'int'), ('json_type', 'primitive_type')])

        doc_search = get_enum_item_docs(self.name)

        if doc_search is None:
            docstring = None
        else:
            docstring = doc_search.description

        res = OrderedDict([
            ('name', self.name),
            ('type', type_),
            ('json_type', 'enum_member'),
            ('docstring', docstring)
        ])

        return res


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

get_enum_item_docs = None
get_enum_docs = None
get_func_docs = None
get_var_docs = None
get_union_docs = None
get_struct_docs = None
get_typedef_docs = None
get_macro_docs = None
get_macros = None


class FileAST(c_ast.FileAST):

    def __init__(self, *args, **kwargs):
        super().__init__(*args, **kwargs)
        self._parent = None

    def setup_docs(self, temp_directory):
        global get_enum_item_docs
        global get_enum_docs
        global get_func_docs
        global get_var_docs
        global get_union_docs
        global get_struct_docs
        global get_typedef_docs
        global get_macro_docs
        global get_macros

        docs = doc_builder.XMLSearch(temp_directory)

        get_enum_item_docs = docs.get_enum_item
        get_enum_docs = docs.get_enum
        get_func_docs = docs.get_function
        get_var_docs = docs.get_variable
        get_union_docs = docs.get_union
        get_struct_docs = docs.get_structure
        get_typedef_docs = docs.get_typedef
        get_macro_docs = docs.get_macro
        get_macros = docs.get_macros

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
        variables = []

        for item in items:
            item.process()
            if is_type(item, Typedef):
                typedefs.append(item)
            elif is_type(item, Enum):
                enums.append(item)
            elif is_type(item, FuncDef):
                functions.append(item)
            elif is_type(item, FuncDecl):
                functions.append(item)
            elif is_type(item, Struct):
                structures.append(item)
            elif is_type(item, Union):
                unions.append(item)
            elif is_type(item, Decl):
                variables.append(item)
            else:
                print('UNKNOWN TYPE:')
                print(item)
                print(item.to_dict())

        res = {
            'enums': [
                enum.to_dict()
                for enum in enums
                if enum is not None and enum.to_dict() is not None
            ],
            'functions': [
                func.to_dict()
                for func in functions
                if func is not None and func.to_dict() is not None
            ],
            'structures': [
                struct.to_dict()
                for struct in structures
                if struct is not None and struct.to_dict() is not None
            ],
            'unions': [
                union.to_dict()
                for union in unions
                if union is not None and union.to_dict() is not None
            ],
            'variables': [
                var.to_dict()
                for var in variables
                if var is not None and var.to_dict() is not None
            ],
            'typedefs': [],
            'forward_decls': list(forward_decls.values()),
            'macros': []
        }

        for typedef in typedefs:
            if typedef is None:
                continue

            typedef = typedef.to_dict()
            if typedef is None:
                continue

            if isinstance(typedef, list):
                typedef, obj_dict = typedef
                if obj_dict['json_type'] == 'struct':
                    res['structures'].append(obj_dict)
                elif obj_dict['json_type'] == 'union':
                    res['unions'].append(obj_dict)
                elif obj_dict['json_type'] == 'enum':
                    res['enums'].append(obj_dict)

            res['typedefs'].append(typedef)

        for macro in get_macros():
            macro_type = OrderedDict([
                ('name', macro.name),
                ('json_type', 'macro'),
                ('docstring', macro.description)
            ])

            res['macros'].append(macro_type)

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
                ret_docstring = None
            else:
                docstring = doc_search.description
                ret_docstring = doc_search.res_description

        else:
            doc_search = None
            docstring = None
            ret_docstring = None

        args = []

        for arg in (self.args or []):
            arg = arg.to_dict()
            if arg['name'] and doc_search is not None:
                for doc_arg in doc_search.args:
                    if doc_arg.name == arg['name']:
                        arg['docstring'] = doc_arg.description
                        break
                else:
                    arg['docstring'] = None
            else:
                arg['docstring'] = None

            args.append(arg)

        type_dict = OrderedDict([
            ('type', self.type.to_dict()),
            ('json_type', 'ret_type'),
            ('docstring', ret_docstring)
        ])

        res = OrderedDict([
            ('name', self.name),
            ('type', type_dict),
            ('json_type', 'function'),
            ('docstring', docstring),
            ('args', args)
        ])

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
            json_type = 'lvgl_type'
        elif name.startswith('_') and name[1:] in collected_types:
            name = name[1:]
            json_type = 'lvgl_type'
        else:
            json_type = 'unknown_type'

        res = OrderedDict([
            ('name', name),
            ('json_type', json_type),
        ])

        return res


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
        if isinstance(self.type, FuncDecl):
            res = OrderedDict([
                ('type', self.type.to_dict()),
                ('json_type', 'function_pointer')
            ])

        else:
            res = OrderedDict([
                ('type', self.type.to_dict()),
                ('json_type', 'pointer')
            ])

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
        if not self.decls:
            name = self.name
            if not name:
                self.name = self.parent.name

            if name:
                struct_doc = get_struct_docs(name)
                if struct_doc:
                    docstring = struct_doc.description
                else:
                    docstring = None
            else:
                docstring = None

            res = OrderedDict([
                ('name', name),
                ('type', OrderedDict([('name', 'struct'), ('json_type', 'primitive_type')])),
                ('json_type', 'forward_decl'),
                ('docstring', docstring),
            ])

        else:
            if self.name:
                struct_doc = get_struct_docs(self.name)
            elif self.parent.name:
                struct_doc = get_struct_docs(self.parent.name)
            else:
                struct_doc = None

            if struct_doc is not None:
                docstring = struct_doc.description
            else:
                docstring = None

            fields = []

            for field in self.decls:
                field = field.to_dict()

                if struct_doc is not None:
                    for field_doc in struct_doc.fields:
                        if field_doc.name == field['name']:
                            field_docstring = field_doc.description
                            break
                    else:
                        field_docstring = None
                else:
                    field_docstring = None

                field['docstring'] = field_docstring
                field['json_type'] = 'field'
                fields.append(field)

            res = OrderedDict([
                ('name', self.name),
                ('type', OrderedDict([('name', 'struct'), ('json_type', 'primitive_type')])),
                ('json_type', 'struct'),
                ('docstring', docstring),
                ('fields', fields)
            ])

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
            res = OrderedDict([
                ('name', self.declname),
                ('type', self.type.to_dict()),
                ('json_type', str(type(self))),
            ])

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

        if isinstance(self.type, TypeDecl):

            if isinstance(self.type.type, (Struct, Union)):
                if self.type.type.decls:
                    if self.type.type.name:
                        type_dict = self.type.type.to_dict()

                        if not type_dict['docstring']:
                            type_dict['docstring'] = docstring
                            docstring = None

                        res = OrderedDict([
                            ('name', self.name),
                            ('type', OrderedDict([('name', type_dict['name']), ('json_type', 'lvgl_type')])),
                            ('json_type', 'typedef'),
                            ('docstring', docstring),
                        ])

                        return [
                            res,
                            type_dict
                        ]

                    else:
                        res = OrderedDict([
                            ('name', self.name),
                            ('type', self.type.type.to_dict()),
                            ('json_type', 'typedef'),
                            ('docstring', docstring),
                        ])

                        return res
                else:
                    if self.name in forward_decls:
                        print(self.name, forward_decls[self.name])

                    res = OrderedDict([
                        ('name', self.name),
                        ('type', OrderedDict([('name', self.type.type.name), ('json_type', 'lvgl_type')])),
                        ('json_type', 'typedef'),
                        ('docstring', docstring),
                    ])

                    return res

            elif isinstance(self.type.type, Enum):
                if self.type.type.name:
                    type_dict = self.type.type.to_dict()

                    if not type_dict['docstring']:
                        type_dict['docstring'] = docstring
                        docstring = None

                    if not type_dict['name']:
                        if self.type.name:
                            type_dict['name'] = self.type.name
                        else:
                            type_dict['name'] = self.name

                    res = OrderedDict([
                        ('name', self.name),
                        ('type', OrderedDict([('name', type_dict['name']), ('json_type', 'lvgl_type')])),
                        ('json_type', 'typedef'),
                        ('docstring', docstring),
                    ])

                    return [
                        res,
                        type_dict
                    ]
                else:
                    type_dict = self.type.type.to_dict()

                    res = OrderedDict([
                        ('name', self.name),
                        ('type', type_dict),
                        ('json_type', 'typedef'),
                        ('docstring', docstring),
                    ])

                    return res

        res = OrderedDict([
            ('name', self.name),
            ('type', self.type.to_dict()),
            ('json_type', 'typedef'),
            ('docstring', docstring),
        ])

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
            if self.name and self.parent.name:
                func_docs = get_func_docs(self.parent.name)

                if func_docs is not None:
                    for arg in func_docs.args:
                        if arg.name and arg.name == self.name:
                            docstring = arg.description
                            break
                    else:
                        docstring = None
                else:
                    docstring = None
            else:
                docstring = None

            res = OrderedDict([
                ('name', self.name),
                ('type', self.type.to_dict()),
                ('json_type', 'arg'),
                ('docstring', docstring),
            ])
        else:
            res = OrderedDict([
                ('name', self.name),
                ('type', self.type.to_dict()),
                ('json_type',  str(type(self))),
            ])

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
            name = self.name
            if not name:
                self.name = self.parent.name

            if name:
                union_doc = get_union_docs(name)
                if union_doc:
                    docstring = union_doc.description
                else:
                    docstring = None
            else:
                docstring = None

            res = OrderedDict([
                ('name', name),
                ('type', OrderedDict([('name', 'union'), ('json_type', 'primitive_type')])),
                ('json_type', 'forward_decl'),
                ('docstring', docstring),
            ])
        else:
            if self.name:
                union_doc = get_union_docs(self.name)
            elif self.parent.name:
                union_doc = get_union_docs(self.parent.name)
            else:
                union_doc = None

            if union_doc is not None:
                docstring = union_doc.description
            else:
                docstring = None

            fields = []

            for field in self.decls:
                field = field.to_dict()

                if union_doc is not None:
                    for field_doc in union_doc.fields:
                        if field_doc.name == field['name']:
                            field_docstring = field_doc.description
                            break
                    else:
                        field_docstring = None
                else:
                    field_docstring = None

                field['docstring'] = field_docstring
                field['json_type'] = 'field'
                fields.append(field)

            res = OrderedDict([
                ('name', self.name),
                ('type', OrderedDict([('name', 'union'), ('json_type', 'primitive_type')])),
                ('json_type', 'union'),
                ('docstring', docstring),
                ('fields', fields)
            ])

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
