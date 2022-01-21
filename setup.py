from setuptools import setup, Extension

module1 = Extension('usbrelay_py',
                    libraries= ['usbrelay'],
                    library_dirs= ['./','/usr/lib','/usr/lib64','/usr/lib/x86_64-linux-gnu','/usr/lib/aarch64-linux-gnu','/usr/lib/arm-linux-gnueabihf'],
                    sources = ['libusbrelay_py.c'])

setup (name = 'usbrelay_py',
       version = '1.0',
       description = 'USB Relay board control from Python',
       author = "Sean Mollet",
       author_email = "sean@malmoset.com",
       ext_modules = [module1])
