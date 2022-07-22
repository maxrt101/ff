
from .task import _tasks, run_task, run_subtask, calculate_progress_increment, calculate_progress_increment_sub
from .console import ERROR, NOTE, Color
from . import config, cache, console, utils, context
from typing import Tuple
import sys, json, time

VERSION = '0.2.0'

options = {
    'task': '',
    'subtask': '',
    'features': []
}

def usage():
    print(
        f'ff/build v{VERSION}\n'
        '\n'
        f'Usage: {sys.argv[0]} [OPTIONS] [:PROFILE] [TASK[:SUBTASK]]\n'
        '\n'
        'Options:\n'
        '  -h, --help            - Shows this message\n'
        '  --version             - Shows version\n'
        '  -v, --verbose         - Turns on verbose output\n'
        '  -o, --output          - Print compiler output\n'
        '  -f, --force           - Forces recompilation\n'
        '  -u, --unchanged       - Consider every file unchanged\n'
        '  -t, --count-time      - Turns on time counting for each task\n'
        '  -p, --profile PROFILE - Select different profile (debug, release)\n'
        '  --feature FEATURELIST - Set feature list\n'
        '\n'
        'Special commands (in place of TASK):\n'
        '  +config               - Shows current config\n'
        '  +info                 - Shows info about the build\n'
        '  +last                 - Shows info about last build\n'
        '  +list                 - Lists all tasks\n'
        '  +cache                - Show file cache for profile\n'
        '  +cache-clear          - Cleans cache for profile\n'
        '\n'
        'Profile:\n'
        '  Specifies a profile for the build (same as -p option)\n'
        f'  Example: {sys.argv[0]} :debug\n'
    )


def parse_task(task_name: str) -> Tuple[str, str]:
    tokens = task_name.split(':')
    if len(tokens) == 1:
        tokens.append('')
    return tokens[0], tokens[1]


def list_tasks():
    for target, task_info in _tasks.items():
        print(f'{Color.GREEN}{target}{Color.RESET}', end='')
        if task_info.depends:
            print(' depends on ' + Color.RED + ', '.join(task_info.depends) + Color.RESET)
        else:
            print()
        if task_info.subtasks:
            print(' subtasks:')
        for subtask, _ in task_info.subtasks.items():
            print(f'  {subtask}')


def task_info(task_name: str):
    parsed = parse_task(task_name)
    if parsed[0] not in _tasks:
        utils.die(f'{ERROR}: Cannot find task "{parsed[0]}"')

    t = _tasks[parsed[0]]

    if parsed[1] != '':
        if parsed[1] not in _tasks[parsed[0]].subtasks:
            utils.die(f'{ERROR}: Cannot find subtask "{parsed[1]}" in task "{parsed[0]}"')
        t = t.subtasks[parsed[1]]
        print(f'Subtask: {t.name}')
        print(f'Parent: {parsed[0]}')
    else:
        print(f'Task: {t.name}')
        print(f'Dependencies: {t.depends}')
        print(f'Subtasks: {list(t.subtasks.keys())}')


def check_next_arg(i: int) -> str:
    if i + 1 >= len(sys.argv):
        print(f'{ERROR}: Option "{sys.argv[i]}" requires an argument')
        print(f'{NOTE}: Run "{sys.argv[0]} -h" for usage')
        sys.exit(1)
    return sys.argv[i+1]


def parse_args():
    i = 1
    while i < len(sys.argv):
        if sys.argv[i] == '--version':
            print(f'ff/build v{VERSION}')
            sys.exit(0)
        elif sys.argv[i] in ['-h', '--help']:
            usage()
            sys.exit(0)
        elif sys.argv[i] in ['-v', '--verbose']:
            config.verbose(True)
        elif sys.argv[i] in ['-o', '--output']:
            config.print_compiler_output(True)
        elif sys.argv[i] in ['-f', '--force']:
            config.force(True)
        elif sys.argv[i] in ['-u', '--unchanged']:
            config.consider_unchanged(True)
        elif sys.argv[i] in ['-t', '--count-time']:
            config.count_time(True)
        elif sys.argv[i] in ['-p', '--profile']:
            config.profile(check_next_arg(i))
            i += 1
        elif sys.argv[i] in ['--feature']:
            options['features'] += check_next_arg(i).split(',')
            i += 1
        else:
            if sys.argv[i][0] == ':':
                config.profile(sys.argv[i][1:])
            elif options['task'] == '':
                if len(sys.argv[i]) > 0 and sys.argv[i][0] == '-':
                    utils.die(f'{ERROR}: Unknown option: "{sys.argv[i]}"')
                options['task'], options['subtask'] = parse_task(sys.argv[i])
            else:
                print(f'{ERROR}: Unknown argument: "{sys.argv[i]}"')
                print(f'{NOTE}: Run "{sys.argv[0]} -h" for usage')
                sys.exit(1)

        i += 1


def run(default_task: str = ''):
    with context() as ctx:
        parse_args()
        ctx.reload()

        if options['task'] == '':
            options['task'], options['subtask'] = parse_task(default_task)

        if config.get_feature_handler():
            config.get_feature_handler()(config.get('profile'), options['features'])

        if options['task'] == '+config':
            print(json.dumps(config._config, indent=4))    
        elif options['task'] == '+info':
            print(
                (
                    '{} build\n'
                    '  force        : {}\n'
                    '  verbose      : {}\n'
                    '  count_time   : {}\n'
                    '  default_task : {}\n'
                    '  features     : {}'
                ).format(
                    config.get('profile'),
                    config.get('force'),
                    config.get('verbose'),
                    config.get('count_time'),
                    default_task,
                    options['features']
                )
            )
        elif options['task'] == '+last':
            cache.print_last_build()
        elif options['task'] == '+list':
            list_tasks()
            sys.exit(0)
        elif options['task'] == '+cache':
            for file, mtime in cache.get(config.get('profile'), 'files').items():
                print(f'{file}: ' + time.strftime('%Y-%m-%d %H:%M:%S', time.gmtime(mtime)))
            sys.exit(0)
        elif options['task'] == '+cache-clear':
            cache.clear()
            sys.exit(0)
        else:
            cache.update_last_build_start(utils.current_time_str())
            cache.update_last_build_result('fail')

            if options['subtask'] != '':
                console.context.increment = calculate_progress_increment_sub(options['task'], options['subtask'])
                run_subtask(options['task'], options['subtask'])
            else:
                console.context.increment = calculate_progress_increment(options['task'])
                run_task(options['task'])
            
            cache.update_last_build_end(utils.current_time_str())
            cache.update_last_build_result('ok')
