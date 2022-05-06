
from typing import Final
import time

class Color:
    RESET         = "\033[0m"
    BLACK         = "\033[0;30m"
    RED           = "\033[0;31m"
    GREEN         = "\033[0;32m"
    BROWN         = "\033[0;33m"
    BLUE          = "\033[0;34m"
    PURPLE        = "\033[0;35m"
    CYAN          = "\033[0;36m"
    LIGHT_GRAY    = "\033[0;37m"
    DARK_GRAY     = "\033[1;30m"
    LIGHT_RED     = "\033[1;31m"
    LIGHT_GREEN   = "\033[1;32m"
    YELLOW        = "\033[1;33m"
    LIGHT_BLUE    = "\033[1;34m"
    LIGHT_PURPLE  = "\033[1;35m"
    LIGHT_CYAN    = "\033[1;36m"
    LIGHT_WHITE   = "\033[1;37m"

ERROR:   Final[str] = f'[{Color.RED}ERROR{Color.RESET}]'
WARNING: Final[str] = f'[{Color.YELLOW}WARNING{Color.RESET}]'
NOTE:    Final[str] = f'[{Color.BLUE}NOTE{Color.RESET}]'

def clear_line():
    print('\033[2K', end='')

def clsprint(*args, **kwargs):
    clear_line()
    print(*args, **kwargs)

def progress_bar(iteration: int,
                 total: int,
                 prefix: str = '',
                 suffix: str = '',
                 decimals: int = 1,
                 length: int = 100,
                 fill: str = '█',
                 print_end: str = '\r',
                 clear_on_finish: bool = False):

    str_format = "{0:." + str(decimals) + "f}"
    percent = str_format.format(100 * (iteration / float(total)))
    filled_length = int(round(length * iteration / float(total)))
    bar = fill * filled_length + '-' * (length - filled_length)
    clsprint(f'\r{prefix} |{bar}| {percent}% {suffix}', end=print_end)

    if iteration >= total:
        if clear_on_finish:
            clear_line()
        else:
            print()

class context:
    spaces = 0
    progress = 0
    increment = 1
    bar_total = 100
    bar_length = 50

    @staticmethod
    def print_task(*args):
        clsprint(' ' * context.spaces + '[+]', *args)

    @staticmethod
    def increment_progress():
        context.progress += context.increment

    @staticmethod
    def print_progress():
        progress_bar(
            iteration=context.progress,
            total=context.bar_total,
            prefix='Progress:',
            suffix='Complete',
            length=context.bar_length,
            clear_on_finish=True
        )

    @staticmethod
    def new_task(*args):
        class Context:
            def __init__(self, *args):
                self.start_time = 0
                self.end_time = 0
                context.print_task(*args)

            def __enter__(self):
                context.spaces += 2
                context.print_progress()
                self.start_time = time.time_ns()

            def __exit__(self, exc_type, exc_value, traceback):
                self.end_time = time.time_ns()

                from .config import get
                if get('count_time'):
                    from .utils import ns_time_to_str
                    clsprint(' ' * context.spaces + f'| Time: {ns_time_to_str(self.end_time - self.start_time)}')

                context.spaces -= 2

                context.increment_progress()
                context.print_progress()

        return Context(*args)
