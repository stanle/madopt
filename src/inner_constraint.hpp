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
#ifndef MADOPT_INNER_CONSTRAINT_H
#define MADOPT_INNER_CONSTRAINT_H

#include <set>
#include <vector>
#include "common.hpp"
#include "array.hpp"

namespace MadOpt {

class Solution;
class Expr;
typedef char OPType;
class Stack;
class CStack;
class SimStack;

class InnerConstraint{
    public:
        InnerConstraint(const Expr& expr, const double _lb, const double _ub,
                HessPosMap& hess_pos_map, SimStack& stack);

        InnerConstraint(const Expr& expr, HessPosMap& hess_pos_map, SimStack& stack);

        // bounds
        //
        //
        double lb();

        void lb(double v);

        double ub();

        void ub(double v);

        // init
        //
        //
        Idx getNNZ_Jac();

        void getNZ_Jac(unsigned int* jCol);

        // compute next point
        //
        //
        void setEvals(CStack&);

        // access next points solution
        //
        //
        const double& getG()const ;

        const vector<double>& getJac()const ;

        void eval_h(double* values, const double& lambda);

        // for debug and testing
        //
        //
        const vector<double>& getHess()const ;

        const vector<Idx>& getHessMap()const;

        const vector<Idx>& getJacEntries();

    private:
        vector<double> jac;

        vector<double> hess;

        vector<Idx> hess_map;

        vector<Idx> jac_entries;

        vector<OPType> operators;

        vector<uintptr_t> data;

        double g;

        double _lb;

        double _ub;

        Array<Idx> conflicts;

        inline const double& getNextValue(Idx& idx); 

        inline Idx getNextCounter(Idx& idx);

        inline const Idx& getNextPos(Idx& idx);

        inline const double& getNextParamValue(Idx& idx);

        void computeFinalStack(Stack&);

        void caseVAR_POINTER(Stack&);

        void caseSQR_VAR(Stack&);

        void caseADD(Stack&);

        void caseMUL(Stack&);

        void casePARAM_POINTER(Stack&);

        void caseCONST(Stack&);

        void casePOW(Stack&);

        void caseSIN(Stack&);

        void caseCOS(Stack&);

        void caseTAN(Stack&);
};
}
#endif
