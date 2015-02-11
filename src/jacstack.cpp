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

#include "jacstack.hpp"

namespace MadOpt {
void JacStack::emplace_back(const Idx& id, const double& value){
    if (stack_end == stack.size())
        stack.resize(stack.size()+1);
    StackElem& elem = stack[stack_end];
    elem.id = id;
    elem.value = value;
    elem.conflict = getAndUpdateLastStackPos(id, stack_end);
    if (positions_end == positions.size())
        positions.resize(positions.size()+1);
    positions[positions_end] = stack_end;
    positions_end++;
    stack_end++;
}

void JacStack::clearLastStackPos(){
    for (Idx i=1; i<stack_end; i++){
        last_pos_map[stack[i].id] = 0;
    }
}

Idx JacStack::getAndUpdateLastStackPos(const Idx& id, const Idx& new_conflict_pos){
    Idx conflict=last_pos_map[id];
    last_pos_map[id] = new_conflict_pos;
    return conflict;
}

void JacStack::setLastStackPos(const Idx& id, const Idx& conflict){
    last_pos_map[id] = conflict;
}

void JacStack::setXSize(const Idx& size){
    last_pos_map.resize(size);
}

void JacStack::mulAll(const double& value){
    for (Idx i=getLast(); i<stack_end; i++)
        stack[i].value *= value;
}


}
