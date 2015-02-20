from distutils.core import setup
from distutils.extension import Extension
from Cython.Distutils import build_ext

build = "build/"

sources=[ 'src/madopt.pyx' ]
libs = ["libmadopt.a", "libmadopt_ipopt.a", "libmadopt_bonmin.a" ]
dependencies = ["ipopt", "bonmin"]

libs = [build + x for x in libs]

ext_modules = [
        Extension('madopt',
            sources=sources,
            extra_compile_args=['--std=c++11'],
            extra_objects=libs,
            libraries=dependencies,
            language="c++")
        ]

setup(
    name = 'madopt',
    version = '0.5',
    cmdclass = {'build_ext' : build_ext},
    ext_modules = ext_modules,
    packages=[''],
    url="https://github.com/stanle/madopt",
    author="Karsten Lehmann",
    license="Apache License Version 2.0"
)
