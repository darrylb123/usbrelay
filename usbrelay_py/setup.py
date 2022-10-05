from setuptools import setuptools, setup, Extension
exec(open('../LIBVER.in').read())

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
    license = 'GPL-2.0',
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
