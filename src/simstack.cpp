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

#include "simstack.hpp"
#include "logger.hpp"

namespace MadOpt {

void SimStack::doAdd(const Idx& nofelems){
    TRACE_START;
    ASSERT_LE(nofelems, size());
    jac_stack.merge(nofelems);
    hess_stack.merge(nofelems);
    _size -= nofelems-1;
    TRACE(str());
    TRACE_END;
}

void SimStack::doMull(){
    TRACE_START;
    ASSERT_LE(2, size());
    hess_stack.setJac(jac_stack);
    hess_stack.merge(2);
    jac_stack.merge(2);
    _size -= 1;
    TRACE(str());
    TRACE_END;
}

double& SimStack::lastG(){
    return dummy;
}

void SimStack::doUnaryOp(const double& jac_value, const double& hess_value){
    TRACE_START;
    hess_stack.emplace_back_empty();
    hess_stack.setSingleJac(jac_stack);
    hess_stack.merge(2);
    TRACE(str());
    TRACE_END;
}

void SimStack::emplace_back(const Idx& id){
    TRACE_START;
    ASSERT(id >= 0);
    _size += 1;
    if (_size > _max_size)
        _max_size = _size;
    jac_stack.emplace_back(id);
    hess_stack.emplace_back_empty();
    ASSERT_UEQ(hess_stack.getPos().size(), 0);
    TRACE(id, str());
    TRACE_END;
}

void SimStack::emplace_back(const double& value){
    TRACE_START;
    _size += 1;
    if (_size > _max_size)
        _max_size = _size;
    jac_stack.emplace_back_empty();
    hess_stack.emplace_back_empty();
    ASSERT_UEQ(hess_stack.getPos().size(), 0);
    TRACE(str());
    TRACE_END;
}

void SimStack::clear(){
    TRACE_START;
    _size = 0;
    data_i = 0;
    jac_stack.clear();
    hess_stack.clear();
    TRACE(str());
    TRACE_END;
}

vector<Idx> SimStack::getJacEntries(){
    TRACE_START;
    vector<Idx> res;
    const auto& stack = jac_stack.getStack();
    const auto& pos = jac_stack.getPos();
    ASSERT_UEQ(pos.size(), 0);
    for (Idx i=pos.back(); i<stack.size(); i++)
        res.push_back(stack[i].id);
    TRACE_END;
    return res;
}

vector<PII> SimStack::getHessEntries(){
    TRACE_START;
    vector<PII> res;
    const auto& stack = hess_stack.getStack();
    const auto& pos = hess_stack.getPos();
    ASSERT_UEQ(pos.size(), 0);
    for (Idx i=pos.back(); i<stack.size(); i++)
        res.push_back(stack[i].id);
    TRACE_END;
    return res;
}

Idx SimStack::size(){
    return _size;
}

void SimStack::setConflicts(Array<Idx>* conflicts){
    jac_stack.setConflicts(conflicts);
    hess_stack.setConflicts(conflicts);
}

const Idx& SimStack::max_g_size()const {
    return _max_size;
}

const Idx& SimStack::max_jac_size()const{
    return jac_stack.max_size();
}

const Idx& SimStack::max_hess_size()const{
    return hess_stack.max_size();
}

void SimStack::setXSize(const Idx& size){
    TRACE_START;
    jac_stack.setXSize(size);
    hess_stack.setXSize(size);
    TRACE_END;
}

string SimStack::str(){
    string res;
    res += "size=" + to_string(_size);
    res += " Jac=" + jac_stack.str();
    res += " Hess=" + hess_stack.str();
    return res;
}

Idx& SimStack::getDataI(){
    return data_i;
}

}
