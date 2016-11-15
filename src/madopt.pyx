#
# Copyright 2014 National ICT Australia Limited (NICTA)
# 
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
# 
#     http://www.apache.org/licenses/LICENSE-2.0
# 
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
#
import signal
from libcpp.string cimport string
from libcpp cimport bool

cdef extern from "model.hpp":
    cdef cppclass MadOptError:
        pass 

    cdef double INF "MadOpt::INF"

    cdef cppclass Expr_ "MadOpt::Expr":
        Expr_()
        Expr_(double)
        void plusEqual(Expr_&)
        void mulEqual(Expr_&)
        Expr_& operator+(Expr_&)
        Expr_& operator*(Expr_&)
        Expr_& operator-(Expr_&)
        Expr_& operator/(Expr_&)
        Expr_& operator-()
        string toString()
        double xe "x"()

    cdef Expr_ esin "MadOpt::sin" (Expr_&)

    cdef Expr_ ecos "MadOpt::cos" (Expr_&)

    cdef Expr_ epow "MadOpt::pow" (Expr_&, double)

    cdef cppclass Var_ "MadOpt::Var"(Expr_):
        Var_()
        bool fixed()
        void fixed(bool)
        void solAsInit()
        double lb()
        double ub()
        void lb(double) except +
        void ub(double) except +
        string name()
        double x()
        double v()
        double init()
        void init(double) except +

    cdef cppclass Param_ "MadOpt::Param"(Expr_):
        Param_()
        double value()
        void value(double)

    cdef cppclass Constraint_ "MadOpt::Constraint":
        double lb()
        double ub()
        double lam()
        void lb(double)
        void ub(double)

    cdef cppclass Model_ "MadOpt::Model":
        void solAsInit()
        bool show_solver
        double timelimit
        void solve()
        int status()
        double objValue()
        Var_ addCVar(double, double, double, string)
        Var_ addIVar(double, double, double, string)
        Var_ addBVar(double, string)
        Param_ addParam(double, string)
        void setNumericOption(string, double)
        void setIntegerOption(string, int)
        void setStringOption(string, string)
        void setObj(Expr_&)
        int nx()
        int ng()
        int np()
        Constraint_ addConstr(double, Expr_&, double)
        bool hasSolution()

ctypedef double (*g_type)(void *param, void *g_data)

cdef extern from "python_callback.hpp":
    cdef cppclass PythonCallback:
        PythonCallback(g_type gtype, void *g_data)

cdef double callback_template(void *parameter, void *method):
    return (<object>method)(<object>parameter)

INFINITY = INF

def convert(e):
    if isinstance(e, (float, int)):
        return Expr(e)
    return e

def add(Expr self, Expr other):
    e = Expr()
    e.expr_ = self.expr_ + other.expr_
    return e

def mul(Expr self, Expr other):
    e = Expr()
    e.expr_ = self.expr_ * other.expr_
    return e

def sub(Expr self, Expr other):
    e = Expr()
    e.expr_ = self.expr_ - other.expr_
    return e

def div(Expr self, Expr other):
    e = Expr()
    e.expr_ = self.expr_ / other.expr_
    return e

def iadd(Expr self, Expr other):
    self.expr_.plusEqual(other.expr_)
    return self

def imul(Expr self, Expr other):
    self.expr_.mulEqual(other.expr_)
    return self


cdef class Expr:
    cdef Expr_ expr_

    def __cinit__(self, double d=0):
        self.expr_ = Expr_(d)

    @property
    def x(self):
        return self.expr_.xe()

    def __add__(self, other):
        return add(convert(self), convert(other))

    def __mul__(self, other):
        return mul(convert(self), convert(other))

    def __pow__(Expr self, double expo, mod):
        e = Expr()
        e.expr_ = epow(self.expr_, expo)
        return e

    def __sub__(self, other):
        return sub(convert(self), convert(other))

    def __neg__(Expr self):
        e = Expr()
        e.expr_ = -self.expr_
        return e

    def __div__(self, other):
        return div(convert(self), convert(other))

    def __truediv__(self, other):
        return div(convert(self), convert(other))

    def __iadd__(self, other):
        return iadd(convert(self), convert(other))

    def __imul__(self, other):
        return imul(convert(self), convert(other))

    def toString(self):
        return self.expr_.toString()

    def __str__(self):
        return self.toString().decode("UTF-8")


def sin(Expr ip):
    e = Expr()
    e.expr_ = esin(ip.expr_)
    return e


def cos(Expr ip):
    e = Expr()
    e.expr_ = ecos(ip.expr_)
    return e


cdef class Var(Expr):
    cdef Var_ getVar(self):
        return (<Var_>self.expr_)

    def solAsInit(self):
        self.getVar().solAsInit()

    property fixed:
        def __get__(self):
            return self.getVar().fixed()

        def __set__(self, bool v):
            self.getVar().fixed(v)

    property init:
        def __get__(self):
            return self.getVar().init()

        def __set__(self, double v):
            self.getVar().init(v)

    property lb:
        def __get__(self):
            return self.getVar().lb()

        def __set__(self, double s):
            self.getVar().lb(s)

    property ub:
        def __get__(self):
            return self.getVar().ub()

        def __set__(self, double s):
            self.getVar().ub(s)

    @property
    def x(self):
        return self.getVar().x()

    @property
    def v(self):
        return self.getVar().v()

cdef class Param(Expr):
    cdef Param_ getParam(self):
        return (<Param_>self.expr_)

    property value:
        def __get__(self):
            return self.getParam().value()

        def __set__(self, double v):
            self.getParam().value(v)

cdef class Constraint:
    cdef Constraint_ constraint_

    property lb:
        def __get__(self):
            return self.constraint_.lb()

        def __set__(self, double v):
            self.constraint_.lb(v)

    property ub:
        def __get__(self):
            return self.constraint_.ub()

        def __set__(self, double v):
            self.constraint_.ub(v)

    @property
    def lam(self):
        return self.constraint_.lam()

cdef class CustomConstraint:
    def __init__(self, g_func, jac_func, hess_func):
        self.g_func = g_func
        self.jac_func = jac_func
        self.hess_func = hess_func

cdef class Model:
    cdef Model_* model_

    def __init__(self, timelimit=-1, show_solver=False):
        signal.signal(signal.SIGINT, signal.SIG_DFL)
        self.model_.show_solver = show_solver
        self.model_.timelimit = timelimit

    def solve(self):
        self.model_.solve()

    # add Variables
    #
    #
    def addVar(self, double lb=-INF, double ub=INF, init=None, name=None):
        return self.addCVar(lb, ub, init, name)

    def addCVar(self, double lb=-INF, double ub=INF, init=None, name=None):
        init = init or max(lb, min(ub, 0.0))
        name = name or ('v' + str(self.model_.nx()))
        e = Var()
        e.expr_ = self.model_.addCVar(lb, ub, init, name.encode('UTF-8'))
        return e

    def addIVar(self, lb=-INF, ub=INF, init=None, name=None):
        init = int(init or max(lb, min(ub, 0.0)))
        name = name or ('v' + str(self.model_.nx()))
        e = Var()
        e.expr_ = self.model_.addIVar(lb, ub, init, name.encode('UTF-8'))
        return e

    def addBVar(self, init=None, name=None):
        init = init or 1.0
        name = name or ('v' + str(self.model_.nx()))
        e = Var()
        e.expr_ = self.model_.addBVar(init, name.encode('UTF-8'))
        return e

    # add Param
    #
    #
    def addParam(self, double v, name=None):
        name = name or ('p' + str(self.model_.np()))
        e = Param()
        e.expr_ = self.model_.addParam(v, name.encode('UTF-8'))
        return e

    # set Objective
    #
    #
    def setObj(self, Expr expr):
        self.model_.setObj(expr.expr_)

    # add Constraint
    #
    #
    def addConstr(self, Expr expr, double lb=-INFINITY, double ub=INFINITY):
        c = Constraint()
        c.constraint_ = self.model_.addConstr(lb, expr.expr_, ub)
        return c

    def addEqConstr(self, Expr expr, double eq=0):
        return self.addConstr(expr, lb=eq, ub=eq)

    # get Solution
    #
    #
    @property
    def obj(self):
        return self.model_.objValue()

    @property
    def obj_value(self):
        return self.model_.objValue()

    @property
    def objValue(self):
        return self.model_.objValue()

    @property
    def status(self):
        return self.model_.status()

    def solAsInit(self):
        self.model_.solAsInit()

    # get info
    #
    #
    property nx:
        def __get__(self):
            return self.model_.nx()

    property ng:
        def __get__(self):
            return self.model_.ng()

    property stat:
        def __get__(self):
            return self.model_.status()

    property show_solver:
        def __get__(self):
            return self.model_.show_solver

        def __set__(self, bool value):
            self.model_.show_solver = value

    property timelimit:
        def __get__(self):
            return self.model_.timelimit

        def __set__(self, double value):
            self.model_.timelimit = value

    property has_solution:
        def __get__(self):
            return self.model_.hasSolution()

    # set Option
    #
    #
    def setOption(self, key, value):
        key = key.encode('UTF-8')
        if isinstance(value, str):
            value = value.encode('UTF-8')
            self.model_.setStringOption(key, value)
        elif isinstance(value, int):
            self.model_.setIntegerOption(key, value)
        elif isinstance(value, float):
            self.model_.setNumericOption(key, value)

cdef extern from "ipopt_model.hpp":
    cdef cppclass IpoptModel_ "MadOpt::IpoptModel"(Model_):
        IpoptModel_()

cdef class IpoptModel(Model):
    def __cinit__(self):
        self.model_ = new IpoptModel_()

    def __dealloc__(self):
        del self.model_

cdef extern from "bonmin_model.hpp":
    cdef cppclass BonminModel_ "MadOpt::BonminModel"(Model_):
        BonminModel_()

cdef class BonminModel(Model):
    def __cinit__(self):
        self.model_ = new BonminModel_()

    def __dealloc__(self):
        del self.model_

# ex: set tabstop=4 shiftwidth=4 expandtab:
