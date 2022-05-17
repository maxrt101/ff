
from typing import List, Callable
from .console import ERROR, NOTE
from .utils import die, vprint
from . import console

_tasks = {}
_ran_tasks = []

def run_task(task: str, parent_task: str = ''):
    if task not in _tasks:
        die(f'{ERROR}: Cannot find task "{task}"' + (f' (as a dependency of "{parent_task}")' if parent_task != '' else ''))
    _tasks[task].run(parent_task)

def run_subtask(task: str, subtask: str):
    if task not in _tasks:
        die(f'{ERROR}: Cannot find task "{task}"')
    if subtask not in _tasks[task].subtasks:
        die(f'{ERROR}: Cannot find subtask "{subtask}" in task "{task}"')
    _tasks[task].run('', True)
    _tasks[task].subtasks[subtask].run()

class Subtask:
    def __init__(self, task_name: str, name: str, fn: Callable):
        self.task_name = task_name
        self.name = name
        self.fn = fn

    def run(self):
        with console.context.new_task(f'Subtask {self.name}'):
            self.fn(self)

class Task:
    def __init__(self, name: str, depends: List[str], fn: Callable):
        self.name = name
        self.depends = depends
        self.fn = fn
        self.subtasks = {}

    def run(self, parent: str = '', depdendency_only: bool = False):
        if self.name in _ran_tasks:
            vprint(f'{NOTE}: Task {self.name} already done')
            return
        with console.context.new_task(("Dependency" if parent else "Task") + f' {self.name}' + (' dependency-only' if depdendency_only else '')):
            for dependency in self.depends:
                run_task(dependency, self.name)
            if not depdendency_only:
                self.fn(self)
                _ran_tasks.append(self.name)

    def run_subtasks(self):
        for _, task in self.subtasks.items():
            task.run()


# decorator
def task(depends: List[str] = []):
    def task_internal(fn: Callable):
        _tasks[fn.__name__] = Task(fn.__name__, depends, fn)
    return task_internal


# decorator
def subtask(parent_task: str):
    def subtask_internal(fn: Callable):
        if parent_task not in _tasks:
            die(f'{ERROR}: Cannot add a subtask to a nonexistant task "{parent_task}"')
        _tasks[parent_task].subtasks[fn.__name__] = Subtask(parent_task, fn.__name__, fn)
    return subtask_internal


def calculate_progress_increment_sub(task_name: str, subtask_name: str, total: int = 100) -> int:
    checked_tasks = []

    def _get_count(task_name: str) -> int:
        if task_name not in _tasks:
            die(f'{ERROR}: Cannot find task "{task_name}"')
        count = 1 + len(_tasks[task_name].subtasks)
        for t in _tasks[task_name].depends:
            if t not in checked_tasks:
                count += _get_count(t)
                checked_tasks.append(t)
        return count

    if subtask_name not in _tasks[task_name].subtasks:
        die(f'{ERROR}: Cannot find subtask "{subtask_name}" in task "{task_name}"')

    count = 1
    for depend in _tasks[task_name].depends:
        count += _get_count(depend)

    return total // count


def calculate_progress_increment(task_name: str, total: int = 100) -> int:
    checked_tasks = []

    def _get_count(task_name: str) -> int:
        if task_name not in _tasks:
            die(f'{ERROR}: Cannot find task "{task_name}"')
        count = 1 + len(_tasks[task_name].subtasks)
        for t in _tasks[task_name].depends:
            if t not in checked_tasks:
                count += _get_count(t)
                checked_tasks.append(t)
        return count
    
    count = _get_count(task_name)
    return total // count
