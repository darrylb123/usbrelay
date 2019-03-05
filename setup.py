from distutils.core import setup, Extension

module1 = Extension('usbrelay_py',
                    libraries= ['usbrelay'],
                    library_dirs= ['./','/usr/lib','/usr/lib64'],
                    sources = ['libusbrelay_py.c'])

setup (name = 'usbrelay_py',
       version = '1.0',
       description = 'USB Relay board control from Python',
       author = "Sean Mollet",
       author_email = "sean@malmoset.com",
       ext_modules = [module1])