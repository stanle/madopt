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
#ifndef MADOPT_SIMSTACK
#define MADOPT_SIMSTACK

#include "stack.hpp"
#include "common.hpp"
#include "jac_simstack.hpp"
#include "hess_simstack.hpp"
#include <vector>

namespace MadOpt {

class SimStack: public Stack {
    public:
	SimStack(): dummy(0), _size(0), _max_size(0), data_i(0){}

        void doAdd(const Idx& nofelems);
        void doMull(); 
        double& lastG();
        void doUnaryOp(const double& jac_value, const double& hess_value);
        void emplace_back(const Idx& id);
        void emplace_back(const double& value);
        void clear();

        void setXSize(const Idx&);

        Idx size();

        const Idx& max_g_size()const;
        const Idx& max_jac_size()const;
        const Idx& max_hess_size()const;

        vector<Idx> getJacEntries();
        vector<PII> getHessEntries();

        void setConflicts(Array<Idx>* c);

        Idx& getDataI();

        string str();

    private:
        double dummy;
        JacSimStack jac_stack;
        HessSimStack hess_stack;
        Idx _size;
        Idx _max_size;
        Idx data_i;
};
}
#endif

