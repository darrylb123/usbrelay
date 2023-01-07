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

from setuptools import setuptools, setup

setup(
    name = 'usbrelay_py',
    version = '1.1',
    description = 'USB Relay board control from Python',
    url = 'https://github.com/darrylb123/usbrelay',
    author = "Sean Mollet",
    author_email = "sean@malmoset.com",
    license = 'GPL-2.0-or-later',
    packages = ['usbrelay_py'],
    package_dir={'usbrelay_py': 'src'},
    package_data = {'usbrelay_py': ['src/*']},

    classifiers = [
        'Intended Audience :: Science/Research',
        'License :: OSI Approved :: GNU General Public License v2 (GPLv2)',
        'Operating System :: POSIX :: Linux',
    ],

)
