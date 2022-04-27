
from .console import ERROR, NOTE, clsprint
from typing import List, Tuple, NoReturn
import os, re, sys, shutil, subprocess, datetime

def nofolder(file: str) -> str:
    return file.split('/')[-1]

def noextension(file: str) -> str:
    return '.'.join(file.split('.')[:-1])

def get_files(folder: str, pattern: str, full_path: bool = True) -> List[str]:
    regex = re.compile(pattern)
    (_, _, files) = next(os.walk(folder))
    if full_path:
        files = list(map(lambda file: f'{folder}/{file}', files))
    return list(filter(lambda file: regex.search(file), files))

def copytree(src, dst, symlinks=False, ignore=None):
    for item in os.listdir(src):
        s, d = os.path.join(src, item), os.path.join(dst, item)
        if os.path.isdir(s):
            shutil.copytree(s, d, symlinks, ignore)
        else:
            shutil.copy2(s, d)

def die(*args, **kwargs) -> NoReturn:
    clsprint(*args, **kwargs)
    sys.exit(1)

def vprint(*args, **kwargs):
    from .config import get
    if get('verbose'):
        clsprint(*args, **kwargs)

def _get_command_full(cmd: str | List[str]) -> str:
    if type(cmd) == list: cmd = ' '.join(cmd)
    return cmd

def _get_command(cmd: str | List[str]) -> str:
    return cmd[0] if type(cmd) == list else cmd.split()[0]

def _gen_cmd(cmd: str | List[str]) -> List[str]:
    cmd = cmd if type(cmd) == list else cmd.split(' ')
    if '' in cmd:
        cmd.remove('')
    return cmd

def require(cmd: str | List[str], exit_on_fail: bool = True) -> str:
    try:
        vprint(f'{NOTE}: Execute:', ' '.join(cmd) if type(cmd) == list else cmd)
        return subprocess.run(_gen_cmd(cmd), check=True, capture_output=True).stdout.decode('utf-8')
    except (subprocess.CalledProcessError, OSError) as e:
        if not exit_on_fail: return None
        clsprint(f'{ERROR}: Failed to execute "{_get_command_full(cmd)}": {e}')
        clsprint(f'{NOTE}: Make sure {_get_command(cmd)} is available on your system')
        sys.exit(1)

def run_cmd(cmd: str | List[str], exit_on_fail: bool = True, print_stdout: bool = False, print_stderr: bool = False) -> Tuple[str, str]:
    try:
        vprint(f'{NOTE}: Execute:', ' '.join(cmd) if type(cmd) == list else cmd)
        result = subprocess.run(_gen_cmd(cmd), check=True, capture_output=True)
        if print_stdout and result.stdout: clsprint('\n' + result.stdout.decode('utf-8'))
        if print_stderr and result.stderr: clsprint('\n' + result.stderr.decode('utf-8'))
        return result.stdout.decode('utf-8'), result.stderr.decode('utf-8')
    except OSError as e:
        if not exit_on_fail: return None
        clsprint(f'{ERROR}: Failed to execute "{_get_command_full(cmd)}": {e}')
        clsprint(f'{NOTE}: Make sure {_get_command(cmd)} is available on your system')
        sys.exit(1)
    except subprocess.CalledProcessError as e:
        if not exit_on_fail: return None
        clsprint(f'{ERROR}: {e}')
        if print_stdout: clsprint('\n' + e.stdout.decode('utf-8'))
        if print_stderr: clsprint('\n' + e.stderr.decode('utf-8'))
        sys.exit(1)

def ns_time_to_str(ns: float) -> str:
    ms = ns // 1_000_000
    s = ms // 1000
    m = s // 60 % 60
    result = ''
    if m > 0:
        result += f'{m}m '
    if (s := s % 60) > 0:
        result += f'{s}s '
    if (ms := ms % 1000) > 0:
        result += f'{ms}ms'
    if result == '':
        result = f'{ns}ns'
    return result

def current_time_str() -> str:
    return datetime.datetime.now().strftime('%Y-%m-%d %H:%M:%S')
