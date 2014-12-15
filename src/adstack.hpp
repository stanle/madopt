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

#include <deque>

#include "mempool.hpp"
#include "adstackelem.hpp"

namespace MadOpt {

class ADStack{
    public:
        ADStack(Idx initsize=0, Idx init_jac_nodes=0, Idx init_hess_nodes=0);

        ~ADStack();

        inline void ensureElemOnTop();

        ADStackElem& back(Idx idx=0) ;

        const ADStackElem& back(Idx idx=0)const ;

        deque<ADStackElem>::iterator backIterator(Idx idx=0);

        void emplace_back(const double& g, const Idx& idx);

        void emplace_back(const double& g);
        
        void emplace_backSQR(const double& g, const Idx& idx);

        void pop_back(Idx counter=1) ;
       
        void clear() ;

        string toString()const;

        Idx size()const;

        bool empty()const;

        void fixSize();

    private:
        JacMemPool jac_mempool;
        HessMemPool hess_mempool;
        deque<ADStackElem> elems;
        deque<ADStackElem>::iterator top;
};

}
#endif
/* ex: set tabstop=4 shiftwidth=4 expandtab: */
