#!/usr/bin/env python

import gzip
import json
import os
import zlib
from argparse import ArgumentParser
from fnmatch import fnmatch
from glob import glob
from sys import stderr
from zlib import crc32

import format
import yaml

try:
    import heatshrink2
except:
    heatshrink2 = None

from frogfs import align, djb2_hash, expand_variables, pad, pipe_script

COMP_ALGO_ZLIB = 1
COMP_ALGO_HEATSHRINK = 2
COMP_ALGO_GZIP = 3


def load_config() -> dict:
    '''Loads and normalizes configuration yaml file'''
    with open(config_file, 'r') as f:
        doc = yaml.safe_load(f)

    config = {'define': {}, 'collect': {}, 'filter': []}

    def add_define(name, value):
        value = expand_variables(value, config['define'])
        config['define'][name] = value

    def add_collect(pattern, destination):
        pattern = expand_variables(pattern, config['define'])
        destination = expand_variables(destination or '', config['define'])
        config['collect'][pattern] = destination

    def add_filter(pattern, actions):
        pattern = expand_variables(pattern, config['define'])
        normalized_actions = []
        for action in actions:
            if isinstance(action, dict):
                action = tuple(next(iter(action.items())))
            if isinstance(action, str):
                action = (action, {})
            normalized_actions.append(action)
        config['filter'].append((pattern, normalized_actions))

    add_define('cwd', os.getcwd())
    add_define('frogfs', os.path.abspath(frogfs_dir))

    defines = doc.get('define', {})
    if isinstance(defines, list):
        for define in defines:
            for name, value in define.items():
                add_define(name, value)
    elif isinstance(defines, dict):
        for name, value in defines.items():
            add_define(name, value)
    else:
        raise Exception('unexpected type for define')

    collects = doc.get('collect', {})
    if isinstance(collects, str):
        add_collect(collects, '')
    elif isinstance(collects, list):
        for collect in collects:
            if isinstance(collect, str):
                add_collect(collect, '')
            else:
                for source, destination in collect.items():
                    add_collect(source, destination)
    elif isinstance(collects, dict):
        for source, destination in collects.items():
            add_collect(source, destination)
    else:
        raise Exception('unexpected type for collect')

    filters = doc.get('filter', {})
    if isinstance(filters, list):
        for filter in filters:
            for pattern, actions in filter.items():
                add_filter(pattern, actions)
    elif isinstance(filters, dict):
        for pattern, actions in filters.items():
            add_filter(pattern, actions)
    else:
        raise Exception('unexpected type for filter')

    return config

def collect_entries() -> dict:
    '''Collects all the path entries'''

    entries = {}
    def add_file(src, dest):
        src = os.path.abspath(src)
        if not os.path.isfile(src):
            raise Exception(f'"{src}" is not a file')

        if dest == '':
            dest = os.path.basename(src)
        dest = os.path.join(os.sep, dest).replace('\\', '/').lstrip('/')
        ent = {
            'type': 'file',
            'name': os.path.basename(dest.rstrip('/')),
            'path': src,
            'dest': dest,
            'skip': True,
        }
        entries[dest] = ent

    for pattern, dest in config['collect'].items():
        base, name = os.path.split(pattern)
        base = os.path.normpath(base)

        if '*' in name or '?' in name:
            try:
                old_cwd = os.getcwd()
                os.chdir(base)
                paths = glob(name)
                os.chdir(old_cwd)
            except:
                paths = ()

            for path in paths:
                src = os.path.join(base, path)
                if os.path.isdir(src):
                    for dir, _, files in os.walk(src, followlinks=True):
                        reldir = dir[len(base) + 1:]
                        for file in files:
                            add_file(os.path.join(dir, file),
                                    os.path.join(dest, reldir, file))
                else:
                    file = src[len(base) + 1:]
                    add_file(src, os.path.join(dest, file))

        elif os.path.isdir(pattern):
            for dir, _, files in os.walk(os.path.join(base, name),
                        followlinks=True):
                reldir = dir[len(base) + 1:]
                for file in files:
                    add_file(os.path.join(dir, file),
                            os.path.join(dest, reldir, file))

        else:
            if dest.endswith('/'):
                dest = os.path.join(dest, name)
            add_file(pattern, dest)

    for dest in entries.copy().keys():
        while True:
            dest = os.path.dirname(dest)
            entries[dest] = {
                'type': 'dir',
                'name': os.path.basename(dest),
                'dest': dest,
                'skip': True,
            }
            if not dest.rstrip('/'):
                break

    return dict(sorted(entries.items()))

def load_transforms() -> dict:
    '''Find transforms in frogfs' tools directory and in ${CWD}/tools'''
    xforms = {}
    for dir in tools_dirs:
        if not os.path.exists(dir):
            continue

        for file in os.listdir(dir):
            if file.startswith('transform-'):
                name, _ = os.path.splitext(file[10:]) # strip off 'transform-'
                path = os.path.join(dir, file)
                xforms[name] = {'path': path}
            if file.startswith('meta-transform-'):
                name, _ = os.path.splitext(file[15:]) # strip off 'meta-transform-'
                path = os.path.join(dir, file)
                xforms[name] = {'path': path, 'meta':True}
    return xforms

### Stage 1 ###

def clean_removed() -> None:
    '''Check if any files have been removed'''
    global dirty

    removed = False
    for dir, _, files in os.walk(cache_dir, topdown=False, followlinks=True):
        reldir = os.path.relpath(dir, cache_dir).replace('\\', '/')
        if reldir == '.' or reldir.startswith('./'):
            reldir = reldir[2:]
        for file in files:
            relfile = os.path.join(reldir, file).replace('\\', '/')
            if relfile not in entries.keys():
                os.unlink(os.path.join(cache_dir, relfile))
                removed = True
        if reldir not in entries.keys():
            os.rmdir(os.path.join(cache_dir, reldir))
            removed = True

    dirty |= removed

def load_state() -> None:
    '''Loads previous state or initializes a empty one'''
    global dirty

    paths = {}
    try:
        with open(state_file, 'r') as f:
            paths = json.load(f)
    except:
        dirty |= True

    for dest, state in paths.items():
        if dest not in entries:
            continue

        ent = entries[dest]
        ent['discard'] = state.get('discard')
        #ent['skip'] = True

        if ent['type'] == 'file':
            ent['compress'] = state.get('compress')
            ent['real_size'] = state.get('real_size')
            ent['transform'] = state.get('transform', {})

def filter_rank(filter) -> int:
    '''Compute the rank for the filter. Filters with higher rank takes precedence over filters with lower rank'''

    if filter == '*':
         return 0

    ext = len(filter.split('.'))
    path = len(filter.split('/'))
    return (ext - 1) + (path - 1) * 2


def apply_rules() -> None:
    '''Applies preprocessing rules for entries'''
    global dirty

    for ent in tuple(entries.values()):
        dest = ent['dest']
        xforms = {}
        compress = None

        for filter, actions in config['filter']:
            if not fnmatch(dest, filter):
                continue

            rank = filter_rank(filter)
            if ent.get('rank') is not None:
                if ent['rank'] > rank:
                    continue
                else:
                    # A filter with a higher rank exists, clear previous transform to avoid mixing transforms order
                    xforms = {}

            ent['rank'] = rank
            for action, args in actions:
                parts = action.split()
                enable = True

                if parts[0] == 'no':
                    enable = False
                    parts = parts[1:]

                verb = parts[0]
                if verb == 'discard' and enable:
                    discards[dest] = ent
                    continue

                if verb == 'cache':
                    ent['cache'] = enable
                    continue

                if verb == 'compress':
                    if ent['type'] == 'dir':
                        continue
                    if not enable:
                        compress = None
                        continue

                    compressors = ['deflate', 'gzip', 'zlib']
                    if heatshrink2:
                        compressors += ['heatshrink']
                    if parts[1] in compressors:
                        compress = [parts[1], args]
                        continue
                    raise Exception(f'{parts[1]} is not a valid compress type')

                if verb in transforms:
                    if ent['type'] == 'dir':
                        continue
                    xforms[verb] = args if enable else False
                    continue

                raise Exception(f'{verb} is not a known transform')

        if ent['type'] == 'file':
            # if never seen before, preprocess
            if ent.get('transform') is None:
                ent['transform'] = {}
                ent['skip'] = False

            if ent['transform'].keys() != xforms.keys():
                # if transforms changed, apply and preprocess
                ent['transform'] = xforms
                ent['skip'] = False

            # if is file and compression changed, apply and preprocess
            if ent.setdefault('compress', None) != compress:
                ent['compress'] = compress
                ent['skip'] = False

def preprocess(ent: dict) -> None:
    '''Run preprocessors for a given entry'''
    global dirty

    dest = ent['dest']

    for discard in discards.values():
        if dest == discard['dest']:
            return
        if discard['type'] != 'dir':
            continue
        if dest.startswith(discard['dest'] + '/'):
            return

    if ent['type'] == 'file':
        print(f'         - {dest}', file=stderr)

        with open(ent['path'], 'rb') as f:
            data = f.read()

        for name, args in ent['transform'].items():
            print(f'           - {name}... ', file=stderr, end='', flush=True)
            transform = transforms[name]
            if 'meta' in transform:
                dest = pipe_script(transform['path'], args, str.encode(ent['dest'])).decode()
                ent['dest'] = dest
                ent['name'] = dest.split('/')[-1]
                print(f'done as {dest}', file=stderr)
            else:
                data = pipe_script(transform['path'], args, data) if isinstance(args, dict) else data
                print('done', file=stderr)

        if ent['compress']:
            name, args = ent['compress']
            if name == 'deflate':
                print(f'           - compress {name} (deprecated, please use zlib)... ',
                        file=stderr, end='', flush=True)
                name = 'zlib'
            else:
                print(f'           - compress {name}... ', file=stderr, end='',
                        flush=True)

            if name == 'zlib':
                level = args.get('level', 9)
                compressed = zlib.compress(data, level)
            elif heatshrink2 and name == 'heatshrink':
                window = args.get('window', 11)
                lookahead = args.get('lookahead', 4)
                compressed = heatshrink2.compress(data, window, lookahead)
            elif name == 'gzip':
                level = args.get('level', 9)
                compressed = gzip.compress(data, level)

            if len(data) < len(compressed):
                print('skipped', file=stderr)
                ent['real_size'] = None
            else:
                percent = 0
                size = os.path.getsize(ent['path'])
                if size != 0:
                    percent = len(compressed) / size
                print(f'done ({percent * 100:.1f}%)', file=stderr)
                ent['real_size'] = len(data)
                data = compressed

        with open(os.path.join(cache_dir, dest), 'wb') as f:
            f.write(data)

    elif ent['type'] == 'dir':
        if not os.path.exists(os.path.join(cache_dir, dest)):
            os.mkdir(os.path.join(cache_dir, dest))

    dirty |= True

def run_preprocessors() -> None:
    '''Run preprocessors on all entries as needed'''
    global dirty

    for ent in entries.values():
        dest = ent['dest']

        # if file is not in cache, preprocess
        if ent['skip'] and not os.path.exists(os.path.join(cache_dir, dest)):
            ent['skip'] = False

        # if file marked not to be cached, preprocess
        if ent['skip'] and ent.get('cache') == False:
            ent['skip'] = False

        # if a file, check that the file is not newer
        if ent['skip'] and ent['type'] == 'file':
            root_mtime = os.path.getmtime(ent['path'])
            cache_mtime = os.path.getmtime(os.path.join(cache_dir, dest))
            if root_mtime > cache_mtime:
                ent['skip'] = False

        # if entry is not marked skip, preprocess
        if not ent['skip']:
            preprocess(ent)

        # mark dirty if something has been marked discard
        if dest in discards.keys() and not ent.get('discard'):
            dirty |= True

def check_output() -> None:
    '''Checks if the output file exists or is older than the state file'''
    global dirty

    dirty |= not os.path.exists(output_file)
    if not dirty:
        dirty |= os.path.getmtime(state_file) > os.path.getmtime(output_file)

    if not dirty:
        print("         - Nothing to do!", file=stderr)
        exit(0)

### Stage 2 ###

def save_state() -> None:
    '''Save current state'''
    paths = {}

    for ent in entries.values():
        dest = ent['dest']

        skip = False
        for discard in discards.values():
            if discard['type'] != 'dir':
                continue
            if dest.startswith(discard['dest'] + '/'):
                skip = True
                break
        if skip:
            continue

        state = {
            'type': ent['type'],
        }
        if dest in discards.keys():
            state['discard'] = True
        else:
            if ent['type'] == 'file':
                if ent['transform'] != {}:
                    state['transform'] = ent['transform']
                if ent['compress'] is not None:
                    state['compress'] = ent['compress']
                if ent.get('real_size') is not None:
                    state['real_size'] = ent['real_size']
        paths[dest] = state

    for ent in tuple(entries.values()):
        dest = ent['dest']
        for discard in discards.values():
            if dest == discard['dest']:
                del entries[dest]
            if discard['type'] == 'dir':
                if dest.startswith(discard['dest'] + '/'):
                    del entries[dest]

    with open(state_file, 'w') as f:
        json.dump(paths, f, indent=4)

def generate_file_header(ent) -> None:
    '''Generate header and load data for a file entry'''
    name = ent['name'].encode('utf-8')

    data_size = os.path.getsize(os.path.join(cache_dir, ent['dest']))
    if ent.get('compress') and ent.get('real_size') is not None:
        method, args = ent['compress']
        if method in ('deflate', 'zlib'):
            comp = COMP_ALGO_ZLIB
            opts = args.get('level', 9)
        elif heatshrink2 and method == 'heatshrink':
            comp = COMP_ALGO_HEATSHRINK
            window = args.get('window', 11)
            lookahead = args.get('lookahead', 4)
            opts = lookahead << 4 | window
        elif method == 'gzip':
            comp = COMP_ALGO_GZIP
            opts = args.get('level', 9)

        header = bytearray(format.comp.size + len(name))
        format.comp.pack_into(header, 0, 0, 0xFF00 | comp, len(name), opts, 0,
                data_size, ent['real_size'])
        header[format.comp.size:] = name
    else:
        header = bytearray(format.file.size + len(name))
        format.file.pack_into(header, 0, 0, 0xFF00, len(name), 0, 0, data_size)
        header[format.file.size:] = name

    ent['header'] = header
    ent['data_size'] = data_size

def generate_dir_header(dirent: dict) -> None:
    '''Generate header and data for a directory entry'''
    if dirent['dest'] == '':
        dirent['parent'] = None
        depth = 0
    else:
        depth = dirent['dest'].count('/') + 1

    seg = dirent['name'].encode('utf-8')

    children = []
    for path, ent in entries.items():
        count = path.count('/')
        if count != depth or not ent['dest']:
            continue
        if (count == 0 and depth == 0) or path.startswith(dirent['dest'] + '/'):
            children.append(ent)
            if ent['dest']:
                ent['parent'] = dirent

    dirent['children'] = children
    child_count = len(children)

    header = bytearray(format.dir.size + (4 * child_count) + len(seg))
    format.dir.pack_into(header, 0, 0, child_count, len(seg), 0)
    header[format.dir.size + (4 * child_count):] = seg
    dirent['header'] = header
    dirent['data_size'] = 0

def generate_entry_headers() -> None:
    '''Iterate entries and call their respective generate header funciton'''
    for ent in entries.values():
        if ent['type'] == 'file':
            generate_file_header(ent)
        elif ent['type'] == 'dir':
            generate_dir_header(ent)

def append_frogfs_header() -> None:
    '''Generate FrogFS header and calculate entry offsets'''
    global data

    num_ent = len(entries)

    bin_size = align(format.head.size) + align(format.hash.size * num_ent)
    for ent in entries.values():
        ent['header_offs'] = bin_size
        bin_size += align(len(ent['header']))

    for ent in entries.values():
        ent['data_offs'] = bin_size
        bin_size += align(ent['data_size'])

    bin_size += format.foot.size

    data += format.head.pack(format.FROGFS_MAGIC, format.FROGFS_VER_MAJOR,
                             format.FROGFS_VER_MINOR, num_ent, bin_size)

def apply_fixups() -> None:
    '''Insert offsets in dir and file headers'''
    for ent in entries.values():
        parent = ent['parent']
        if parent:
            format.offs.pack_into(ent['header'], 0, parent['header_offs'])
        if ent['type'] == 'file':
            format.offs.pack_into(ent['header'], 8, ent['data_offs'])
            continue
        for i, child in enumerate(ent['children']):
            offs = child['header_offs']
            format.offs.pack_into(ent['header'], format.dir.size + (i * 4),
                                  offs)

def append_hashtable() -> None:
    '''Generate hashtable for entries'''
    global data

    hashed_entries = {djb2_hash(v['dest']): v for k,v in entries.items()}
    hashed_entries = dict(sorted(hashed_entries.items(), key=lambda e: e))

    for hash, ent in hashed_entries.items():
        data += format.hash.pack(hash, ent['header_offs'])

def append_headers_and_files() -> None:
    global data

    # first append the entry headers
    for ent in entries.values():
        data += pad(ent['header'])

    # then append the file data
    for ent in entries.values():
        if ent['type'] == 'file':
            with open(os.path.join(cache_dir, ent['dest']), 'rb') as f:
                data += pad(f.read())

def append_footer() -> None:
    '''Generate FrogFS footer'''
    global data

    data += format.foot.pack(crc32(data) & 0xFFFFFFFF)

def write_output() -> None:
    '''Write the data to the output file'''
    with open(output_file, 'wb') as f:
        f.write(data)

    print("         - Output file written", file=stderr)

if __name__ == '__main__':
    frogfs_dir = os.path.abspath(os.path.join(os.path.dirname(__file__), '..'))
    default_tools_dir = os.path.join(frogfs_dir, 'tools')

    # Get arguments
    argparse = ArgumentParser()
    argparse.add_argument('-C', dest='work_dir', metavar='WORKDIR',
                          help='change working directory')
    argparse.add_argument('--tools', metavar='TOOLSDIR', action='append',
                          help='specify additional tools directories',
                          default=[default_tools_dir])
    argparse.add_argument('config_file', metavar='CONFIGFILE',
                          help='YAML configuration file')
    argparse.add_argument('build_dir', metavar='BUILDDIR',
                          help='intermediary artifact path')
    argparse.add_argument('output_file', metavar='OUTPUTFILE',
                          help='output binary file')

    args = argparse.parse_args()
    tools_dirs = [os.path.abspath(p) for p in args.tools]
    config_file = os.path.abspath(args.config_file)
    build_dir = os.path.abspath(args.build_dir)
    output_file = os.path.abspath(args.output_file)
    if args.work_dir:
        os.makedirs(args.work_dir, exist_ok=True)
        os.chdir(args.work_dir)

    output_name, _ = os.path.splitext(os.path.basename(output_file))
    cache_dir = os.path.join(build_dir, output_name + '-cache')
    state_file = os.path.join(build_dir, output_name + '-cache-state.json')

    # setup environment
    os.environ['FROGFS_DIR'] = frogfs_dir
    os.environ['NODE_PREFIX'] = build_dir
    os.environ['NODE_PATH'] = os.path.join(build_dir, 'node_modules') + \
                              os.pathsep + default_tools_dir

    # Initial setup
    config  = load_config()
    entries = collect_entries()
    transforms = load_transforms()
    discards = {}
    dirty = False
    data = b''

    # Stage 1
    print("       - Stage 1", file=stderr)
    clean_removed()
    load_state()
    apply_rules()
    run_preprocessors()
    check_output()

    # Stage 2
    print("       - Stage 2", file=stderr)
    save_state()
    generate_entry_headers()
    append_frogfs_header()
    append_hashtable()
    apply_fixups()
    append_headers_and_files()
    append_footer()
    write_output()
