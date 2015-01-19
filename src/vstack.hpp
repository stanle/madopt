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

#include "common.hpp"

namespace MadOpt {

using namespace std;

typedef pair<size_t, size_t> MergePair;
typedef pair<size_t, size_t> HessPair;

class VStack {
    public:
        VStack(): jac_stack_after_last(0), hess_stack_after_last(0), stack_after_last(0)
        {}

        void clear(){
            jac_stack_after_last = 0;
            hess_stack_after_last = 0;
            stack_after_last = 0;
        }

        void mergeLastTwoHess(const double value=1, const double scn_value=1){
            tmp_hess_map.clear();
            size_t hess_frstlastpos = stack[hess_stack_after_last-1].hess_stack_pos;
            size_t hess_scndlastpos = stack[hess_stack_after_last-2].hess_stack_pos;
            for (size_t i=hess_scndlastpos; i<hess_frstlastpos; i++){
                HessStackElem& e = hess_stack[i];
                e.value *= scn_value;
                tmp_hess_map[e.idx] = i;
            }

            for (size_t i=hess_stack_after_last-1; i>=hess_frstlastpos; i--){
                HessStackElem& e = hess_stack[i];
                e.value *= value;
                auto it = tmp_hess_map.find(e.idx);
                if (it != tmp_hess_map.end()){
                    hess_stack[it->second].value += e.value;
                    hess_stack[i] = hess_stack[--hess_stack_after_last];
                }
            }
        }

        void mergeLastTwoJac(const double value=1, const double scn_value=1){
            tmp_jac_map.clear();
            size_t jac_frstlastpos = stack[jac_stack_after_last-1].jac_stack_pos;
            size_t jac_scndlastpos = stack[jac_stack_after_last-2].jac_stack_pos;
            for (size_t i=jac_scndlastpos; i<jac_frstlastpos; i++){
                JacStackElem& e = jac_stack[i];
                if (scn_value != 1)
                    e.value *= scn_value;
                tmp_jac_map[e.idx] = i;
            }

            for (size_t i=jac_stack_after_last-1; i>=jac_frstlastpos; i--){
                JacStackElem& e = jac_stack[i];
                if (value != 1)
                    e.value *= value;
                auto it = tmp_jac_map.find(e.idx);
                if (it != tmp_jac_map.end()){
                    jac_stack[it->second].value += e.value;
                    jac_stack[i] = jac_stack[--jac_stack_after_last];
                }
            }
        }

        void mergeLastTwoJacsTwoHess(){
            tmp_hess_map.clear();
            size_t hess_frstlastpos = stack[hess_stack_after_last-1].hess_stack_pos;
            for (size_t i=hess_stack_after_last-1; i>=hess_frstlastpos; i--){
                HessStackElem& e = hess_stack[i];
                tmp_hess_map[e.idx] = i;
            }

            size_t jac_frstlastpos = stack[jac_stack_after_last-1].jac_stack_pos;
            size_t jac_scndlastpos = stack[jac_stack_after_last-2].jac_stack_pos;

            double res = 1;

            for (size_t i=jac_scndlastpos; i<jac_frstlastpos; i++){
                JacStackElem& e = jac_stack[i];
                for (size_t j=jac_stack_after_last-1; j>=jac_frstlastpos; j--){
                    JacStackElem& f = jac_stack[j];
                    PII p(uPII(e.idx, f.idx));

                    res = e.value * f.value;
                    if (e.idx == f.idx)
                        res *= 2;

                    auto it = tmp_hess_map.find(p);
                    if (it == tmp_hess_map.end()){
                        ensureHessStackElem();
                        hess_stack[hess_stack_after_last].idx = p;
                        hess_stack[hess_stack_after_last].value = res;
                        tmp_hess_map[p] = hess_stack_after_last;
                        hess_stack_after_last++;
                    } else {
                        hess_stack[it->second].value += res;

                    }
                }
            }
        }

        void mulAll(const double& v){
            size_t jac_frstlastpos = stack[jac_stack_after_last-1].jac_stack_pos;
            for (size_t i=jac_stack_after_last-1; i>=jac_frstlastpos; i--){
                jac_stack[i].value *= v;
            }
        }

        void mergeJacTwoHess(const double& jac_value, const double& hess_value){
            tmp_hess_map.clear();
            size_t hess_frstlastpos = stack[hess_stack_after_last-1].hess_stack_pos;
            for (size_t i=hess_stack_after_last-1; i>=hess_frstlastpos; i--){
                HessStackElem& e = hess_stack[i];
                tmp_hess_map[e.idx] = i;
            }

            size_t jac_frstlastpos = stack[jac_stack_after_last-1].jac_stack_pos;

            double res = 1;

            for (size_t i=jac_stack_after_last-1; i>=jac_frstlastpos; i--){
                JacStackElem& e = jac_stack[i];
                for (size_t j=jac_stack_after_last-1; j>=jac_frstlastpos; j--){
                    JacStackElem& f = jac_stack[j];
                    PII p(uPII(e.idx, f.idx));

                    res = e.value * f.value * jac_value;
                    if (e.idx == f.idx)
                        res *= 2;

                    auto it = tmp_hess_map.find(p);
                    if (it == tmp_hess_map.end()){
                        ensureHessStackElem();
                        hess_stack[hess_stack_after_last].idx = p;
                        hess_stack[hess_stack_after_last].value = res;
                        tmp_hess_map[p] = hess_stack_after_last;
                        hess_stack_after_last++;
                    } else {
                        hess_stack[it->second].value *= hess_value;
                        hess_stack[it->second].value += res;
                    }
                }
            }

        }

        void mergeLastTwoG(){

        }

        double& lastG(){
            return stack[stack_after_last-1].g;
        }

        void emplace_back(Idx& idx, const double& value){
            ensureJackStackElem();
            VStackElem& e = stack[stack_after_last++];
            e.g = value;
            e.jac_stack_pos = jac_stack_after_last;
            jac_stack[jac_stack_after_last].idx = idx;
            jac_stack[jac_stack_after_last].value = 1;
            jac_stack_after_last++;
            e.hess_stack_pos = hess_stack_after_last;
        }

        void emplace_backSQR(Idx& idx, const double& value){
            ensureJackStackElem();
            VStackElem& e = stack[stack_after_last++];
            e.g = value;
            e.jac_stack_pos = jac_stack_after_last;
            jac_stack[jac_stack_after_last].idx = idx;
            jac_stack[jac_stack_after_last].value = 2*value;
            jac_stack_after_last++;
            e.hess_stack_pos = hess_stack_after_last;
            hess_stack[hess_stack_after_last].idx = PII(idx, idx);
            hess_stack[hess_stack_after_last].value = 2;
            hess_stack_after_last++;
        }

        void emplace_back(const double& value){
            ensureJackStackElem();
            VStackElem& e = stack[stack_after_last++];
            e.g = value;
            e.jac_stack_pos = jac_stack_after_last;
            e.hess_stack_pos = hess_stack_after_last;
        }

        Idx data_i=0;
        double* x=nullptr;

    private:
        struct VStackElem {
            double g;
            size_t jac_stack_pos;
            size_t hess_stack_pos;
        };

        struct JacStackElem {
            double value;
            size_t idx;
        };

        struct HessStackElem {
            double value;
            HessPair idx;
        };

        vector<JacStackElem> jac_stack;
        vector<HessStackElem> hess_stack;
        vector<VStackElem> stack;

        size_t jac_stack_after_last;
        size_t hess_stack_after_last;
        size_t stack_after_last;

        unordered_map<size_t, size_t> tmp_jac_map;
        unordered_map<HessPair, size_t> tmp_hess_map;

        unordered_map<size_t, vector<size_t> > jac_map;
        unordered_map<HessPair, vector<size_t> > hess_map;

        void ensureHessStackElem(){
            if (hess_stack_after_last == hess_stack.size()){
                hess_stack.resize(hess_stack.size()+1);
            }
        }

        void ensureJackStackElem(){
            if (jac_stack_after_last == jac_stack.size()){
                jac_stack.resize(jac_stack.size()+1);
            }
        }
};

}

#endif
