from distutils.core import setup
from distutils.extension import Extension
from Cython.Distutils import build_ext

bonmin_libs = [
    'ipopt',
    'bonmin',
    'Cbc',
    'Cgl',
    'Clp',
    'CoinUtils',
    'Osi',
    'OsiCbc',
    'OsiClp',
    'coinhsl',
    'dl',
    'pthread',
    'blas',
    'lapack',
    'coinmetis',
    'coinmumps',
    'z',
    'bz2',
]

libs = bonmin_libs

ext_modules = [
        Extension('madopt',
			sources=[
				'src/madopt.pyx',
                                'src/ipopt_model.cpp',
                                'src/bonmin_model.cpp',
				'src/model.cpp',
				'src/common.cpp',
				'src/expr.cpp',
				'src/ipopt_nlp.cpp',
				'src/bonmin_minlp.cpp',
                                'src/adstack.cpp',
                                'src/adstackelem.cpp',
                                'src/inner_constraint.cpp',
                                'src/constraint.cpp',
                                'src/inner_var.cpp',
                                'src/solution.cpp',
                                'src/var.cpp',
				],
			extra_compile_args=['--std=c++11', '-g'],
			libraries=libs,
			language="c++")
        ]

setup(
    name = 'madopt',
    version = '0.1',
    cmdclass = {'build_ext' : build_ext},
    ext_modules = ext_modules,
    packages=['']
)
