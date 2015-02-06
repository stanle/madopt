/* * Copyright 2014 National ICT Australia Limited (NICTA)
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

#include "hessstack.hpp"
#include "jacstack.hpp"
#include "logger.hpp"

namespace MadOpt {
void HessStack::mergeJacInto(JacStack& other){
    auto& other_stack = other.getStack();

    for (Idx i=other.getPrev(); i<other.getLast(); i++){
        auto& elem2 = other_stack[i];
        for (Idx k=other.getLast(); k<other.getEnd(); k++){
            auto& elem1 = other_stack[k];

            double v = elem1.value * elem2.value;
            if (elem1.id == elem2.id)
                v *= 2;
            insertOrUpdatePair(elem1.id, elem2.id, v);
        }
    }
}

void HessStack::mergeSingle(JacStack& other, 
        const double& jac_value, 
        const double& hess_value){
    auto& other_stack = other.getStack();

    for (Idx i=getLast(); i<stack_end; i++)
        stack[i].value *= jac_value;

    for (Idx i=other.getLast(); i<other.getEnd(); i++){
        auto& elem2 = other_stack[i];
        for (Idx k=i; k<other.getEnd(); k++){
            auto& elem1 = other_stack[k];

            double v = elem1.value * elem2.value * hess_value;
            insertOrUpdatePair(elem1.id, elem2.id, v);

        }
    }
}

void HessStack::clearLastStackPos(){
    for (Idx i=1; i<stack_end; i++)
        last_pos_map[stack[i].id] = 0;
}

void HessStack::setLastStackPos(const HessPair& id, const Idx& conflict){
    last_pos_map[id] = conflict;
}

void HessStack::insertOrUpdatePair(const Idx& frst, const Idx& scd, const double& value){
    auto p = uPII(frst, scd);
    Idx& conflict = last_pos_map[p];

    if (conflict >= getLast())
        stack[conflict].value += value;
    else {
        if (stack_end == stack.size())
            stack.resize(stack_end+1);
        auto& new_elem = stack[stack_end];
        new_elem.id = p;
        new_elem.value = value;
        new_elem.conflict = conflict;
        conflict = stack_end;
        stack_end++;
    }
}

void HessStack::setXSize(const Idx& size){
    last_pos_map.resize(size);
}

}

