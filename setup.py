from setuptools import setup, Extension

module1 = Extension(
    'usbrelay_py',
    libraries= ['usbrelay'],
    library_dirs= ['./','/usr/lib','/usr/lib64','/usr/lib/x86_64-linux-gnu','/usr/lib/aarch64-linux-gnu','/usr/lib/arm-linux-gnueabihf'],
    sources = ['libusbrelay_py.c']
)

setup(
    name = 'usbrelay_py',
    version = '1.0',
    description = 'USB Relay board control from Python',
    url = 'https://github.com/darrylb123/usbrelay',
    author = "Sean Mollet",
    author_email = "sean@malmoset.com",
    license = 'GPL-2.0',
    ext_modules = [module1],
    packages = ['usbrelay_py'],

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
