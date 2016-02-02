Madopt
======
Madopt = **M**odelling and **A**utomatic **D**ifferentiation for **Opt**imisation

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
- c++11 std
- Bonmin >= 1.8
(Bonmin includes Ipopt)

Install
=======
Running
```
mkdir build
cd build
cmake ..
make
```
creates static libraries called **libmadopt.a**, **libmadopt_ipopt.a** and **libmadopt_bonmin.a**.
If ipopt/bonmin is installed in a custom location, one can provide the **absolute** path to the ipopt/bonmin libraries and headers via

```
cmake -DCUSTOM_LIBRARY=/path/to/bonmin/libs -DCUSTOM_INCLUDE=/path/to/bonmin/includes ..
```

To be able to use the C++ version of madopt one has to install the libraries by running
```
make install
```

The python interface depends on [cython](http://cython.org/). 
Madopt can be complied for python 2 or 3 as long as the right cython version is present.
To use the python interface it is not neccessary to install the C++ libraries, however they have to be build.
To build and install the python module run the [setup](setup.py) file in the base path of madopt. For example after doing the build run
```
cd ..
python setup.py install
```

To build a python module object ''madopt.so'' that can be copied to wherever it may be used run
```
cd ..
python setup.py build_ext --inplace
```

Examples
========
An example for the usage with **C++** can be found in [examples/get_started.cpp](examples/get_started.cpp).

An example for the usage with **python** can be found in [examples/get_started.py](examples/get_started.py). To run it call
```
python examples/get_started.py
```

License
=======

Apache License Version 2.0

Citing
======

If you are using madopt for your research, please consider citing the Ipopt project. The details on how to cite it can be found on the Ipopt [website](https://projects.coin-or.org/Ipopt).
