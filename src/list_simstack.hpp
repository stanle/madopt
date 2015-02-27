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

#ifndef MADOPT_LISTSIMSTACK
#define MADOPT_LISTSIMSTACK

#include <string.h>

#include "common.hpp"
#include "array.hpp"
#include "logger.hpp"

namespace MadOpt {

template<class T>
class ListSimStack{
    public:
        ListSimStack():stack(1), _max_size(0){}

        virtual ~ListSimStack(){}

        struct ListSimStackElem {
            T id;
            Idx conflict;
        };

        void merge(const Idx& nofelems){
            TRACE_START;
            ASSERT_LE(2, nofelems);
            ASSERT_LE(nofelems, positions.size());
            ASSERT_LE(positions.back(), stack.size());
            Idx conflict_counter_pos = conflicts->getEndPosAndPush();
            (*conflicts)[conflict_counter_pos] = 0;
            const Idx& prev_start = positions.back(nofelems);
            const Idx& last_start = positions.back(nofelems-1);
            TRACE(str());
            TRACE("nof elems to merge=", nofelems,
                    "prev begin=", prev_start,
                    "last begin=", last_start);
            ASSERT_BETWEEN(1, prev_start, stack.size());
            ASSERT_BETWEEN(1, last_start, stack.size());
            for (Idx i=stack.size()-1; i>=last_start; i--){
                auto& elem = stack[i];
                if (prev_start <= elem.conflict){
                    (*conflicts)[conflict_counter_pos]++;
                    ASSERT_LE(1, (*conflicts)[conflict_counter_pos]);
                    ASSERT_BETWEEN(1, elem.conflict, stack.size()-2);
                    ASSERT_BETWEEN(1, i, stack.size()-1);
                    ASSERT_LE(elem.conflict, i);
                    conflicts->push(elem.conflict);
                    conflicts->push(i);
                    TRACE("ins conf", 
                            "counter=", (*conflicts)[conflict_counter_pos], 
                            "to=", elem.conflict, 
                            "form=", i, 
                            "nof confs=", conflicts->size());
                    setLastStackPos(elem.id, elem.conflict);
                    if (stack.size() != i){
                        elem = stack.pop();
                        setLastStackPos(elem.id, i);
                    }
                }
            }
            positions.pop(nofelems-1);
            ASSERT_UEQ(positions.size(), 0);
            TRACE(str());
            TRACE_END;
        }

        void emplace_back_empty(){
            positions.push(stack.size());
            ASSERT_LE(positions.back(), stack.size());
        }

        void clear(){
            clearLastStackPos();
            stack.clear();
            stack.getEndAndPushSave();
            positions.clear();
        }

        void setConflicts(Array<Idx>* c){
            conflicts = c;
        }

        const Array<ListSimStackElem>& getStack()const {
            return stack;
        }

        Array<ListSimStackElem>& getStack(){
            ASSERT_LE(positions.back(), stack.size());
            return stack;
        }

        const Array<Idx>& getPos()const {
            ASSERT_LE(positions.back(), stack.size());
            return positions;
        }

        const Idx& max_size()const {
            return _max_size;
        }

        virtual string str(){
            if (positions.size() == 0)
                return "-";
            string res;
            for (Idx i=0; i<positions.size()-1; i++){
                for (Idx k=positions[i]; k<positions[i+1]; k++)
                    res += to_string(stack[k].id) + "," + to_string(stack[k].conflict) + " ";
                res += "|";
            }
            for (Idx k=positions.back(); k<stack.size(); k++){
                res += to_string(stack[k].id) + "," + to_string(stack[k].conflict) + " ";
            }
            res += "|";
            return res;
        }

    protected:
        Array<ListSimStackElem> stack;
        Array<Idx> positions;
        Array<Idx>* conflicts;

        Idx _max_size;

        virtual void setLastStackPos(const T& id, const Idx& conflict)=0;
        virtual void clearLastStackPos()=0;
        virtual void setXSize(const Idx&)=0;
};
}
#endif

