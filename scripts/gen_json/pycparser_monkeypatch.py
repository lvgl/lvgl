# -*- coding: utf-8 -*-

import sys
from pycparser import c_ast  # NOQA
import tempfile

temp_directory = tempfile.mkdtemp(suffix='.lvgl_json')

import doc_builder  # NOQA

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

collected_types = {}


def is_lib_c_node(n):
    if hasattr(n, 'coord') and n.coord is not None:
        if 'fake_libc_include' in n.coord.file:
            return True
    return False


class ArrayDecl(c_ast.ArrayDecl):

    def to_dict(self):
        if is_lib_c_node(self):
            return None

        if self.dim is None:
            dim = None
        else:
            dim = self.dim.to_dict()

        if isinstance(self.type, TypeDecl):
            res = self.type.to_dict()
            res['json_type'] = 'array'
            res['is_const'] = 'const' in self.dim_quals
            res['is_pointer'] = False
            res['dim'] = dim
            return res

        return {
            'name': '',
            'type': self.type.to_dict(),
            'is_const': 'const' in self.dim_quals,
            'dim': dim,
            'json_type': 'array'
        }


class ArrayRef(c_ast.ArrayRef):

    def to_dict(self):
        return None


class Assignment(c_ast.Assignment):

    def to_dict(self):
        return None


class Alignas(c_ast.Alignas):

    def to_dict(self):
        return None


class BinaryOp(c_ast.BinaryOp):

    def to_dict(self):
        return None


class Break(c_ast.Break):

    def to_dict(self):
        return None


class Case(c_ast.Case):

    def to_dict(self):
        return None


class Cast(c_ast.Cast):

    def to_dict(self):
        return None


class Compound(c_ast.Compound):

    def to_dict(self):
        return None


class CompoundLiteral(c_ast.CompoundLiteral):

    def to_dict(self):
        return None


class Constant(c_ast.Constant):

    def to_dict(self):
        return {
            'value': self.value,
            'type': self.type,
            'json_type': 'constant'
        }


class Continue(c_ast.Continue):

    def to_dict(self):
        return None


class Decl(c_ast.Decl):

    def to_dict(self):
        if is_lib_c_node(self):
            return None

        if isinstance(self.type, (Struct, Union)):
            if (
                not self.name and
                not self.type.decls and
                self.type.name and
                self.type.name.startswith('_')
            ):
                return None

            if not self.name and self.type.name:
                res = self.type.to_dict()
                res['is_pointer'] = False
                res['is_const'] = False
                return res

        if isinstance(self.type, FuncDecl):
            res = self.type.to_dict()

            res['name'] = self.name
            return res

        if isinstance(self.type, Enum):
            res = self.type.to_dict()
            res['is_const'] = False
            res['is_pointer'] = False

            return res

        if self.bitsize is None:
            bitsize = {
                "value": None,
                "type": None,
                "json_type": "null_type"
            }
        else:
            bitsize = self.bitsize.to_dict()

        if isinstance(self.type, TypeDecl):
            res = self.type.to_dict()
            if 'extern' in self.storage:
                res['json_type'] = 'variable'

                if 'name' in res and res['name']:
                    docstring = get_var_docs(res['name'])

                    if docstring is not None:
                        docstring = docstring.description
                else:
                    docstring = None

                res['docstring'] = docstring
            else:
                res['bitsize'] = bitsize

            if 'declname' in res:
                res['name'] = res.pop('declname')

            return res

        if (
            isinstance(self.type, PtrDecl) and
            isinstance(self.type.type, FuncDecl)
        ):
            type_dict = self.type.type.to_dict()
            type_dict['is_pointer'] = True
            type_dict['json_type'] = 'function_pointer'
            collected_types[type_dict['name']] = type_dict
            return type_dict

        return {
            'name': self.name,
            'is_const': 'const' in self.quals,
            'is_pointer': False,
            'type': self.type.to_dict(),
            'bitsize': bitsize,
            'json_type': 'decl'
        }


class DeclList(c_ast.DeclList):

    def to_dict(self):
        return None


class Default(c_ast.Default):

    def to_dict(self):
        return None


class DoWhile(c_ast.DoWhile):

    def to_dict(self):
        return None


class EllipsisParam(c_ast.EllipsisParam):

    @property
    def name(self):
        return 'ellipsis-param'

    def to_dict(self):
        return {
            'special_type': 'ellipsis'
        }


class EmptyStatement(c_ast.EmptyStatement):

    def to_dict(self):
        return None


class Enum(c_ast.Enum):

    def to_dict(self):

        if is_lib_c_node(self):
            return None

        if self.values:
            enum_items = self.values.to_dict()

        else:
            enum_items = []

        if self.name:
            docstring = get_enum_docs(self.name)
            if docstring is not None:
                docstring = docstring.description
        else:
            docstring = None

        json_type = {
            'name': self.name,
            'enum_items': enum_items,
            'json_type': 'enum',
            'docstring': docstring
        }

        if self.name is not None:
            collected_types[self.name] = json_type

        return json_type


class Enumerator(c_ast.Enumerator):

    def __init__(self, name, value, coord=None):
        self.name = name
        self.value = value
        self.coord = coord

    def to_dict(self):
        if is_lib_c_node(self):
            return None

        docstring = get_enum_item_docs(self.name)
        if docstring is not None:
            docstring = docstring.description

        return {
            'name': self.name,
            'type': {
                'name': 'int',
                'json_type': 'primitive'
            },
            'json_type': 'enum_item',
            'docstring': docstring
        }


class EnumeratorList(c_ast.EnumeratorList):

    def to_dict(self):
        if self.enumerators:
            return [item.to_dict() for item in self.enumerators]
        else:
            return []


class ExprList(c_ast.ExprList):

    def to_dict(self):
        return None


class FileAST(c_ast.FileAST):

    def to_dict(self):
        res = []
        for item in self:
            json_type = item.to_dict()
            if json_type is None:
                continue

            if isinstance(json_type, list):
                res.extend(json_type)
            else:
                res.append(json_type)

        return res


class For(c_ast.For):

    def to_dict(self):
        return None


class FuncCall(c_ast.FuncCall):

    def to_dict(self):
        return None


class FuncDecl(c_ast.FuncDecl):

    def to_dict(self):
        if is_lib_c_node(self):
            return None

        args = []

        if isinstance(self.type, TypeDecl):
            name = self.type.declname
            type_dict = self.type.to_dict()

        elif (
            isinstance(self.type, PtrDecl) and
            isinstance(self.type.type, PtrDecl) and
            isinstance(self.type.type.type, TypeDecl)
        ):
            type_dict = self.type.type.type.to_dict()
            name = type_dict.pop('declname')
            type_dict['is_pointer'] = True
            type_dict['json_type'] = 'pointer'

            type_dict = {
                'is_const': type_dict['is_const'],
                'is_pointer': True,
                'type': type_dict
            }
            type_dict['type']['is_const'] = False

        elif (
            isinstance(self.type, PtrDecl) and
            isinstance(self.type.type, TypeDecl)
        ):
            name = self.type.type.declname
            type_dict = self.type.to_dict()
            type_dict['type'] = self.type.type.type.to_dict()
        else:
            name = ''
            type_dict = self.type.to_dict()

        if 'declname' in type_dict:
            if type_dict['declname'] and not name:
                name = type_dict['declname']

            del type_dict['declname']

        if 'name' in type_dict:
            if type_dict['name'] and not name:
                name = type_dict['name']

            del type_dict['name']

        type_dict['json_type'] = 'retval'

        if name:
            docreader = get_func_docs(name)
            if docreader is not None:
                docstring = docreader.description
                ret_docstring = docreader.res_description
                docargs = docreader.args

            else:
                ret_docstring = None
                docstring = None
                docargs = None
        else:
            docargs = None
            docstring = None
            ret_docstring = None

        for arg in (self.args or []):
            if isinstance(arg, EllipsisParam) or arg.type is None:
                arg_dict = arg.to_dict()

            elif (
                isinstance(arg.type, ArrayDecl) and
                isinstance(arg.type.type, PtrDecl)

            ):
                arg_dict = arg.to_dict()
                t_dict = arg_dict['type']['type']['type']['type']
                arg_dict['is_pointer'] = arg_dict['type']['type']['is_pointer']
                arg_dict['type']['type'] = t_dict
                arg_dict['type']['json_type'] = 'array'
                del arg_dict['type']['name']

            elif (
                isinstance(arg.type, PtrDecl) and
                isinstance(arg.type.type, TypeDecl) and
                isinstance(arg.type.type.type, (Struct, Union)) and
                not arg.type.type.type.decls and
                arg.type.type.type.name.startswith('_')
            ):
                t_dict = {
                    'name': arg.type.type.type.name[1:],
                    'json_type': 'lv_type'
                }

                arg_dict = arg.type.to_dict()
                arg_dict['type'] = t_dict

            elif (
                isinstance(arg.type, PtrDecl) and
                isinstance(arg.type.type, TypeDecl)
            ):
                arg_dict = arg.type.to_dict()
                arg_dict['type'] = arg.type.type.type.to_dict()

            else:
                arg_dict = arg.type.to_dict()

            arg_dict['json_type'] = 'arg'
            arg_dict['name'] = arg.name

            if 'declname' in arg_dict:
                del arg_dict['declname']

            if 'is_pointer' not in arg_dict:
                arg_dict['is_pointer'] = False

            if arg_dict['name'] and docargs is not None:
                for darg in docargs:
                    if darg.name != arg_dict['name']:
                        continue

                    arg_dict['docstring'] = darg.description
                    break

                else:
                    arg_dict['docstring'] = None
            else:
                arg_dict['docstring'] = None

            args.append(arg_dict)

        type_dict['docstring'] = ret_docstring

        return {
            'name': name,
            'args': args,
            'type': type_dict,
            'json_type': 'function',
            'docstring': docstring
        }


found_quals = set()


class FuncDef(c_ast.FuncDef):

    def to_dict(self):
        if is_lib_c_node(self):
            return None

        return self.decl.to_dict()


class Goto(c_ast.Goto):

    def to_dict(self):
        return None


class ID(c_ast.ID):

    def to_dict(self):
        return self.name


class IdentifierType(c_ast.IdentifierType):

    def to_dict(self):
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


class If(c_ast.If):

    def to_dict(self):
        return None


class InitList(c_ast.InitList):

    def to_dict(self):
        return None


class Label(c_ast.Label):

    def to_dict(self):
        return None


class NamedInitializer(c_ast.NamedInitializer):

    def to_dict(self):
        return None


class ParamList(c_ast.ParamList):

    def to_dict(self):
        return [item.to_dict() for item in self]


class PtrDecl(c_ast.PtrDecl):

    def to_dict(self):
        if is_lib_c_node(self):
            return None

        return {
            'is_pointer': True,
            'is_const': 'const' in self.quals,
            'type': self.type.to_dict(),
            'json_type': 'ptr_decl'
        }


class Return(c_ast.Return):

    def to_dict(self):
        return None


class StaticAssert(c_ast.StaticAssert):

    def to_dict(self):
        return None


class Struct(c_ast.Struct):

    def to_dict(self):
        if is_lib_c_node(self):
            return None

        if self.name:
            docreader = get_struct_docs(self.name)
            if docreader is not None:
                docstring = docreader.description
                docfields = docreader.fields
            else:
                docstring = None
                docfields = None

            if not self.decls:
                if self.name[1:] in collected_types:
                    return {
                        'name': self.name[1:],
                        'is_const': False,
                        'is_pointer': False,
                        'type': 'lv_type',
                        'docstring': docstring

                    }

                return {
                    'name': self.name,
                    'is_const': False,
                    'is_pointer': False,
                    'type': 'lv_type',
                    'docstring': docstring
                }
        else:
            docfields = None
            docstring = None

        fields = []
        for field in (self.decls or []):
            if (
                isinstance(field.type, PtrDecl) and
                isinstance(field.type.type, TypeDecl)
            ):
                field_dict = field.type.type.to_dict()
                field_dict['is_pointer'] = True
                field_dict['is_const'] = (
                    'const' in field.type.quals or field_dict['is_const']
                )

                if 'declname' in field_dict:
                    field_dict['name'] = field_dict.pop('declname')

            elif (
                isinstance(field.type, PtrDecl) and
                isinstance(field.type.type, PtrDecl) and
                isinstance(field.type.type.type, TypeDecl)

            ):
                field_dict = field.type.type.type.to_dict()
                field_dict['is_pointer'] = True
                field_dict['json_type'] = 'pointer'

                if 'declname' in field_dict:
                    name = field_dict.pop('declname')
                else:
                    name = field_dict.pop('name')

                if field.bitsize:
                    bitsize = field.bitsize.to_dict()
                else:
                    bitsize = {
                        "value": None,
                        "type": None,
                        "json_type": "null_type"
                    }

                field_dict = {
                    'name': name,
                    'is_pointer': True,
                    'bitsize': bitsize,
                    'is_const': (
                        'const' in field.type.quals or field_dict['is_const']
                    ),
                    'type': field_dict,
                }
            else:
                field_dict = field.to_dict()

            field_dict['json_type'] = 'field'

            if isinstance(field.type, ArrayDecl):
                if 'type' in field_dict:
                    if 'declname' in field_dict['type']:
                        del field_dict['type']['declname']

                    if 'name' in field_dict['type']:
                        del field_dict['type']['name']

            if docfields:
                for dfield in docfields:
                    if dfield.name != field_dict['name']:
                        continue

                    field_dict['docstring'] = dfield.description
                    break
                else:
                    field_dict['docstring'] = None
            else:
                field_dict['docstring'] = None

            fields.append(field_dict)

        res = {
            'name': self.name,
            'fields': fields,
            'json_type': 'struct',
            'docstring': docstring
        }

        if fields and self.name and self.name not in collected_types:
            collected_types[self.name] = res

        return res


class StructRef(c_ast.StructRef):

    def to_dict(self):
        return None


class Switch(c_ast.Switch):

    def to_dict(self):
        return None


class TernaryOp(c_ast.TernaryOp):

    def to_dict(self):
        return None


class TypeDecl(c_ast.TypeDecl):

    def to_dict(self):
        if is_lib_c_node(self):
            return None

        if isinstance(self.type, (Struct, Union)):
            if self.declname and not self.type.name:
                res = self.type.to_dict()
                res['name'] = self.declname

                if isinstance(self.type, Struct):
                    t_docs = get_struct_docs(self.declname)
                else:
                    t_docs = get_union_docs(self.declname)

                if t_docs:
                    res['docstring'] = t_docs.description
                    fdocs = t_docs.fields
                else:
                    res['docstring'] = None
                    fdocs = None

                for field in res['fields']:
                    if 'name' in field and field['name'] and fdocs:
                        for fdoc in fdocs:
                            if fdoc.name != field['name']:
                                continue

                            field['docstring'] = fdoc.description
                            break
                        else:
                            field['docstring'] = None
                    else:
                        field['docstring'] = None

                if self.type.decls and self.declname not in collected_types:
                    collected_types[self.declname] = res

                return res

            if (
                self.type.name and
                not self.type.decls and
                self.type.name.startswith('_')
            ):
                type_dict = self.type.to_dict()

                if 'fields' not in type_dict:
                    return type_dict

                type_dict['name'] = self.type.name[1:]
                type_dict['json_type'] = 'lv_type'

                del type_dict['fields']

                res = {
                    'name': self.declname,
                    'is_const': 'const' in self.quals,
                    'is_pointer': False,
                    'type': type_dict,
                    'json_type': 'type_decl1'
                }

                return res

        res = {
            'declname': self.declname,
            'is_const': 'const' in self.quals,
            'is_pointer': False,
            'type': self.type.to_dict(),
            'json_type': 'type_decl2'
        }

        return res


class Typedef(c_ast.Typedef):

    def to_dict(self):
        if is_lib_c_node(self):
            return None

        if (
            isinstance(self.type, TypeDecl) and
            isinstance(self.type.type, (Struct, Union))
        ):
            # if not self.type.type.name and self.type.type.decls:
            #     if isinstance(self.type.type, Struct):
            #         t_docs = get_struct_docs(self.name)
            #     else:
            #         t_docs = get_union_docs(self.name)
            #
            #     if t_docs:
            #         docstring = t_docs.description
            #         fdocs = t_docs.fields
            #     else:
            #         docstring = None
            #         fdocs = None
            # else:
            #     docstring = None
            #     fdocs = None

            res1 = self.type.to_dict()

            # if docstring is None:
            #     t_docs = get_typedef_docs(self.name)
            #     if t_docs:
            #         docstring = t_docs.description
            #     else:
            #         docstring = None
            #
            # if 'docstring' not in res1 or not res1['docstring']:
            #     res1['docstring'] = docstring

            # for field in res1['fields']:
            #     if 'name' in field and field['name'] and fdocs:
            #         for fdoc in fdocs:
            #             if fdoc.name != field['name']:
            #                 continue
            #
            #             field['docstring'] = fdoc.description
            #             break
            #         else:
            #             field['docstring'] = None
            #
            #     else:
            #         field['docstring'] = None

            if self.type.declname:
                res1['name'] = self.type.declname
            elif self.name:
                res1['name'] = self.name

                collected_types[self.name] = res1
                return res1

            res2 = {
                'name': self.name,
                'is_const': 'const' in self.quals,
                'is_pointer': False,
                'type': {
                    'name': res1['name'],
                    'json_type': 'lv_type'
                },
                'json_type': 'typedef'
            }

            collected_types[self.name] = res2

            if self.type.type.decls:
                if res1['name'] not in collected_types:
                    collected_types[res1['name']] = res1
                return [res1, res2]

            return res2

        if (
            self.name and
            isinstance(self.type, TypeDecl)
        ):
            if (
                self.name and
                self.type.declname
            ):
                if self.name == self.type.declname:
                    res = self.type.to_dict()
                    res['is_pointer'] = False

                    if 'declname' in res:
                        del res['declname']

                    res['name'] = self.name
                    res['json_type'] = 'typedef'

                    col_name = f'_{res["name"]}'

                    if res['name'] and col_name in collected_types:
                        if collected_types[col_name]['json_type'] == 'enum':
                            for enum_item in collected_types[col_name]['enum_items']:
                                enum_item['type']['name'] = res['name']
                                enum_item['type']['json_type'] = 'lv_type'

                    collected_types[res['name']] = res

                    t_docs = get_typedef_docs(self.name)
                    if t_docs:
                        docstring = t_docs.description
                    else:
                        docstring = None

                    if 'docstring' not in res or not res['docstring']:
                        res['docstring'] = docstring

                    return res
                else:
                    res1 = self.type.to_dict()
                    res1['is_pointer'] = False
                    if 'declname' in res1:
                        res1['name'] = res1['declname']
                        del res1['declname']

                    collected_types[res1['name']] = res1

                    t_docs = get_typedef_docs(self.name)
                    if t_docs:
                        docstring = t_docs.description
                    else:
                        docstring = None

                    if 'docstring' not in res1 or not res1['docstring']:
                        res1['docstring'] = docstring

                    res1['json_type'] = 'typedecl'

                    res2 = {
                        'name': self.name,
                        'type': {
                            'name': self.type.declname,
                            'json_type': 'lv_type'
                        },
                        'is_pointer': False,
                        'is_const': False,
                        'json_type': 'typedef'
                    }
                    collected_types[res2['name']] = res2

                    return [res1, res2]
        if (
            isinstance(self.type, PtrDecl) and
            isinstance(self.type.type, TypeDecl)
        ):
            if (
                self.name and
                self.type.type.declname
            ):
                if self.name == self.type.type.declname:
                    res = self.type.type.to_dict()
                    res['is_pointer'] = True
                    del res['declname']
                    res['name'] = self.name
                    res['json_type'] = 'typedef'

                    collected_types[res['name']] = res

                    t_docs = get_typedef_docs(self.name)
                    if t_docs:
                        docstring = t_docs.description
                    else:
                        docstring = None

                    if 'docstring' not in res or not res['docstring']:
                        res['docstring'] = docstring

                    return res
                else:
                    res1 = self.type.type.to_dict()
                    res1['is_pointer'] = True
                    res1['name'] = res1['declname']
                    del res1['declname']
                    res1['json_type'] = 'typedecl'

                    collected_types[res1['name']] = res1

                    t_docs = get_typedef_docs(self.name)
                    if t_docs:
                        docstring = t_docs.description
                    else:
                        docstring = None

                    if 'docstring' not in res1 or not res1['docstring']:
                        res1['docstring'] = docstring

                    res2 = {
                        'name': self.name,
                        'type': {
                            'name': self.type.type.declname,
                            'json_type': 'lv_type'
                        },
                        'is_pointer': False,
                        'is_const': False,
                        'json_type': 'typedef'
                    }

                    collected_types[res2['name']] = res2

                    return [res1, res2]

        if (
            isinstance(self.type, PtrDecl) and
            isinstance(self.type.type, FuncDecl)
        ):
            res = self.type.type.to_dict()
            res['is_pointer'] = True
            res['is_const'] = 'const' in self.quals
            res['json_type'] = 'function_pointer'

            collected_types[res['name']] = res

            t_docs = get_typedef_docs(self.name)
            if t_docs:
                docstring = t_docs.description
            else:
                docstring = None

            if 'docstring' not in res or not res['docstring']:
                res['docstring'] = docstring

            return res

        t_docs = get_typedef_docs(self.name)
        if t_docs:
            docstring = t_docs.description
        else:
            docstring = None

        res = {
            'name': self.name,
            'is_const': 'const' in self.quals,
            'is_pointer': False,
            'type': self.type.to_dict(),
            'json_type': 'typedef',
            'docstring': docstring
        }

        col_name = f'_{res["name"]}'

        if res['name'] and col_name in collected_types:
            if collected_types[col_name]['json_type'] == 'enum':
                for enum_item in collected_types[col_name]['enum_items']:
                    enum_item['type']['name'] = res['name']
                    enum_item['type']['json_type'] = 'lv_type'

        if self.name:
            collected_types[self.name] = res

        return res


class Typename(c_ast.Typename):

    def to_dict(self):
        if is_lib_c_node(self):
            return None

        for item in (self.quals or []):
            found_quals.add(item)

        return {
            'name': self.name,
            'is_const': 'const' in self.quals,
            'type': self.type.to_dict(),
            'json_type': 'typename'
        }


class UnaryOp(c_ast.UnaryOp):

    def to_dict(self):
        return None


class Union(c_ast.Union):

    def to_dict(self):
        if is_lib_c_node(self):
            return None

        if self.name:
            docreader = get_union_docs(self.name)
            if docreader is not None:
                docstring = docreader.description
                docfields = docreader.fields
            else:
                docstring = None
                docfields = None

            if not self.decls:
                if self.name[1:] in collected_types:
                    return {
                        'name': self.name[1:],
                        'type': 'lv_type',
                        'docstring': docstring

                    }

                return {
                    'name': self.name,
                    'type': 'lv_type',
                    'docstring': docstring
                }
        else:
            docfields = None
            docstring = None

        fields = []
        for field in (self.decls or []):
            if (
                isinstance(field.type, PtrDecl) and
                isinstance(field.type.type, TypeDecl)
            ):
                field_dict = field.type.type.to_dict()
                field_dict['is_pointer'] = True
                field_dict['is_const'] = (
                    'const' in field.type.quals or field_dict['is_const']
                )

                if 'declname' in field_dict:
                    field_dict['name'] = field_dict.pop('declname')

            elif (
                isinstance(field.type, PtrDecl) and
                isinstance(field.type.type, PtrDecl) and
                isinstance(field.type.type.type, TypeDecl)

            ):
                field_dict = field.type.type.type.to_dict()
                field_dict['is_pointer'] = True
                field_dict['json_type'] = 'pointer'

                if 'declname' in field_dict:
                    name = field_dict.pop('declname')
                else:
                    name = field_dict.pop('name')

                if field.bitsize:
                    bitsize = field.bitsize.to_dict()
                else:
                    bitsize = {
                        "value": None,
                        "type": None,
                        "json_type": "null_type"
                    }

                field_dict = {
                    'name': name,
                    'is_pointer': True,
                    'bitsize': bitsize,
                    'is_const': (
                        'const' in field.type.quals or field_dict['is_const']
                    ),
                    'type': field_dict,
                }
            else:
                field_dict = field.to_dict()

            field_dict['json_type'] = 'field'

            if isinstance(field.type, ArrayDecl):
                if 'type' in field_dict:
                    if 'declname' in field_dict['type']:
                        del field_dict['type']['declname']

                    if 'name' in field_dict['type']:
                        del field_dict['type']['name']

            if 'name' in field_dict and field_dict['name'] and docfields:
                for dfield in docfields:
                    if dfield.name != field_dict['name']:
                        continue

                    field_dict['docstring'] = dfield.description
                    break
                else:
                    field_dict['docstring'] = None
            else:
                field_dict['docstring'] = None

            fields.append(field_dict)

        res = {
            'name': self.name,
            'fields': fields,
            'json_type': 'union',
            'docstring': docstring
        }

        if fields and self.name and self.name not in collected_types:
            collected_types[self.name] = res

        return res


class While(c_ast.While):

    def to_dict(self):
        return None


class Pragma(c_ast.Pragma):

    def to_dict(self):
        return None


for cls in (
    ArrayDecl,
    ArrayRef,
    Assignment,
    Alignas,
    BinaryOp,
    Break,
    Case,
    Cast,
    Compound,
    CompoundLiteral,
    Constant,
    Continue,
    Decl,
    DeclList,
    Default,
    DoWhile,
    EllipsisParam,
    EmptyStatement,
    Enum,
    Enumerator,
    EnumeratorList,
    ExprList,
    EnumeratorList,
    ExprList,
    FileAST,
    For,
    FuncCall,
    FuncDecl,
    FuncDef,
    Goto,
    ID,
    IdentifierType,
    If,
    InitList,
    Label,
    NamedInitializer,
    ParamList,
    PtrDecl,
    Return,
    StaticAssert,
    Struct,
    StructRef,
    Switch,
    TernaryOp,
    TypeDecl,
    Typedef,
    Typename,
    UnaryOp,
    Union,
    While,
    Pragma
):
    cls_name = cls.__name__
    setattr(getattr(sys.modules['pycparser.c_parser'], 'c_ast'), cls_name, cls)
