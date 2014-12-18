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
    top(elems.begin())
    {}

ADStack::~ADStack(){ 
    elems.clear(); 
}

inline void ADStack::ensureElemOnTop(){
    if (top == elems.end()){
        elems.emplace_back(jac_mempool, hess_mempool);
        top = elems.end() - 1;
    }
}

ADStackElem& ADStack::back(Idx idx) { 
    ASSERT(top != elems.begin());
    return *(top-1-idx);
}

const ADStackElem& ADStack::back(Idx idx)const { 
    ASSERT(top != elems.end());
    return *(top-1-idx);
}

deque<ADStackElem>::iterator ADStack::backIterator(Idx idx){
    return top-1-idx;
}

void ADStack::emplace_back(const double& g, const Idx& idx){
    ensureElemOnTop();
    top->emplace(g,idx);
    top++;
}

void ADStack::emplace_back(const double& g){
    ensureElemOnTop();
    top->g = g;
    top++;
}

void ADStack::emplace_backSQR(const double& g, const Idx& idx){ 
    ensureElemOnTop();
    top->emplaceSQR(g, idx);
    top++;
}

void ADStack::pop_back(Idx counter) { 
    for (Idx i=0; i<counter; i++){
        assert(top != elems.begin());
        top--; 
        top->clear();
    }
}

void ADStack::clear() { 
    for (auto iter=elems.begin(); iter!=top; iter++)
        iter->clear();
    top = elems.begin();
    assert(jac_mempool.full());
    assert(hess_mempool.full());
}

string ADStack::toString()const{
    string res;
    int i=0;
    for (auto iter=elems.begin(); iter!=top; iter++)
        res += std::to_string(i++) + ": " + iter->toString() + "\n";
    return res;
}

Idx ADStack::size()const{
    Idx i=0;
    for (auto iter=elems.begin(); iter!=top; iter++)
        i++;
    return i;
}

bool ADStack::empty()const{
    return (top == elems.begin());
}

void ADStack::optimizeAlignment(){
    jac_mempool.optimizeAlignment();
    hess_mempool.optimizeAlignment();
}

}
