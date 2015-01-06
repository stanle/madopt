Madopt
======
Madopt = Modelling and Automatic Differentiation for Optimisation

What it is
==========

Madopt is a c++/python modelling interface for the MINLP solver [Bonmin](https://projects.coin-or.org/Bonmin) and the NLP solver [Ipopt](https://projects.coin-or.org/Ipopt).

Madopt implements (c++):
- expression building using operator overloading. 
- automatic differentiation for expressions.
- an interface to the solvers Ipopt and Bonmin.
- a python wrapper using [cython](http://cython.org/).

Supported Operators are 
+, +=, *, *=, /, -, pow, sin, cos, tan, sqrt.

Dependencies
============
neccessary:
- c++11 std
- Ipopt

optional:
- Bonmin

Install C++
===========
Running
```
mkdir build
cd build
cmake ..
make
make install
```
creates and installs static libraries called **libmadopt.a** and **libmadopt_ipopt.a**.
If libbonmin is found on the system then the static library **libmadopt_bonmin.a**
will be build and installed as well.
To build madopt using a custom path to the ipopt/bonmin libraries and headers
use
```
cmake -DCUSTOM_LIBRARY=/path/to/bonmin/libs -DCUSTOM_INCLUDES=/path/to/bonmin/includes ..
```
An example for the usage can be found in [examples/get_started.cpp](examples/get_started.cpp)
If Ipopt is complied without HSL, to compile and run the example run
```
g++ ../examples/get_started.cpp --std=c++11 -lmadopt_ipopt -lmadopt -lipopt -lcoinmetis -llapack -lcoinmumps -lpthread -ldl -lm -lz -lbz2 -lblas -o get_started
./get_started
```
If Ipopt is complied with HSL, to compile and run the example use
```
g++ ../examples/get_started.cpp --std=c++11 -lmadopt_ipopt -lmadopt -lipopt -lcoinhsl -lcoinmetis -llapack -lcoinmumps -lpthread -ldl -lm -lz -lbz2 -lblas -o get_started
./get_started
```

Install Python
==============
The python interface depends on [cython](http://cython.org/). 
Madopt can be complied for python 2 or 3 as long as the right cython version is present.
To use the python interface it is not neccessary to build and/or install the
C++ libraries.
A module object ''madopt.so'' that can be copied to wherever it may be used is
build by running
```
python setup.py build_ext --inplace
```
To install the module into python globally run
```
python setup.py install
```
An example for the usage can be found in [examples/get_started.py](examples/get_started.py). To run it call
```
python examples/get_started.py
```

License
=======

Apache License Version 2.0

Citing
======

If you are using madopt for your research, please consider citing the Ipopt project. The details on how to cite it can be found on the Ipopt [website](https://projects.coin-or.org/Ipopt).

