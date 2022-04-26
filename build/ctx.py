
from . import config, cache, cpp
import os

def init(config_file: str = 'config.json'):
    config.load(config_file)
    if not os.path.exists(config.get('build_dir')):
        os.mkdir(config.get('build_dir'))
    cache.load()
    cpp.init()

def finalize():
    cache.update()
    cache.save()

class context:
    def __init__(self, config_file = 'config.json'):
        self.config_file = config_file

    def reload(self):
        if not os.path.exists(config.get('build_dir')):
            os.mkdir(config.get('build_dir'))
        cache.load()
        cpp.init()

    def __enter__(self):
        init(self.config_file)
        return self

    def __exit__(self, exc_type, exc_value, traceback):
        finalize()
