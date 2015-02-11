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

#ifndef MADOPT_LISTCSTACK
#define MADOPT_LISTCSTACK

#include "common.hpp"
#include "logger.hpp"
#include "array.hpp"

namespace MadOpt {

class ListCStack{
    public:
        ListCStack(): stack(1) {}

        void merge(const Idx& nofelems){
            TRACE_START;
            TRACE("conf elems", conflicts->str());
            Idx& counter = conflicts->next();
            for (Idx i=0; i<counter; i++){
                Idx& to = conflicts->next();
                Idx& from = conflicts->next();
                ASSERT_BETWEEN(1, to, stack.size()-2, to, from);
                ASSERT_BETWEEN(1, from, stack.size()-1);
                ASSERT_LE(to, from);
                TRACE("sol conf", to, from);
                stack[to] += stack[from];
                if (from != stack.size())
                    stack[from] = stack.pop();
            }
            positions.pop(nofelems-1);
            TRACE_END;
        }

        void mulAllLast(const double& value){
            for (Idx i=positions.back(1); i<stack.size(); i++)
                stack[i] *= value;
        }

        void mulAllPrev(const double& value){
            for (Idx i=positions.back(2); i<positions.back(); i++)
                stack[i] *= value;
        }

        void emplace_back_empty(){
            positions.pushSave(stack.size());
        }

        void clear(){
            stack.clear();
            stack.getEndAndPush();
            positions.clear();
        }

        void fill(double* data){
            TRACE_START;
            //ASSERT_XOR(stackSize() > 0, data != nullptr);
            ASSERT_EQ(positions.back(), 1);
            for (Idx i=1; i<stack.size(); i++){
                TRACE(i-1, stack[i]);
                data[i-1] = stack[i];
            }
            TRACE_END;
        }

        void setConflicts(Array<Idx>* c){
            //ASSERT(c != nullptr);
            conflicts = c;
        }

        void emplace_back(const double& value){
            TRACE_START;
            positions.pushSave(stack.size());
            stack.pushSave(value);
            TRACE(value, stack.size(), stack.back());
        }

        void resize(const Idx& stack_size, const Idx& pos_size){
            stack.resize(stack_size);
            positions.resize(pos_size);
        }

        const Array<double>& getStack()const {
            return stack;
        }

        Array<double>& getStack(){
            return stack;
        }

        const Array<Idx>& getPos()const {
            return positions;
        }

        void push(const double& value){
            TRACE_START;
            stack.pushSave(value);
            TRACE_END;
        }

        const Idx& size()const {
            return positions.size();
        }

        Idx stackSize()const {
            return stack.size()-1;
        }

        string str(){
            string res;
            for (Idx i=0; i<positions.size()-1; i++){
                for (Idx k=positions[i]; k<positions[i+1]; k++)
                    res += to_string(stack[k]) + " ";
                res += "|";
            }
            for (Idx k=positions.back(); k<stack.size(); k++)
                res += to_string(stack[k]) + " ";
            return res;
        }

    private:
        Array<double> stack;
        Array<Idx> positions;
        Array<Idx>* conflicts;
};
}
#endif
