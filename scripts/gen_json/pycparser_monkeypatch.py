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
            res['quals'].extend(self.dim_quals)
            return res

        res = OrderedDict([
            ('type', self.type.to_dict()),
            ('json_type', 'array'),
            ('dim', dim),
            ('quals', self.dim_quals)
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

                if 'quals' in res:
                    res['quals'].extend(self.quals)
                else:
                    res['quals'] = self.quals

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
                docstring = ''
            else:
                docstring = doc_search.description

            if isinstance(self.type, PtrDecl) and isinstance(self.type.type, FuncDecl):
                type_dict = self.type.type.to_dict()
                type_dict['json_type'] = 'function_pointer'

                if docstring:
                    type_dict['docstring'] = docstring

                if 'quals' in type_dict:
                    type_dict['quals'].extend(self.quals)
                else:
                    type_dict['quals'] = self.quals

                return type_dict

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


member_namespace = {}


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
                docstring = ''
            else:
                docstring = doc_search.description
        else:
            docstring = ''

        members = []
        value_num = 0

        for item in (self.values or []):
            item_dict = item.to_dict()
            try:
                code = generator.visit(item.value)

                try:
                    value = eval("bytearray([b'" + code + "'])[0]")
                except:  # NOQA
                    index = code.find('L')

                    while index >= 1:
                        if code[index - 1].isdigit():
                            code = list(code)
                            code.pop(index)
                            code = ''.join(code)

                        index = code.find('L', index + 1)

                    value = eval(code, member_namespace)

                member_namespace[item_dict['name']] = value

                value_num = value + 1

                code = f'0x{hex(value)[2:].upper()}'
                value = code
            except:  # NOQA
                value = f'0x{hex(value_num)[2:].upper()}'
                member_namespace[item_dict['name']] = value_num
                value_num += 1

            item_dict['value'] = value
            members.append(item_dict)

        hex_len = len(hex(value_num)[2:])
        for member in members:
            member['value'] = f'0x{hex(int(member["value"], 16))[2:].zfill(hex_len).upper()}'

        res = OrderedDict([
            ('name', self.name),
            ('type', OrderedDict([('name', 'int'), ('json_type', 'primitive_type')])),
            ('json_type', 'enum'),
            ('docstring', docstring),
            ('members', members)
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
            docstring = ''
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
    if isinstance(obj, list):
        return type_ == 'typedef'

    return obj['json_type'] == type_


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


_enums = {}
_functions = {}
_structures = {}
_unions = {}
_typedefs = {}
_variables = {}
_function_pointers = {}
_forward_decls = {}


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
        function_pointers = []
        forward_decl = []

        for itm in items:
            itm.process()
            item = itm.to_dict()

            if item is None:
                continue

            if is_type(item, 'typedef'):
                typedefs.append(item)
                _typedefs[itm.name] = item
            elif is_type(item, 'function_pointer'):
                function_pointers.append(item)
                _function_pointers[item['name']] = item
            elif is_type(item, 'function'):
                functions.append(item)
                _functions[item['name']] = item
            elif is_type(item, 'struct'):
                structures.append(item)
                _structures[item['name']] = item
            elif is_type(item, 'union'):
                unions.append(item)
                _unions[item['name']] = item
            elif is_type(item, 'enum'):
                enums.append(item)
                _enums[item['name']] = item
            elif is_type(item, 'variable'):
                variables.append(item)
                _variables[item['name']] = item
            elif is_type(item, 'forward_decl'):
                forward_decl.append(item)
                _forward_decls[item['name']] = item
            else:
                print('UNKNOWN TYPE:')
                print(item)
                print(item.to_dict())

        res = {
            'enums': enums,
            'functions': functions,
            'function_pointers': function_pointers,
            'structures': structures,
            'unions': unions,
            'variables': variables,
            'typedefs': [],
            'forward_decls': forward_decl,
            'macros': []
        }

        for typedef in typedefs:
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
                docstring = ''
                ret_docstring = ''
            else:
                docstring = doc_search.description
                ret_docstring = doc_search.res_description

        else:
            doc_search = None
            docstring = ''
            ret_docstring = ''

        args = []

        for arg in (self.args or []):
            arg = arg.to_dict()
            if arg['name'] and doc_search is not None:
                for doc_arg in doc_search.args:
                    if doc_arg.name == arg['name']:
                        arg['docstring'] = doc_arg.description
                        break
                else:
                    arg['docstring'] = ''
            else:
                arg['docstring'] = ''

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
        elif name.startswith('_lv_') or name.startswith('lv_'):
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
            type_dict = self.type.to_dict()
            type_dict['json_type'] = 'function_pointer'
            res = type_dict
        else:
            res = OrderedDict([
                ('type', self.type.to_dict()),
                ('json_type', 'pointer')
            ])

        if 'quals' in res:
            res['quals'].extend(self.quals)
        else:
            res['quals'] = self.quals

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
                    docstring = ''
            else:
                docstring = ''

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
                docstring = ''

            fields = []

            for field in self.decls:
                field = field.to_dict()

                if struct_doc is not None:
                    for field_doc in struct_doc.fields:
                        if field_doc.name == field['name']:
                            field_docstring = field_doc.description
                            break
                    else:
                        field_docstring = ''
                else:
                    field_docstring = ''

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

        res['quals'] = self.quals

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
            docstring = ''
        else:
            docstring = doc_search.description

        if isinstance(self.type, PtrDecl) and isinstance(self.type.type, FuncDecl):
            type_dict = self.type.type.to_dict()
            type_dict['json_type'] = 'function_pointer'
            type_dict['name'] = self.name
            if 'quals' in type_dict:
                type_dict['quals'].extend(self.quals)
            else:
                type_dict['quals'] = self.quals

            if (
                'docstring' not in type_dict or
                not type_dict['docstring']
            ):
                type_dict['docstring'] = docstring

            return type_dict

        if isinstance(self.type, TypeDecl):
            type_dict = self.type.type.to_dict()

            if 'quals' in type_dict:
                type_dict['quals'].extend(self.quals)
            else:
                type_dict['quals'] = self.quals

            type_dict['quals'].extend(self.type.quals)

            if 'docstring' not in type_dict:
                type_dict['docstring'] = ''

            if not type_dict['docstring']:
                type_dict['docstring'] = docstring

            if type_dict['name'] in _structures:
                _structures[type_dict['name']]['name'] = self.name

                if 'quals' in _structures[type_dict['name']]:
                    _structures[type_dict['name']]['quals'].extend(type_dict['quals'])
                else:
                    _structures[type_dict['name']]['quals'] = type_dict['quals']

                if type_dict['docstring'] and not _structures[type_dict['name']]['docstring']:
                    _structures[type_dict['name']]['docstring'] = type_dict['docstring']

                return None

            if type_dict['name'] in _unions:
                _unions[type_dict['name']]['name'] = self.name

                if 'quals' in _unions[type_dict['name']]:
                    _unions[type_dict['name']]['quals'].extend(type_dict['quals'])
                else:
                    _unions[type_dict['name']]['quals'] = type_dict['quals']

                if type_dict['docstring'] and not _structures[type_dict['name']]['docstring']:
                    _structures[type_dict['name']]['docstring'] = type_dict['docstring']

                return None

            if type_dict['name'] in _enums:
                _enums[type_dict['name']]['name'] = self.name

                if 'quals' in _enums[type_dict['name']]:
                    _enums[type_dict['name']]['quals'].extend(type_dict['quals'])
                else:
                    _enums[type_dict['name']]['quals'] = type_dict['quals']

                if type_dict['docstring'] and not _enums[type_dict['name']]['docstring']:
                    _enums[type_dict['name']]['docstring'] = type_dict['docstring']

                return None

            if not type_dict['name']:
                type_dict['name'] = self.name
                return type_dict

            if type_dict['name'] and type_dict['name'][1:] == self.name:
                type_dict['name'] = self.name
                return type_dict

            if type_dict['name'] and type_dict['name'] == self.name:
                return type_dict

            if isinstance(self.type.type, (Struct, Union)):
                res = OrderedDict([
                    ('name', self.name),
                    ('type', OrderedDict([('name', self.type.type.name), ('json_type', 'lvgl_type')])),
                    ('json_type', 'typedef'),
                    ('docstring', docstring),
                    ('quals', self.quals + self.type.quals)
                ])

                return [res, self.type.type.to_dict()]

            #     if self.type.type.decls:
            #         type_dict = self.type.type.to_dict()
            #         type_dict['name'] = self.name
            #         if not type_dict['docstring']:
            #             type_dict['docstring'] = docstring
            #
            #         return type_dict
            #
            #         if self.type.type.name:
            #             type_dict = self.type.type.to_dict()
            #
            #             if not type_dict['docstring']:
            #                 type_dict['docstring'] = docstring
            #                 docstring = ''
            #
            #             res = OrderedDict([
            #                 ('name', self.name),
            #                 ('type', OrderedDict([('name', type_dict['name']), ('json_type', 'lvgl_type')])),
            #                 ('json_type', 'typedef'),
            #                 ('docstring', docstring),
            #             ])
            #
            #             return type_dict
            #
            #         else:
            #             self.type.type.to_dict()
            #
            #             res = OrderedDict([
            #                 ('name', self.name),
            #                 ('type', ),
            #                 ('json_type', 'typedef'),
            #                 ('docstring', docstring),
            #             ])
            #
            #             return res
            #     else:
            #         res = OrderedDict([
            #             ('name', self.name),
            #             ('type', OrderedDict([('name', self.type.type.name), ('json_type', 'lvgl_type')])),
            #             ('json_type', 'typedef'),
            #             ('docstring', docstring),
            #         ])
            #
            #         return res
            #
            elif isinstance(self.type.type, Enum):
                if self.type.type.name:
                    type_dict = self.type.type.to_dict()

                    if not type_dict['docstring']:
                        type_dict['docstring'] = docstring
                        docstring = ''

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

                    return [res, type_dict]
            #     else:
            #         type_dict = self.type.type.to_dict()
            #
            #         res = OrderedDict([
            #             ('name', self.name),
            #             ('type', type_dict),
            #             ('json_type', 'typedef'),
            #             ('docstring', docstring),
            #         ])
            #
            #         return res
        type_dict = self.type.to_dict()

        if 'quals' in type_dict:
            type_dict['quals'].extend(self.quals)
        else:
            type_dict['quals'] = self.quals

        if docstring and 'docstring' in type_dict and not type_dict['docstring']:
            type_dict['docstring'] = docstring

        if 'name' in type_dict and type_dict['name']:
            if type_dict['name'] == self.name:
                return type_dict
            if type_dict['name'][1:] == self.name:
                type_dict['name'] = self.name
                return type_dict

        quals = type_dict['quals']
        del type_dict['quals']

        res = OrderedDict([
            ('name', self.name),
            ('type', type_dict),
            ('json_type', 'typedef'),
            ('docstring', docstring),
            ('quals', quals)
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
            res['quals'] = self.quals

        elif isinstance(self.parent, FuncDecl):
            if self.name and self.parent.name:
                func_docs = get_func_docs(self.parent.name)

                if func_docs is not None:
                    for arg in func_docs.args:
                        if arg.name and arg.name == self.name:
                            docstring = arg.description
                            break
                    else:
                        docstring = ''
                else:
                    docstring = ''
            else:
                docstring = ''

            res = OrderedDict([
                ('name', self.name),
                ('type', self.type.to_dict()),
                ('json_type', 'arg'),
                ('docstring', docstring),
                ('quals', self.quals)
            ])
        else:
            res = OrderedDict([
                ('name', self.name),
                ('type', self.type.to_dict()),
                ('json_type',  str(type(self))),
                ('quals', self.quals)
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
                    docstring = ''
            else:
                docstring = ''

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
                docstring = ''

            fields = []

            for field in self.decls:
                field = field.to_dict()

                if union_doc is not None:
                    for field_doc in union_doc.fields:
                        if field_doc.name == field['name']:
                            field_docstring = field_doc.description
                            break
                    else:
                        field_docstring = ''
                else:
                    field_docstring = ''

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
