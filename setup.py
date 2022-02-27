#!/usr/bin/env python3
# -*- coding: utf-8 -*-

import glob
import platform
import distutils.sysconfig

try:
    from setuptools import setup, Extension
except ImportError:
    from distutils.core import setup, Extension


def remove_unwanted_options():
    cfg_vars = distutils.sysconfig.get_config_vars()

    for key, value in cfg_vars.items():
        if type(value) == str and cfg_vars[key].startswith('-'):
            cfg_vars[key] = '' #value.replace('-Wstrict-prototypes', '')


remove_unwanted_options()

orderbook = Extension('orderbook',
    language = 'c++',
    extra_compile_args = [ '-Wall', '-pedantic', '-fPIC', '-std=c++14' ],
    libraries = [ 'boost_python' ],
    include_dirs = [ '/usr/include/python' + '.'.join(platform.python_version_tuple()[:2]), 'src' ],
    sources = (
        glob.glob('src/*.cpp')
    )
)

setup(
    name = 'orderbook',
    version = '1.0.0',
    description = 'Orderbook engine',
    url = 'https://github.com/pavelschon/OrderBook.git',
    author ='Pavel Schön',
    author_email = 'pavel@schon.cz',
    requires = [ 'twisted' ],
    scripts = glob.glob('bin/*.py'),
    ext_modules = [ orderbook ],
    data_files = [
        ( 'orderbook/tests', glob.glob('test/*.py') ),
        ( 'orderbook/', [ 'README.md' ] ),
    ]
)
