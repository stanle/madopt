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

#include "vstack.hpp"
#include "logger.hpp"

namespace MadOpt {

void VStack::doADD(Idx nofelems){
    TRACE_START;
    ASSERT_LE(nofelems, size());
    jac_stack.merge(nofelems);
    hess_stack.merge(nofelems);

    double& goal = g_stack[g_stack_end-nofelems];
    for (Idx i=0; i<nofelems-1; i++){
        goal += g_stack[--g_stack_end];
    }

    ASSERT_EQ(size(), jac_stack.size());
    ASSERT_EQ(size(), hess_stack.size());
    TRACE_END;
}

void VStack::doMUL(){
    TRACE_START;
    ASSERT_LE(1, size());

    double& last = g_stack[--g_stack_end];
    double& prev = g_stack[g_stack_end-1];

    hess_stack.merge(prev, last);
    hess_stack.mergeJacInto(jac_stack);
    jac_stack.merge(prev, last);
    prev *= last;
    ASSERT_EQ(size(), jac_stack.size());
    ASSERT_EQ(size(), hess_stack.size());
    TRACE_END;
}

double& VStack::lastG(){
    return g_stack[g_stack_end-1];
}

void VStack::doUnaryOp(const double& jac_value, const double& hess_value){
    TRACE_START;
    hess_stack.mergeSingle(jac_stack, jac_value, hess_value);
    jac_stack.mulAll(jac_value);
    ASSERT_EQ(size(), jac_stack.size());
    ASSERT_EQ(size(), hess_stack.size());
    TRACE_END;
}

void VStack::emplace_back(const Idx& id){
    TRACE_START;
    DEBUG_CODE(Idx s = size());

    ensureElem();
    g_stack[g_stack_end++] = x[id];
    jac_stack.emplace_back(id, 1);
    hess_stack.emplace_back_empty();
    ASSERT_EQ(size(), s+1);
    ASSERT_EQ(size(), jac_stack.size());
    ASSERT_EQ(size(), hess_stack.size());
    TRACE_END;
}

void VStack::emplace_back(const double& value){
    TRACE_START;
    DEBUG_CODE(Idx s = size());

    ensureElem();
    g_stack[g_stack_end++] = value;
    jac_stack.emplace_back_empty();
    hess_stack.emplace_back_empty();
    ASSERT_EQ(size(), s+1);
    ASSERT_EQ(size(), jac_stack.size());
    ASSERT_EQ(size(), hess_stack.size());
    TRACE_END;
}

void VStack::clear(){
    TRACE_START;
    ASSERT_EQ(g_stack_end, jac_stack.size());
    ASSERT_EQ(g_stack_end, hess_stack.size());

    jac_stack.clear();
    hess_stack.clear();
    data_i = 0;
    g_stack_end = 0;

    ASSERT_EQ(g_stack_end, jac_stack.size());
    ASSERT_EQ(g_stack_end, hess_stack.size());
    TRACE_END;
}

Idx VStack::size(){
    ASSERT_EQ(g_stack_end, jac_stack.size());
    ASSERT_EQ(g_stack_end, hess_stack.size());
    return g_stack_end;
}

void VStack::fill(double* jac, double* hess){
    jac_stack.fill(jac);
    hess_stack.fill(hess);
}

void VStack::optimizeAlignment(){}

Idx VStack::getNNZ_Jac(){
    return jac_stack.length();
}

vector<Idx> VStack::getJacEntries(){
    vector<Idx> jac_entries(jac_stack.length());
    for (Idx i=1; i<=jac_stack.length(); i++){
        jac_entries[i-1] = jac_stack.getStackElemId(i);
    }
    return jac_entries;
}

vector<HessPair> VStack::getHessEntries(){
    vector<HessPair> hess_entries(hess_stack.length());
    for (Idx i=1; i<=hess_stack.length(); i++){
        hess_entries[i-1] = hess_stack.getStackElemId(i);
    }
    return hess_entries;
}

void VStack::setX(const double* xx, const Idx& size){
    x = xx;
    x_size = size;
    jac_stack.setXSize(size);
    hess_stack.setXSize(size);
}

void VStack::ensureElem(){
    if (g_stack_end == g_stack.size())
        g_stack.resize(g_stack_end+1);
}

}

