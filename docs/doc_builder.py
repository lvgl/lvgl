import os
from xml.etree import ElementTree as ET

base_path = ''
xml_path = ''


def load_xml(fle):
    fle = os.path.join(xml_path, fle + '.xml')

    with open(fle, 'rb') as f:
        d = f.read().decode('utf-8')

    # This code is to correct a bug in Doxygen. That bug incorrectly parses
    # a typedef and it causes an error to occur building the docs. The Error
    # doesn't stop the documentation from being generated, I just don't want
    # to see the ugly red output.
    #
    # if 'typedef void() lv_lru_free_t(void *v)' in d:
    #     d = d.replace(
    #         '<type>void()</type>\n        '
    #         '<definition>typedef void() lv_lru_free_t(void *v)</definition>',
    #         '<type>void</type>\n        '
    #         '<definition>typedef void(lv_lru_free_t)(void *v)</definition>'
    #     )
    #     with open(fle, 'wb') as f:
    #         f.write(d.encode('utf-8'))

    return ET.fromstring(d)


structures = {}
functions = {}
enums = {}
typedefs = {}
variables = {}
unions = {}
namespaces = {}
files = {}


class STRUCT(object):
    template = '''\
.. doxygenstruct:: {name}
   :project: lvgl
   :members:
   :protected-members:
   :private-members:
   :undoc-members:
'''

    def __init__(self, parent, refid, name, **_):
        if name in structures:
            self.__dict__.update(structures[name].__dict__)
            return

        structures[name] = self
        self.parent = parent
        self.refid = refid
        self.name = name
        self.types = set()
        self._deps = None
        self.header_file = ''

        root = load_xml(refid)

        for compounddef in root:
            if compounddef.attrib['id'] != self.refid:
                continue

            for child in compounddef:
                if child.tag == 'includes':
                    self.header_file = os.path.splitext(child.text)[0]

                if child.tag != 'sectiondef':
                    continue

                for memberdef in child:
                    t = get_type(memberdef)

                    if t is None:
                        continue

                    self.types.add(t)

    @property
    def deps(self):
        if self._deps is None:
            self._deps = dict(
                typedefs=set(),
                functions=set(),
                enums=set(),
                structures=set(),
                unions=set(),
                namespaces=set(),
                variables=set(),
            )
            for type_ in self.types:
                if type_ in typedefs:
                    self._deps['typedefs'].add(typedefs[type_])
                elif type_ in structures:
                    self._deps['structures'].add(structures[type_])
                elif type_ in unions:
                    self._deps['unions'].add(unions[type_])
                elif type_ in enums:
                    self._deps['enums'].add(enums[type_])
                elif type_ in functions:
                    self._deps['functions'].add(functions[type_])
                elif type_ in variables:
                    self._deps['variables'].add(variables[type_])
                elif type_ in namespaces:
                    self._deps['namespaces'].add(namespaces[type_])
        return self._deps

    def __str__(self):
        return self.template.format(name=self.name)


class UNION(STRUCT):
    template = '''\
.. doxygenunion:: {name}
   :project: lvgl
'''


def get_type(node):
    def gt(n):
        for c in n:
            if c.tag == 'ref':
                t = c.text.strip()
                break
        else:
            t = node.text.strip()

        return t.replace('*', '').replace('(', '').replace(')', '').strip()

    for child in node:
        if child.tag == 'type':
            return gt(child)


class VARIABLE(object):
    template = '''\
.. doxygenvariable:: {name}
   :project: lvgl
'''

    def __init__(self, parent, refid, name, **_):
        if name in variables:
            self.__dict__.update(variables[name].__dict__)
            return

        variables[name] = self
        self.parent = parent
        self.refid = refid
        self.name = name

    def __str__(self):
        return self.template.format(name=self.name)


class NAMESPACE(object):
    template = '''\
.. doxygennamespace:: {name}
   :project: lvgl
   :members:
   :protected-members:
   :private-members:
   :undoc-members:
'''

    def __init__(self, parent, refid, name, **_):
        if name in namespaces:
            self.__dict__.update(namespaces[name].__dict__)
            return

        namespaces[name] = self
        self.parent = parent
        self.refid = refid
        self.name = name

    def __str__(self):
        return self.template.format(name=self.name)


class FUNCTION(object):
    template = '''\
.. doxygenfunction:: {name}
   :project: lvgl
'''

    def __init__(self, parent, refid, name, **_):
        if name in functions:
            self.__dict__.update(functions[name].__dict__)
            return

        functions[name] = self
        self.parent = parent
        self.refid = refid
        self.name = name
        self.types = set()
        self.restype = None
        self._deps = None

        if parent is not None:
            root = load_xml(parent.refid)

            for compounddef in root:
                if compounddef.attrib['id'] != parent.refid:
                    continue

                for child in compounddef:
                    if child.tag != 'sectiondef':
                        continue
                    if child.attrib['kind'] != 'func':
                        continue

                    for memberdef in child:
                        if memberdef.attrib['id'] == refid:
                            break
                    else:
                        continue

                    break
                else:
                    continue

                break
            else:
                return

            self.restype = get_type(memberdef)

            for child in memberdef:
                if child.tag == 'param':
                    t = get_type(child)
                    if t is not None:
                        self.types.add(t)

        if self.restype in self.types:
            self.restype = None

    @property
    def deps(self):
        if self._deps is None:
            self._deps = dict(
                typedefs=set(),
                functions=set(),
                enums=set(),
                structures=set(),
                unions=set(),
                namespaces=set(),
                variables=set(),
            )
            if self.restype is not None:
                self.types.add(self.restype)

            for type_ in self.types:
                if type_ in typedefs:
                    self._deps['typedefs'].add(typedefs[type_])
                elif type_ in structures:
                    self._deps['structures'].add(structures[type_])
                elif type_ in unions:
                    self._deps['unions'].add(unions[type_])
                elif type_ in enums:
                    self._deps['enums'].add(enums[type_])
                elif type_ in functions:
                    self._deps['functions'].add(functions[type_])
                elif type_ in variables:
                    self._deps['variables'].add(variables[type_])
                elif type_ in namespaces:
                    self._deps['namespaces'].add(namespaces[type_])
        return self._deps

    def __str__(self):
        return self.template.format(name=self.name)


class FILE(object):
    def __init__(self, _, refid, name, node, **__):
        if name in files:
            self.__dict__.update(files[name].__dict__)
            return

        files[name] = self

        self.refid = refid
        self.name = name
        self.header_file = os.path.splitext(name)[0]

        enums_ = []

        for member in node:
            if member.tag != 'member':
                continue

            cls = globals()[member.attrib['kind'].upper()]
            if cls == ENUM:
                member.attrib['name'] = member[0].text.strip()
                enums_.append(cls(self, **member.attrib))
            elif cls == ENUMVALUE:
                if enums_[-1].is_member(member):
                    enums_[-1].add_member(member)

            else:
                member.attrib['name'] = member[0].text.strip()
                cls(self, **member.attrib)


class ENUM(object):
    template = '''\
.. doxygenenum:: {name}
   :project: lvgl
'''

    def __init__(self, parent, refid, name, **_):
        if name in enums:
            self.__dict__.update(enums[name].__dict__)
            return

        enums[name] = self

        self.parent = parent
        self.refid = refid
        self.name = name
        self.members = []

    def is_member(self, member):
        return (
            member.attrib['kind'] == 'enumvalue' and
            member.attrib['refid'].startswith(self.refid)
        )

    def add_member(self, member):
        self.members.append(
            ENUMVALUE(
                self,
                member.attrib['refid'],
                member[0].text.strip()
            )
        )

    def __str__(self):
        template = [self.template.format(name=self.name)]
        template.extend(list(str(member) for member in self.members))

        return '\n'.join(template)


defines = {}


class DEFINE(object):
    template = '''\
.. doxygendefine:: {name}
   :project: lvgl
'''

    def __init__(self, parent, refid, name, **_):
        if name in defines:
            self.__dict__.update(defines[name].__dict__)
            return

        defines[name] = self

        self.parent = parent
        self.refid = refid
        self.name = name

    def __str__(self):
        return self.template.format(name=self.name)


class ENUMVALUE(object):
    template = '''\
.. doxygenenumvalue:: {name}
   :project: lvgl
'''

    def __init__(self, parent, refid, name, **_):
        self.parent = parent
        self.refid = refid
        self.name = name

    def __str__(self):
        return self.template.format(name=self.name)


class TYPEDEF(object):
    template = '''\
.. doxygentypedef:: {name}
   :project: lvgl
'''

    def __init__(self, parent, refid, name, **_):
        if name in typedefs:
            self.__dict__.update(typedefs[name].__dict__)
            return

        typedefs[name] = self

        self.parent = parent
        self.refid = refid
        self.name = name
        self.type = None
        self._deps = None

        if parent is not None:
            root = load_xml(parent.refid)

            for compounddef in root:
                if compounddef.attrib['id'] != parent.refid:
                    continue

                for child in compounddef:
                    if child.tag != 'sectiondef':
                        continue
                    if child.attrib['kind'] != 'typedef':
                        continue

                    for memberdef in child:
                        if memberdef.attrib['id'] == refid:
                            break
                    else:
                        continue

                    break
                else:
                    continue

                break
            else:
                return

            self.type = get_type(memberdef)

    @property
    def deps(self):
        if self._deps is None:
            self._deps = dict(
                typedefs=set(),
                functions=set(),
                enums=set(),
                structures=set(),
                unions=set(),
                namespaces=set(),
                variables=set(),
            )
            if self.type is not None:
                type_ = self.type

                if type_ in typedefs:
                    self._deps['typedefs'].add(typedefs[type_])
                elif type_ in structures:
                    self._deps['structures'].add(structures[type_])
                elif type_ in unions:
                    self._deps['unions'].add(unions[type_])
                elif type_ in enums:
                    self._deps['enums'].add(enums[type_])
                elif type_ in functions:
                    self._deps['functions'].add(functions[type_])
                elif type_ in variables:
                    self._deps['variables'].add(variables[type_])
                elif type_ in namespaces:
                    self._deps['namespaces'].add(namespaces[type_])

        return self._deps

    def __str__(self):
        return self.template.format(name=self.name)


classes = {}


class CLASS(object):

    def __init__(self, _, refid, name, node, **__):
        if name in classes:
            self.__dict__.update(classes[name].__dict__)
            return

        classes[name] = self

        self.refid = refid
        self.name = name

        enums_ = []

        for member in node:
            if member.tag != 'member':
                continue

            cls = globals()[member.attrib['kind'].upper()]
            if cls == ENUM:
                member.attrib['name'] = member[0].text.strip()
                enums_.append(cls(self, **member.attrib))
            elif cls == ENUMVALUE:
                if enums_[-1].is_member(member):
                    enums_[-1].add_member(member)

            else:
                member.attrib['name'] = member[0].text.strip()
                cls(self, **member.attrib)


lvgl_src_path = ''
api_path = ''
html_files = {}


def iter_src(n, p):
    if p:
        out_path = os.path.join(api_path, p)
    else:
        out_path = api_path

    index_file = None

    if p:
        src_path = os.path.join(lvgl_src_path, p)
    else:
        src_path = lvgl_src_path

    folders = []

    for file in os.listdir(src_path):
        if 'private' in file:
            continue

        if os.path.isdir(os.path.join(src_path, file)):
            folders.append((file, os.path.join(p, file)))
            continue

        if not file.endswith('.h'):
            continue

        if not os.path.exists(out_path):
            os.makedirs(out_path)

        if index_file is None:
            index_file = open(os.path.join(out_path, 'index.rst'), 'w')
            if n:
                index_file.write('=' * len(n))
                index_file.write('\n' + n + '\n')
                index_file.write('=' * len(n))
                index_file.write('\n\n\n')

            index_file.write('.. toctree::\n    :maxdepth: 2\n\n')

        name = os.path.splitext(file)[0]
        index_file.write('    ' + name + '\n')

        rst_file = os.path.join(out_path, name + '.rst')
        html_file = os.path.join(p, name + '.html')
        html_files[name] = html_file

        with open(rst_file, 'w') as f:
            f.write('.. _{0}:'.format(name))
            f.write('\n\n')
            f.write('=' * len(file))
            f.write('\n')
            f.write(file)
            f.write('\n')
            f.write('=' * len(file))
            f.write('\n\n\n')

            f.write('.. doxygenfile:: ' + file)
            f.write('\n')
            f.write('    :project: lvgl')
            f.write('\n\n')

    for name, folder in folders:
        if iter_src(name, folder):
            if index_file is None:
                index_file = open(os.path.join(out_path, 'index.rst'), 'w')

                if n:
                    index_file.write('=' * len(n))
                    index_file.write('\n' + n + '\n')
                    index_file.write('=' * len(n))
                    index_file.write('\n\n\n')

                index_file.write('.. toctree::\n    :maxdepth: 2\n\n')

            index_file.write('    ' + os.path.split(folder)[-1] + '/index\n')

    if index_file is not None:
        index_file.write('\n')
        index_file.close()
        return True

    return False


def clean_name(nme):
    if nme.startswith('_lv_'):
        nme = nme[4:]
    elif nme.startswith('lv_'):
        nme = nme[3:]

    if nme.endswith('_t'):
        nme = nme[:-2]

    return nme


def is_name_match(item_name, obj_name):
    u_num = item_name.count('_') + 1

    obj_name = obj_name.split('_')
    if len(obj_name) < u_num:
        return False
    obj_name = '_'.join(obj_name[:u_num])

    return item_name == obj_name


def get_includes(name1, name2, obj, includes):
    name2 = clean_name(name2)

    if not is_name_match(name1, name2):
        return

    if obj.parent is not None:
        header_file = obj.parent.header_file
    elif hasattr(obj, 'header_file'):
        header_file = obj.header_file
    else:
        return

    if not header_file:
        return

    if header_file not in html_files:
        return

    includes.add((header_file, html_files[header_file]))


def run(project_path, temp_directory, *doc_paths):
    global base_path
    global xml_path
    global lvgl_src_path
    global api_path
    
    base_path = temp_directory
    xml_path = os.path.join(base_path, 'xml')
    api_path = os.path.join(base_path, 'API')
    lvgl_src_path = os.path.join(project_path, 'src')

    if not os.path.exists(api_path):
        os.makedirs(api_path)
    
    iter_src('API', '')
    index = load_xml('index')

    for compound in index:
        compound.attrib['name'] = compound[0].text.strip()
        if compound.attrib['kind'] in ('example', 'page', 'dir'):
            continue

        globals()[compound.attrib['kind'].upper()](
            None,
            node=compound,
            **compound.attrib
        )

    for folder in doc_paths:
        items = list(
            (os.path.splitext(item)[0], os.path.join(folder, item))
            for item in os.listdir(folder)
            if item.endswith('rst') and 'index' not in item
        )

        for name, path in items:
            html_includes = set()

            for container in (
                defines,
                enums,
                variables,
                namespaces,
                structures,
                unions,
                typedefs,
                functions
            ):
                for n, o in container.items():
                    get_includes(name, n, o, html_includes)

            if html_includes:
                html_includes = list(
                    ':ref:`{0}`\n'.format(inc)
                    for inc, _ in html_includes
                )

                output = ('\n'.join(html_includes)) + '\n'

                with open(path, 'rb') as f:
                    try:
                        data = f.read().decode('utf-8')
                    except UnicodeDecodeError:
                        print(path)
                        raise

                data = data.split('.. Autogenerated', 1)[0]

                data += '.. Autogenerated\n\n'
                data += output

                with open(path, 'wb') as f:
                    f.write(data.encode('utf-8'))
