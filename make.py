#!/usr/bin/env python3

from build.config import format as cf
import build, os, shutil

@build.config.feature_handler
def features(profile, feature_list):
    if 'LOG_STDOUT_ONLY' in feature_list: build.config.get('cpp', 'cxxflags').append('-D_FF_LOG_STDOUT_ONLY')
    if profile == 'debug':
        build.config.get('cpp', 'cxxflags').extend(['-g3', '-D_DEBUG'])
        if 'MEM'     in feature_list: build.config.get('cpp', 'cxxflags').append('-D_FF_MEMORY_DEBUG')
        if 'REF'     in feature_list: build.config.get('cpp', 'cxxflags').append('-D_FF_REF_DEBUG')
        if 'EVAL'    in feature_list: build.config.get('cpp', 'cxxflags').append('-D_FF_EVAL_NODE_DEBUG')
        if 'DISASM'  in feature_list: build.config.get('cpp', 'cxxflags').append('-D_FF_DEBUG_DISASM')
        if 'TOKENS'  in feature_list: build.config.get('cpp', 'cxxflags').append('-D_FF_DEBUG_TOKENS')
        if 'TREE'    in feature_list: build.config.get('cpp', 'cxxflags').append('-D_FF_DEBUG_PRINT_TREE')
        if 'TRACE'   in feature_list: build.config.get('cpp', 'cxxflags').append('-D_FF_DEBUG_TRACE')
        if 'SCOPES'  in feature_list: build.config.get('cpp', 'cxxflags').append('-D_FF_DEBUG_SCOPES')
        if 'GLOBALS' in feature_list: build.config.get('cpp', 'cxxflags').append('-D_FF_DEBUG_GLOBALS')
        if 'NOCATCH' in feature_list: build.config.get('cpp', 'cxxflags').append('-D_FF_DEBUG_DONT_CATCH_EXCEPTIONS')

@build.task()
def install_headers(ctx):
    src, dst = cf('{topdir}/include'), cf('{build_dir}/{profile}/include/ff')
    if os.path.exists(dst):
        shutil.rmtree(dst)
    os.mkdir(dst)
    build.utils.copytree(src, dst)

@build.task()
def dependencies(ctx):
    ctx.run_subtasks()

@build.subtask('dependencies')
def mrt(ctx):
    if not os.path.exists(cf('{topdir}/mrt')):
        build.utils.die('Please download mrt')
    build.run_cmd(['make', '-C', cf('{topdir}/mrt'), 'PREFIX=' + cf('{build_dir}/{profile}')])

@build.task(['libff', 'stdlib'])
def ff(ctx):
    build.cpp.compile(cf('{topdir}/src/main.cc'))
    build.cpp.link_exe(
       files=[cf('{build_dir}/{profile}/obj/main.o')],
       output='ff',
       libs=['ff', 'dl']
    )

@build.task(['install_headers', 'dependencies'])
def libff(ctx):
    ctx.run_subtasks()
    build.cpp.create_static_lib(
        files=build.cpp.get_objs([
            cf('{build_dir}/{profile}/obj/compiler'),
            cf('{build_dir}/{profile}/obj/utils'),
            cf('{build_dir}/{profile}/obj/types'),
            cf('{build_dir}/{profile}/obj/ast'),
            cf('{build_dir}/{profile}/obj/core')
        ]),
        output='libff.a'
    )

@build.subtask('libff')
def core(ctx):
    build.cpp.compile_batch(build.utils.get_files(cf('{topdir}/src/core'), r'.+\.cc'), 'core')

@build.subtask('libff')
def types(ctx):
    build.cpp.compile_batch(build.utils.get_files(cf('{topdir}/src/core/types'), r'.+\.cc'), 'types')

@build.subtask('libff')
def compiler(ctx):
    build.cpp.compile_batch(build.utils.get_files(cf('{topdir}/src/compiler'), r'.+\.cc'), 'compiler')

@build.subtask('libff')
def ast(ctx):
    build.cpp.compile_batch(build.utils.get_files(cf('{topdir}/src/compiler/ast'), r'.+\.cc'), 'ast')

@build.subtask('libff')
def utils(ctx):
    build.cpp.compile_batch(build.utils.get_files(cf('{topdir}/src/utils'), r'.+\.cc'), 'utils')

@build.task(['libff'])
def stdlib(ctx):
    if not os.path.exists(cf('{build_dir}/{profile}/lib/ff')):
        os.mkdir(cf('{build_dir}/{profile}/lib/ff'))
    build.cpp.compile_batch(build.utils.get_files(cf('{topdir}/src/lib'), r'.+\.cc'), 'stdlib', ['-fPIC'])
    for file in build.utils.get_files(cf('{build_dir}/{profile}/obj/stdlib'), r'.+\.o'):
        build.cpp.create_shared_lib([file], 'ff/' + build.utils.noextension(build.utils.nofolder(file)) + '.ffmod')

@build.task()
def clean(ctx):
    shutil.rmtree(cf('{build_dir}/{profile}'))

@build.task()
def cppcheck(ctx):
    build.run_cmd([
        'cppcheck',
        '--enable=all',
        '--inline-suppr',
        '--suppress=unusedFunction',
        '--suppress=missingIncludeSystem',
        '--suppress=unmatchedSuppression',
        '--error-exitcode=1',
        cf('-I{build_dir}/{profile}/include'),
        cf('{topdir}/src')
    ], print_stdout=True, print_stderr=True)

@build.task()
def test(ctx):
    build.run_cmd(
        ['./tests/run.py', cf('{profile}')],
        exit_on_fail=True,
        print_stderr=True,
        print_stdout=True
    )

build.cli.run('ff')
