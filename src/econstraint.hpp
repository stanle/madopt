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
#ifndef MADOPT_ECONSTRAINT_H
#define MADOPT_ECONSTRAINT_H

#include <stdlib.h>
#include <cmath>
#include <set>
#include <vector>
#include "operator.hpp"
#include "inner_constraint.hpp"

namespace MadOpt {

class Expr;
class ADStack;
class ADStackElem;

class EConstraint: public InnerConstraint{
    public:

        EConstraint(const Expr& expr, const double _lb, const double _ub, ADStack& stack);

        EConstraint(const Expr& expr, ADStack& stack);

        double lb();

        void lb(double v);

        double ub();

        void ub(double v);

        Idx getNNZ_Jac();

        void init(HessPosMap& hess_pos_map);

        vector<Idx> getJacEntries();

        void getNZ_Jac(unsigned int* jCol);

        void setEvals(const double* x);

        string opsToString()const;

        const string toString() const ;

    private:
        double _lb;
        double _ub;

        vector<uintptr_t> data;
        vector<OPType> operators;

        ADStack& stack;

        set<Idx> getVarsSet();

        inline double getX(const double* x, Idx index)const;

        void computeFinalStack(const double* x=nullptr);

        inline void caseADD(const Idx& counter);

        inline void caseMUL(const Idx& counter);

        inline void caseSIN();

        inline void caseCOS();

        inline void caseTAN();

        inline void casePOW(const double& value);

        inline void doHessJacs(ADStackElem& top, double frst, double scd);

        inline double getNextValue(Idx& idx); 

        inline Idx getNextCounter(Idx& idx);

        inline Idx getNextPos(Idx& idx);

        inline double getNextParamValue(Idx& idx);
};

}
#endif
/* ex: set tabstop=4 shiftwidth=4 expandtab: */
