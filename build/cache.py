
from typing import Final
from . import config, utils, console
import os, json

CONFIG_CACHE_FILE: Final[str] = 'cache_file'

_cache = {}

def _reset_config_file():
    open(config.get(CONFIG_CACHE_FILE), 'w').write(r'{}')

def load():
    if not os.path.exists(config.get(CONFIG_CACHE_FILE)):
        _reset_config_file()
    try:
        global _cache
        _cache = json.loads(open(config.get(CONFIG_CACHE_FILE)).read())
    except json.JSONDecodeError as e:
        _reset_config_file()
    if config.get('profile') not in _cache:
        _cache[config.get('profile')] = {
            'files': {},
            'last_build': {'compilation': {}, 'result': '', 'start_time': '', 'end_time': ''}
        }

def save():
    open(config.get(CONFIG_CACHE_FILE), 'w').write(json.dumps(_cache, indent=4))

def update():
    for file in _cache[config.get('profile')]['files'].keys():
        add_file(file)

def add_file(filename: str):
    _cache[config.get('profile')]['files'][filename] = os.path.getmtime(filename)

def changed(filename: str, add_if_not_exists: bool = True) -> bool:
    if config.get('consider_unchanged'):
        return False
    if filename not in _cache[config.get('profile')]['files'] and add_if_not_exists:
        add_file(filename)
        return True
    return float(_cache[config.get('profile')]['files'][filename]) < os.path.getmtime(filename)

def get(*args):
    value = _cache
    for arg in args:
        try:
            value = value[arg]
        except Exception as e:
            utils.die(f'{console.ERROR}: Can\'t get cache entry for key "' + '.'.join(args) + f'": {e}')
    return value

def update_compilation_output(filename: str, stdout: str, stderr: str):
    _cache[config.get('profile')]['last_build']['compilation'][filename] = {'stdout': stdout, 'stderr': stderr}

def update_last_build_start(timestamp):
    _cache[config.get('profile')]['last_build']['start_time'] = timestamp

def update_last_build_end(timestamp):
    _cache[config.get('profile')]['last_build']['end_time'] = timestamp

def update_last_build_result(result: str):
    _cache[config.get('profile')]['last_build']['result'] = result

def print_last_build():
    print('last build:'
        + '\nstarted: ' + _cache[config.get('profile')]['last_build']['start_time']
        + '\nended: '   + _cache[config.get('profile')]['last_build']['end_time']
        + '\nresult: '  + _cache[config.get('profile')]['last_build']['result'] + '\n')

    for filename, out in _cache[config.get('profile')]['last_build']['compilation'].items():
        if out['stdout'] != '' or out['stderr'] != '':
            print(f'{console.Color.BLUE}FILE{console.Color.RESET}: {filename}')
            print(f'{console.Color.GREEN}STDOUT{console.Color.RESET}:', out['stdout'])
            print(f'{console.Color.RED}STDERR{console.Color.RESET}:', out['stderr'])
