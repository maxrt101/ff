
from .console import ERROR, NOTE, clsprint
from .utils import die
from typing import Final, Dict, Callable
import os, re, sys, json

FORMAT_PATTERN: Final = re.compile(r'\{.+\}')
CONFIG_BUILD_DIR: Final[str] = 'build_dir'

_filename: str = ''
_config: Dict = {}
_feature_handler = None

def load(filename: str):
    global _filename, _config
    _filename = filename
    _config = json.loads(open(filename).read())
    _config['topdir'] = os.getcwd()
    if 'profile' not in _config:
        _config['profile'] = 'release'
    if 'force' not in _config:
        _config['force'] = False
    if 'verbose' not in _config:
        _config['verbose'] = False
    if 'count_time' not in _config:
        _config['count_time'] = False
    if 'consider_unchanged' not in _config:
        _config['consider_unchanged'] = False
    if 'print_compiler_output' not in _config:
        _config['print_compiler_output'] = False

def get(*args):
    value = _config
    for arg in args:
        try:
            value = value[arg]
        except KeyError as e:
            die(f'{ERROR}: Can\'t get config for key "' + '.'.join(args) + f'": {e}')
    return format(value)

def format(s: str) -> str:
    if type(s) != str: return s
    while FORMAT_PATTERN.search(s):
        try:
            s = s.format(**_config)
        except KeyError as e:
            die(f'{ERROR}: Cannot find key {e} (value: "{s}")')
    return s

def require(*args):
    value = _config
    for arg in args:
        try:
            value = value[arg]
        except KeyError as e:
            clsprint(f'{ERROR}: Can\'t find required config for key "' + '.'.join(args) + '"')
            clsprint(f'{NOTE}: Add "' + '.'.join(args) + f'" key to {_filename} to fix this')
            sys.exit(1)

def update(key: str, value):
    try:
        keys = key.split('.')
        cfg = _config
        for k in keys[:-1]:
            cfg = cfg[k]
        cfg[keys[-1]] = value
    except Exception as e:
        die(f'{ERROR}: Error while setting "{key}": {e}')

# Setters

def profile(value: str):
    _config['profile'] = value

def force(value: bool):
    _config['force'] = value

def consider_unchanged(value: bool):
    _config['consider_unchanged'] = value

def verbose(value: bool):
    _config['verbose'] = value

def print_compiler_output(value: bool):
    _config['print_compiler_output'] = value

def count_time(value: bool):
    _config['count_time'] = value

# Feature handler

def get_feature_handler():
    return _feature_handler

# decorator
def feature_handler(fn: Callable):
    global _feature_handler
    _feature_handler = fn
    return fn
