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
#ifndef MADOPT_VSTACK
#define MADOPT_VSTACK

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

        void mulAll(const double& value){
            for (Idx i=getLast(); i<stack_end; i++)
                stack[i].value *= value;
        }

        void emplace_back(const T& id, const double& value){
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

        virtual Idx getAndUpdateLastStackPos(const T& id, const Idx& new_conflict_pos)=0;
        virtual void setLastStackPos(const T& id, const Idx& conflict)=0;
        virtual void clearLastStackPos()=0;

};

class JacListStack : public ListStack<Idx> {
    private:
        vector<Idx> last_pos_map;

        void clearLastStackPos(){
            for (Idx i=1; i<stack_end; i++){
                last_pos_map[stack[i].id] = 0;
            }
        }

        Idx getAndUpdateLastStackPos(const Idx& id, const Idx& new_conflict_pos){
            if (id >= last_pos_map.size())
                last_pos_map.resize(id+1, 0);
            Idx conflict=last_pos_map[id];
            last_pos_map[id] = new_conflict_pos;
            return conflict;
        }

        virtual void setLastStackPos(const Idx& id, const Idx& conflict){
            if (id >= last_pos_map.size())
                last_pos_map.resize(id+1, 0);
            last_pos_map[id] = conflict;
        }
};

typedef pair<Idx, Idx> HessPair;

class HessListStack : public ListStack<HessPair> {
    public:
        void mergeJacInto(JacListStack& other){
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

        void mergeSingle(JacListStack& other, 
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

    private:
        vector<map<Idx, Idx> > last_pos_map;

        void clearLastStackPos(){
            for (Idx i=1; i<stack_end; i++){
                auto& id = stack[i].id;
                last_pos_map[id.first][id.second] = 0;
            }
        }

        Idx getLastStackPos(const HessPair& id){
            ensureMapPosElem(id);
            auto& frstmap = last_pos_map[id.first];
            auto it = frstmap.find(id.second);
            if (it != frstmap.end())
                return it->second;
            return 0;
        }

        Idx getAndUpdateLastStackPos(const HessPair& id, const Idx& new_conflict_pos){
            Idx conflict=0;
            ensureMapPosElem(id);
            auto& frstmap = last_pos_map[id.first];
            auto it = frstmap.find(id.second);
            if (it != frstmap.end()){
                conflict = it->second;
                it->second = new_conflict_pos;
            } else 
                frstmap[id.second] = new_conflict_pos;

            return conflict;
        }

        void setLastStackPos(const HessPair& id, const Idx& conflict){
            ensureMapPosElem(id);
            last_pos_map[id.first][id.second] = conflict;
        }

        void insertOrUpdatePair(const Idx& frst, const Idx& scd, const double& value){
            auto p = uPII(frst, scd);
            Idx conflict = getLastStackPos(p);

            if (conflict >= getLast())
                stack[conflict].value += value;
            else {
                if (stack_end == stack.size())
                    stack.resize(stack_end+1);
                setLastStackPos(p, stack_end);
                auto& new_elem = stack[stack_end];
                new_elem.id = p;
                new_elem.value = value;
                new_elem.conflict = conflict;
                stack_end++;
            }
        }

        void ensureMapPosElem(const HessPair& id){
            if (last_pos_map.size() <= id.first)
                last_pos_map.resize(id.first+1);
        }

        Idx getMapPos(const HessPair& id){
            return id.first;
        }
};

class VStack {
    public:
        VStack()
        {}

        void doADD(Idx nofelems){
            TRACE_START;
            ASSERT_LE(nofelems, size());
            jac_stack.merge(nofelems);
            hess_stack.merge(nofelems);

            for (Idx i=0; i<nofelems-1; i++){
                double last = g_stack.back();
                g_stack.pop_back();
                g_stack.back() += last;
            }

            ASSERT_EQ(size(), jac_stack.size());
            ASSERT_EQ(size(), hess_stack.size());
            TRACE_END;
        }

        void doMUL(){
            TRACE_START;
            ASSERT_LE(1, size());
            double last = g_stack.back();
            g_stack.pop_back();
            double& prev = g_stack.back();
            hess_stack.merge(prev, last);
            hess_stack.mergeJacInto(jac_stack);
            jac_stack.merge(prev, last);
            prev *= last;
            ASSERT_EQ(size(), jac_stack.size());
            ASSERT_EQ(size(), hess_stack.size());
            TRACE_END;
        }

        double& lastG(){
            return g_stack.back();
        }

        void doUnaryOp(const double& jac_value, const double& hess_value){
            TRACE_START;
            hess_stack.mergeSingle(jac_stack, jac_value, hess_value);
            jac_stack.mulAll(jac_value);
            ASSERT_EQ(size(), jac_stack.size());
            ASSERT_EQ(size(), hess_stack.size());
            TRACE_END;
        }

        void emplace_back(Idx& id, const double& value){
            TRACE_START;
            DEBUG_CODE(Idx s = size());
            g_stack.emplace_back(value);
            jac_stack.emplace_back(id, 1);
            hess_stack.emplace_back_empty();
            ASSERT_EQ(size(), s+1);
            ASSERT_EQ(size(), jac_stack.size());
            ASSERT_EQ(size(), hess_stack.size());
            TRACE_END;
        }

        void emplace_backSQR(Idx& id, const double& value){
            TRACE_START;
            DEBUG_CODE(Idx s = size());
            g_stack.emplace_back(value*value);
            jac_stack.emplace_back(id, 2*value);
            hess_stack.emplace_back(HessPair(id, id), 2);
            ASSERT_EQ(size(), s+1);
            ASSERT_EQ(size(), jac_stack.size());
            ASSERT_EQ(size(), hess_stack.size());
            TRACE_END;
        }

        void emplace_back(const double& value){
            TRACE_START;
            DEBUG_CODE(Idx s = size());
            g_stack.emplace_back(value);
            jac_stack.emplace_back_empty();
            hess_stack.emplace_back_empty();
            ASSERT_EQ(size(), s+1);
            ASSERT_EQ(size(), jac_stack.size());
            ASSERT_EQ(size(), hess_stack.size());
            TRACE_END;
        }

        void clear(){
            TRACE_START;
            ASSERT_EQ(g_stack.size(), jac_stack.size());
            ASSERT_EQ(g_stack.size(), hess_stack.size());
            jac_stack.clear();
            hess_stack.clear();
            g_stack.clear();
            data_i = 0;
            ASSERT_EQ(g_stack.size(), jac_stack.size());
            ASSERT_EQ(g_stack.size(), hess_stack.size());
            TRACE_END;
        }

        Idx size(){
            ASSERT_EQ(g_stack.size(), jac_stack.size());
            ASSERT_EQ(g_stack.size(), hess_stack.size());
            return g_stack.size();
        }

        void fill(double* jac, double* hess){
            jac_stack.fill(jac);
            hess_stack.fill(hess);
        }

        void optimizeAlignment(){}

        Idx getNNZ_Jac(){
            return jac_stack.length();
        }

        Idx data_i=0;
        const double* x;

        HessListStack hess_stack;
        JacListStack jac_stack;
    private:
        vector<double> g_stack;
};
}
#endif
