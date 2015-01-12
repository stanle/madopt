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
#ifndef MADOPT_STACK_H
#define MADOPT_STACK_H

#include <vector>

#include "mempool.hpp"
#include "adstackelem.hpp"

namespace MadOpt {

class ADStack{
    public:
        ADStack(const ADStack&);

        ADStack(Idx initsize=0, Idx init_jac_nodes=0, Idx init_hess_nodes=0);

        ~ADStack();

        ADStackElem& back(Idx idx=0);

        const ADStackElem& back(Idx idx=0)const ;

        void emplace_back(const double& g, const Idx& idx);

        void emplace_back(const double& g);
        
        void emplace_backSQR(const double& g, const Idx& idx);

        ADStackElem& pop_back() ;

        void clear() ;

        string toString()const;

        Idx size()const;

        Idx capacity()const;

        Idx jac_mempool_capacity()const;

        Idx hess_mempool_capacity()const;

        void reserve(Idx new_size, Idx new_jac_size=0, Idx new_hess_size=0);

        bool empty()const;

        void optimizeAlignment();

        // for testing an algo
        const double* x;

        // for testing an algo
        Idx data_i = 0;

    private:
        JacMemPool jac_mempool;
        HessMemPool hess_mempool;
        vector<ADStackElem> elems;
        Idx top_idx;

        void ensureElemOnTop();
};

}
#endif
/* ex: set tabstop=4 shiftwidth=4 expandtab: */
