
from . import config, cache, utils
from .console import ERROR, NOTE
from typing import List
import os

dirs = {}

def check():
    config.require('cpp')
    config.require('cpp', 'compiler')
    utils.require([config.get('cpp', 'compiler'), '-v'])


def init():
    check()
    dirs['bin'] = config.format('{build_dir}/{profile}/bin')
    dirs['include'] = config.format('{build_dir}/{profile}/include')
    dirs['lib'] = config.format('{build_dir}/{profile}/lib')
    dirs['obj'] = config.format('{build_dir}/{profile}/obj')
    if not os.path.exists(config.format('{build_dir}/{profile}')):
        os.mkdir(config.format('{build_dir}/{profile}'))
    for _, folder in dirs.items():
        if not os.path.exists(folder):
            os.mkdir(folder)


def compile(file: str, prefix_folder: str = '', cxxflags: List[str] | str = ''):
    output = dirs['obj'] \
            + ('/' + prefix_folder.rstrip('/') if prefix_folder else '') \
            + '/' + utils.noextension(utils.nofolder(file)) + '.o'

    file_state = 'unchanged'
    if config.get('force'):   file_state = 'forced to recompile'
    elif cache.changed(file): file_state = 'changed, recompiling'

    utils.vprint(f'{NOTE}: {file} {file_state}')

    if not os.path.exists(output) or cache.changed(file) or config.get('force'):
        if not os.path.exists(dirs['obj'] + '/' + prefix_folder):
            os.mkdir(dirs['obj'] + '/' + prefix_folder)

        if type(cxxflags) == str:
            cxxflags = cxxflags.split(' ')
        elif type(cxxflags) != list:
            utils.die(f'{ERROR}: cpp.compile: Unexpected type of cxxflags: "{type(cxxflags)}"') 

        stdout, stderr = utils.run_cmd([
            config.get('cpp', 'compiler'),
            '-I' + dirs['include'],
            *config.get('cpp', 'cxxflags'),
            *cxxflags,
            '-c',
            file,
            '-o', output
        ], print_stderr=config.get('print_compiler_output'))

        cache.update_compilation_output(output, stdout, stderr)


def link_exe(files: List[str], output: str, libs: List[str] = [], cxxflags: List[str] | str = []):
    if type(cxxflags) == str:
        cxxflags = cxxflags.split(' ')
    elif type(cxxflags) != list:
        utils.die(f'{ERROR}: cpp.link_exe: Unexpected type of cxxflags: "{type(cxxflags)}"') 

    stdout, stderr = utils.run_cmd([
        config.get('cpp', 'compiler'),
        '-I' + dirs['include'],
        '-L' + dirs['lib'],
        *config.get('cpp', 'cxxflags'),
        *cxxflags,
        *files,
        *list(map(lambda l: f'-l{l}', libs)),
        '-o', dirs['bin'] + '/' + output
    ], print_stderr=config.get('print_compiler_output'))

    cache.update_compilation_output(dirs['bin'] + '/' + output, stdout, stderr)


def create_static_lib(files: List[str], output: str):
    stdout, stderr = utils.run_cmd([
        config.get('cpp', 'ar'),
        'cr',
        dirs['lib'] + '/' + output,
        *files,
    ], print_stderr=config.get('print_compiler_output'))

    cache.update_compilation_output(dirs['lib'] + '/' + output, stdout, stderr)


def clean():
    pass


def compile_batch(files: List[str], prefix_folder: str = '', cxxflags: str = ''):
    for file in files:
        compile(file, prefix_folder, cxxflags)


def get_objs(folder: List[str] | str) -> List[str]:
    if type(folder) == list:
        result = []
        for f in folder:
            result += utils.get_files(f, r'.+\.o')
        return result
    return utils.get_files(folder, r'.+\.o')
