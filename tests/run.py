#!/usr/bin/env python3

from typing import Dict, Tuple, Final
from tests import tests
import os, sys, subprocess


class Color:
    RESET  = "\033[0m"
    RED    = "\033[0;31m"
    BLUE   = "\033[0;34m"
    GREEN  = "\033[0;32m"
    YELLOW = "\033[1;33m"

ERROR:   Final[str] = f'[{Color.RED}ERROR{Color.RESET}]'
WARNING: Final[str] = f'[{Color.YELLOW}WARNING{Color.RESET}]'
NOTE:    Final[str] = f'[{Color.BLUE}NOTE{Color.RESET}]'

VERSION: Final[str] = '0.1.0'
FOLDER:  Final[str] = os.path.dirname(os.path.realpath(__file__))

config = {
    'profile': '',
    'script_dir': FOLDER,
    'topdir': FOLDER + '/..',
    'print_result': False,
    'tests': list()
}

context = {'failed': 0, 'succeded': 0}

def run_cmd(cmd: str) -> Tuple[int, str, str]:
    try:
        cmd = cmd.split(' ')
        if '' in cmd: cmd.remove('')
        if '\n' in cmd: cmd.remove('\n')
        result = subprocess.run(cmd, check=True, capture_output=True)
        return result.returncode, result.stdout.decode('utf-8'), result.stderr.decode('utf-8')
    except OSError as e:
        print(f'{ERROR}: Failed to execute command: {e}')
        sys.exit(1)
    except subprocess.CalledProcessError as e:
        return e.returncode, e.stdout.decode('utf-8'), e.stderr.decode('utf-8')

def run_test(test: str, test_config: Dict) -> Tuple[bool, int]:
    result = run_cmd(f'{config["topdir"]}/target/{config["profile"]}/bin/ff {config["script_dir"]}/{test}.ff {test_config.get("args", "")}')
    if config['print_result']:
        print(f'{NOTE}: {test}: {result}')
    if test_config['expect'] == 'return':
        return result[0] == test_config['value'], result[0]
    elif test_config['expect'] == 'stdout':
        return result[1] == test_config['value'], result[0]
    elif test_config['expect'] == 'stderr':
        return result[2] == test_config['value'], result[0]
    else:
        print(f'{ERROR}: Bad test configuration ({test})')
        sys.exit(1)

def run_and_handle_test(test: str, test_config: Dict):
    result = run_test(test, test_config)
    if result[0]:
        context['succeded'] += 1
        print(f'[  {Color.GREEN}OK{Color.RESET}  ] {test}')
    else:
        context['failed'] += 1
        print(f'[ {Color.RED}FAIL{Color.RESET} ] {test} ({result[1]})')

def print_summary():
    print(f'Succeded: {Color.GREEN}{context["succeded"]}{Color.RESET}')
    print(f'Failed:   {Color.RED}{context["failed"]}{Color.RESET}')

def run_tests(test_list):
    for test in test_list:
        try:
            run_and_handle_test(test, tests[test])
        except KeyError as e:
            print(f'{ERROR}: No such test: {test}')
            sys.exit(1)
    print_summary()

def usage(print_version: bool):
    print(
        f'{f"ff/tests v{VERSION}" if print_version else ""}'
        f'Usage: {sys.argv[0]} [OPTION] PROFILE [TEST...]\n'
        'Options:\n'
        '  -h, --help         - Prints this message\n'
        '  -p, --print-result - Prints test results (return code, stdout, stderr)'
    )

def main():
    i = 1
    while i < len(sys.argv):
        if sys.argv[i] in ['-h', '--help', 'help']:
            usage(True)
            return
        elif sys.argv[i] in ['-p', '--print-result']:
            config['print_result'] = True
        else:
            if config['profile'] == '':
                config['profile'] = sys.argv[i]
            else:
                config['tests'].append(sys.argv[i])

        i += 1
    
    if config['profile'] == '':
        print(f'{ERROR}: No PROFILE supplied')
        usage(False)
        sys.exit(1)

    if config['tests']:
        run_tests(config['tests'])
    else:
        run_tests(tests.keys())

if __name__ == '__main__':
    main()
