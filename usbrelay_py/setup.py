# Copyright (C) 2019  Sean Mollet

# This program is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 2 of the License, or
# (at your option) any later version.

# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.

# You should have received a copy of the GNU General Public License along
# with this program; if not, write to the Free Software Foundation, Inc.,
# 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.

from setuptools import setuptools, setup, Extension

module1 = Extension(
    'usbrelay_py',
    libraries= ['usbrelay'],
    library_dirs= ['../'],
    include_dirs= ['../'],
    sources = ['src/libusbrelay_py.c']
)

setup(
    name = 'usbrelay_py',
    version = '1.0',
    description = 'USB Relay board control from Python',
    url = 'https://github.com/darrylb123/usbrelay',
    author = "Sean Mollet",
    author_email = "sean@malmoset.com",
    license = 'GPL-2.0-or-later',
    ext_modules = [module1],
    package_dir={"": "src"},
    packages=setuptools.find_packages(where="src"),
    # packages = ['usbrelay_py'],

    classifiers = [
        'Intended Audience :: Science/Research',
        'License :: OSI Approved :: GNU General Public License v2 (GPLv2)',
        'Operating System :: POSIX :: Linux',
        'Programming Language :: Python :: 3',
        'Programming Language :: Python :: 3.6',
        'Programming Language :: Python :: 3.7',
        'Programming Language :: Python :: 3.8',
        'Programming Language :: Python :: 3.9',
        'Programming Language :: Python :: 3.10',
    ],

)
