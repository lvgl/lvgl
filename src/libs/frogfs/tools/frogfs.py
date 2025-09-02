import os
from importlib import import_module
from shutil import which
from subprocess import DEVNULL, PIPE, Popen, call
from sys import executable, stderr
from typing import Union
from re import findall


def align(n: int) -> int:
    '''Return n rounded up to the next 4'''
    return ((n + 3) // 4) * 4

def djb2_hash(s: str) -> int:
    '''A simple string hashing algorithm'''
    hash = 5381
    for c in s.encode('utf-8'):
        hash = ((hash << 5) + hash ^ c) & 0xFFFFFFFF
    return hash

def expand_variables(s, defines={}):
    matches = findall(r'(?<!\\)\$[\w]+|(?<!\\)\$\{[:\w]+\}', s)
    for match in matches:
        if match.startswith('${ENV:'):
            value = os.environ.get(match[6:-1], '')
        elif match.startswith('${'):
            value = defines.get(match[2:-1], '')
        else:
            value = defines.get(match[1:], '')
        s = s.replace(match, value, 1)
    return s

def needs(module):
    try:
        import_module(module)
    except:
        print("installing... ", end='', file=stderr, flush=True)
        status = call([executable, '-m', 'pip', '-qq', 'install', module],
                stdout=DEVNULL)
        if status != 0:
            print('could not install ' + module, file=stderr)
            exit(status)

def pad(data: Union[bytes, bytearray]) -> bytes:
    '''Return data padded with zeros to next alignment'''
    return data.ljust(align(len(data)), b'\0')

def pipe_script(script: str, args: dict, data: Union[bytes, bytearray]) -> bytes:
    '''Run a script with arguments and data to stdin, returning stdout data'''
    _, ext = os.path.splitext(script)
    if ext == '.js':
        if which('node') == None:
            raise Exception('node was not found, please install it')
        command = ['node']
    elif ext == '.py':
        command = [executable]
    else:
        raise Exception(f'unhandled file extension for {script}')

    command.append(script)
    for arg, value in args.items():
        arg = str(arg)
        command.append(('--' if len(arg) > 1 else '-') + arg)
        if value is not None:
            command.append(str(value))

    process = Popen(command, stdin=PIPE, stdout=PIPE)
    data, _ = process.communicate(input=data)
    code = process.returncode
    if code != 0:
        raise Exception(f'error {code} running process')

    return data
