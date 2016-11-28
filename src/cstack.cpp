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

#include "cstack.hpp"
#include "simstack.hpp"
#include "logger.hpp"

namespace MadOpt {

void CStack::doAdd(const Idx& nofelems){
    TRACE_START;
    ASSERT_LE(nofelems, size());
    ASSERT_LE(nofelems, g_stack.size());
    ASSERT_LE(2, nofelems);

    jac_stack.merge(nofelems);
    hess_stack.merge(nofelems);
    double& goal = g_stack.back(nofelems);
    for (Idx i=0; i<nofelems-1; i++)
        goal += g_stack.pop();
}

void CStack::doMull(){
    TRACE_START;
    ASSERT_LE(2, g_stack.size());

    double& last = g_stack.pop();
    double& prev = g_stack.back();
    TRACE("last=", last, "prev=", prev);
    hess_stack.mulAllLast(prev);
    hess_stack.mulAllPrev(last);

    const auto& stack = jac_stack.getStack();
    const auto& pos = jac_stack.getPos();
    for (Idx i=pos.back(1); i<stack.size(); i++)
        for (Idx k=pos.back(2); k<pos.back(1); k++){
            TRACE(i, k);
            hess_stack.push(stack[i]*stack[k]);
        }

    TRACE("conf elems", conflicts->str());
    Idx& counter = conflicts->next();
    for (Idx i=0; i<counter; i++){
        TRACE("sol 00 conf", conflicts->current());
        hess_stack.getStack()[conflicts->next()] *= 2;
    }

    hess_stack.merge(2);

    jac_stack.mulAllLast(prev);
    jac_stack.mulAllPrev(last);
    jac_stack.merge(2);
    prev *= last;
    TRACE_END;
}

double& CStack::lastG(){
    TRACE_START;
    return g_stack.back();
}

void CStack::doUnaryOp(const double& jac_value, const double& hess_value){
    TRACE_START;
    hess_stack.mulAllLast(jac_value);
    const auto& stack = jac_stack.getStack();
    const auto& pos = jac_stack.getPos();
    hess_stack.emplace_back_empty();
    for (Idx i=pos.back(); i<stack.size(); i++)
        for (Idx k=i; k<stack.size(); k++)
            hess_stack.push(stack[i]*stack[k]*hess_value);
    hess_stack.merge(2);
    jac_stack.mulAllLast(jac_value);
    TRACE_END;
}

void CStack::emplace_back(const Idx& id){
    TRACE_START;
    g_stack.pushSave(x[id]);
    jac_stack.emplace_back(1);
    hess_stack.emplace_back_empty();
}

void CStack::emplace_back(const double& value){
    TRACE_START;
    g_stack.pushSave(value);
    jac_stack.emplace_back_empty();
    hess_stack.emplace_back_empty();
}

void CStack::clear(){
    g_stack.clear();
    jac_stack.clear();
    hess_stack.clear();
    data_i = 0;
}

void CStack::fill(double& g, double* jac, double* hess){
    //ASSERT_XOR(jac != nullptr, jac_stack.stackSize() > 0);
    //ASSERT_XOR(hess != nullptr, hess_stack.stackSize() > 0);
    g = g_stack.back();
    jac_stack.fill(jac);
    hess_stack.fill(hess);
}

void CStack::resize(const SimStack& simstack){
    TRACE_START;
    TRACE("simstack sizes=",
            simstack.max_g_size(), 
            simstack.max_jac_size(), 
            simstack.max_hess_size());
    g_stack.resize(simstack.max_g_size());
    jac_stack.resize(simstack.max_jac_size(), simstack.max_g_size());
    hess_stack.resize(simstack.max_hess_size(), simstack.max_g_size()+1);
    TRACE_END;
}

Idx CStack::size(){
    return g_stack.size();
}

void CStack::setConflicts(Array<Idx>* conflicts){
    conflicts->reset();
    jac_stack.setConflicts(conflicts);
    hess_stack.setConflicts(conflicts);
    this->conflicts = conflicts;
}

void CStack::setX(const double* xx){
    x = xx;
}

Idx& CStack::getDataI(){
    return data_i;
}

}
