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

#include "adstack.hpp"
#include "common.hpp"
#include "logger.hpp"

namespace MadOpt {

ADStack::ADStack(Idx initsize, Idx init_jac_nodes, Idx init_hess_nodes):
    jac_mempool(init_jac_nodes), 
    hess_mempool(init_hess_nodes),
    elems(initsize, ADStackElem(jac_mempool, hess_mempool)),
    top_idx(0)
    {
        TRACE("constructor");
    }

ADStack::~ADStack(){ 
    elems.clear(); 
}

inline void ADStack::ensureElemOnTop(){
    TRACE_START;
    ASSERT_BETWEEN(0, top_idx, elems.size());
    if (top_idx == elems.size()){
        elems.emplace_back(jac_mempool, hess_mempool);
        top_idx = elems.size()-1;
    }
    TRACE_END;
}

ADStackElem& ADStack::back(Idx idx) { 
    ASSERT_BETWEEN(1, top_idx, elems.size());
    ASSERT_BETWEEN(0, idx, size()-1);
    return elems[top_idx-1-idx];
}

const ADStackElem& ADStack::back(Idx idx)const { 
    ASSERT_BETWEEN(1, top_idx, elems.size());
    ASSERT_BETWEEN(0, idx, size()-1);
    return elems[top_idx-1-idx];
}

void ADStack::emplace_back(const double& g, const Idx& idx){
    TRACE_START;
    ensureElemOnTop();
    elems[top_idx].emplace(g, idx);
    top_idx++;
    ASSERT_BETWEEN(1, top_idx, elems.size());
    TRACE_END;
}

void ADStack::emplace_back(const double& g){
    TRACE_START;
    ensureElemOnTop();
    elems[top_idx].g = g;
    top_idx++;
    ASSERT_BETWEEN(1, top_idx, elems.size());
    TRACE_END;
}

void ADStack::emplace_backSQR(const double& g, const Idx& idx){ 
    ensureElemOnTop();
    elems[top_idx].emplaceSQR(g, idx);
    top_idx++;
    ASSERT_BETWEEN(1, top_idx, elems.size());
}

void ADStack::pop_back(Idx counter) { 
    TRACE_START;
    ASSERT_LE(counter,  size());
    for (Idx i=0; i<counter; i++){
        ASSERT_UEQ(top_idx, 0);
        top_idx --;
        elems[top_idx].clear();
    }
    ASSERT_BETWEEN(0, top_idx, elems.size());
    TRACE_END;
}

void ADStack::clear() { 
    TRACE_START;
    TRACE("stack=", toString());
    for (Idx i=0; i<top_idx; i++)
        elems[i].clear();
    top_idx = 0;
    ASSERT(jac_mempool.full());
    ASSERT(hess_mempool.full());
    TRACE_END;
}

string ADStack::toString()const{
    string res;
    for (Idx i=0; i<top_idx; i++)
        res += std::to_string(i) + ": " + elems[i].toString() + "\n";
    return res;
}

Idx ADStack::size()const{
    return top_idx;
}

bool ADStack::empty()const{
    return (top_idx == 0);
}

void ADStack::optimizeAlignment(){
    jac_mempool.optimizeAlignment();
    hess_mempool.optimizeAlignment();
}

}
