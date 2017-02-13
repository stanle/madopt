/*
 * Copyright 2014 National ICT Australia Limited (NICTA)
 * 
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 * 
 *     http://www.apache.org/licenses/LICENSE-2.0
 * 
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
#ifndef MADOPT_CONSTRAINT_INTERFACE_H
#define MADOPT_CONSTRAINT_INTERFACE_H

#include "common.hpp"
#include "array.hpp"

namespace MadOpt {

class Solution;
class Expr;
typedef char OPType;
class Stack;
class CStack;
class SimStack;

class ConstraintInterface{
public:
  virtual ~ConstraintInterface() {}
    virtual double lb()= 0;
    virtual void lb(double v) = 0;
    virtual double ub() = 0;
    virtual void ub(double v) = 0;
    virtual Idx getNNZ_Jac() = 0;
    virtual void getNZ_Jac(unsigned int* jCol) = 0;
    virtual void setEvals(CStack&){}
    virtual const double& getG()const = 0;
    virtual const vector<double>& getJac()const = 0;
    virtual void eval_h(double* values, const double& lambda) = 0;
};
}
#endif
