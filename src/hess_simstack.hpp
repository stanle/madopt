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

#ifndef MADOPT_HESSIMSTACK
#define MADOPT_HESSIMSTACK

#include "common.hpp"
#include "list_simstack.hpp"
#include "jac_simstack.hpp"
#include "pairhashmap.hpp"
#include "logger.hpp"

namespace MadOpt {

class HessSimStack : public ListSimStack<PII> {
    public:

        void setJac(const JacSimStack& jac){
            TRACE_START;
            ASSERT_LE(positions.back(), stack.size());
            Idx conflict_counter_pos = conflicts->getEndPosAndPush();
            (*conflicts)[conflict_counter_pos] = 0;
            const auto& jac_stack = jac.getStack();
            const auto& pos = jac.getPos();
            ASSERT_BETWEEN(pos.back(2), pos.back(1), jac_stack.size(),
                    pos.back(2), pos.back(1), jac_stack.size() );
            for (Idx i=pos.back(1); i<jac_stack.size(); i++){
                const auto& elem2 = jac_stack[i];
                for (Idx k=pos.back(2); k<pos.back(); k++){
                    const auto& elem1 = jac_stack[k];
                    if (elem1.id == elem2.id){
                        (*conflicts)[conflict_counter_pos]++;
                        conflicts->push(stack.size());
                        ASSERT_LE(1, stack.size());
                        ASSERT_LE(1, (*conflicts)[conflict_counter_pos]);
                        TRACE("ins 00 conf", 
                                (*conflicts)[conflict_counter_pos], stack.size(),
                                conflicts->size());
                    }
                    TRACE("push new elem", i, k, elem1.id, elem2.id);
                    push(uPII(elem1.id, elem2.id));
                }
            }
            TRACE_END;
        }

        void setSingleJac(const JacSimStack& jac){
            TRACE_START;
            ASSERT_LE(positions.back(), stack.size());
            const auto& jac_stack = jac.getStack();
            for (Idx i=jac.getPos().back(); i<jac_stack.size(); i++)
                for (Idx k=i; k<jac_stack.size(); k++)
                    push(uPII(jac_stack[i].id, jac_stack[k].id));
            TRACE(str());
            TRACE_END;
        }

        void setXSize(const Idx& size){
            last_pos_map.resize(size);
        }

    private:
        PairHashMap last_pos_map;

        void setLastStackPos(const PII& id, const Idx& conflict){
            ASSERT_LE(conflict, stack.size()-1);
            last_pos_map[id] = conflict;
        }

        void clearLastStackPos(){
            for (Idx i=1; i<stack.size(); i++)
                last_pos_map[stack[i].id] = 0;
        }

        void push(const PII id){
            TRACE_START;
            ASSERT_LE(positions.back(), stack.size());
            auto& new_elem = stack.getEndAndPush();
            new_elem.id = id;
            auto& c = last_pos_map[new_elem.id];
            ASSERT_LE(c, stack.size()-1, last_pos_map.str());
            new_elem.conflict = c;
            c = stack.size()-1;
            TRACE(id, "old conf=", new_elem.conflict, "new conf=", c);
            if (stack.size() > _max_size)
                _max_size = stack.size();
            TRACE_END;
        }

};
}
#endif
