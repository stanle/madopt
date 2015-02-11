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
#ifndef MADOPT_LISTSTACK
#define MADOPT_LISTSTACK

#include <vector>
#include <unordered_map>
#include <map>
#include <string.h>

#include "logger.hpp"
#include "common.hpp"

namespace MadOpt {

using namespace std;

template<class T>
class ListStack {
    public:
        struct StackElem {
            T id;
            double value;
            Idx conflict;

            string toString(){
                return to_string(id) + 
                    "=" + std::to_string(value) + "/" + to_string(conflict);
            }
        };

        ListStack(): stack(1), positions(0), stack_end(1), positions_end(0){}

        virtual ~ListStack(){}

        void merge(Idx nofelems){
            const Idx& prev_start = getPrev(nofelems-1);
            const Idx& last_start = getPrev(nofelems-2);
            for (Idx i=stack_end-1; i>=last_start; i--){
                if (prev_start <= stack[i].conflict)
                    solveConflict(i);
            }
            positions_end -= nofelems-1;
        }

        void merge(const double mul_with_last, 
                const double mul_with_prev){
            TRACE_START;
            const Idx& prev_start = getPrev();
            const Idx& last_start = getLast();

            for (Idx i=prev_start; i<last_start; i++)
                stack[i].value *= mul_with_prev;

            for (Idx i=stack_end-1; i>=last_start; i--){
                StackElem& elem = stack[i];
                elem.value *= mul_with_last;
                if (prev_start <= elem.conflict)
                    solveConflict(i);
            }
            positions_end--;
            TRACE_END;
        }

        const vector<StackElem>& getStack(){
            return stack;
        }

        const Idx& getEnd(){
            return stack_end;
        }

        const Idx& getLast(){
            return positions[positions_end-1];
        }

        const Idx& getPrev(Idx pos=1){
            return positions[positions_end-1-pos];
        }

        void emplace_back_empty(){
            if (positions_end == positions.size())
                positions.resize(positions.size()+1);
            positions[positions_end] = stack_end;
            positions_end++;
        }

        Idx size(){
            return positions_end;
        }

        T& getStackElemId(const Idx& i){
            return stack[i].id;
        }

        void fill(double* data){
            ASSERT_EQ(size(), 1);
            ASSERT_EQ(getLast(), 1);
            for (Idx i=getLast(); i<stack_end; i++){
                auto& elem = stack[i];
                data[i-1] = elem.value;
            }
        }

        Idx length(){
            return stack_end-1;
        }

        string toString(){
            string res;
            for (Idx i=0; i<positions_end; i++){
                Idx a = positions[i];
                Idx b = stack_end;
                if (i+1 != positions_end)
                    b = positions[i+1];
                for (Idx k=a; k<b; k++)
                    res += to_string(k) + ":" + stack[k].toString() + "\n";
            }
            return res;
        }

        void clear(){
            clearLastStackPos();
            stack_end = 1;
            positions_end = 0;
        }

        virtual void setXSize(const Idx& size)=0;

    protected:
        vector<StackElem> stack;
        vector<Idx> positions;
        Idx stack_end;
        Idx positions_end;

        void solveConflict(Idx i){
            auto& elem = stack[i];
            stack[elem.conflict].value += elem.value;
            setLastStackPos(elem.id, elem.conflict);
            stack_end--;
            if (stack_end != i){
                elem = stack[stack_end];
                setLastStackPos(elem.id, i);
            }
        }

        virtual void setLastStackPos(const T& id, const Idx& conflict)=0;
        virtual void clearLastStackPos()=0;

};
}
#endif
