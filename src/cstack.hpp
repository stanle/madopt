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
#ifndef MADOPT_CSTACK
#define MADOPT_CSTACK

#include "stack.hpp"
#include "array.hpp"
#include "list_cstack.hpp"

namespace MadOpt {

class SimStack;

class CStack: public Stack {
    public:
	CStack(): data_i(0){}

        void doAdd(const Idx& nofelems);
        void doMull(); 
        double& lastG();
        void doUnaryOp(const double& jac_value, const double& hess_value);
        void emplace_back(const Idx& id);
        void emplace_back(const double& value);
        void clear();
        Idx size();

        void setConflicts(Array<Idx>* conflicts);

        void fill(double& g, double* jac, double* hess);

        void resize(const SimStack& simstack);

        void setX(const double* xx);

        Idx& getDataI();

    private:
        Array<double> g_stack;
        ListCStack jac_stack;
        ListCStack hess_stack;
        Array<Idx>* conflicts;
        const double* x;
        Idx data_i;
};
}
#endif


