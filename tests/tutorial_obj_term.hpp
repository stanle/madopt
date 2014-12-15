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
#ifndef MADOPT_TUTORIAL_OBJ_CONSTRAINT_H
#define MADOPT_TUTORIAL_OBJ_CONSTRAINT_H

#include "../src/inner_constraint.hpp"

using namespace MadOpt;

class TutorialObjTerm: public InnerConstraint {
    public:
        TutorialObjTerm(int N, int offset): N(N), offset(offset){
            jac.resize(N);
        }

        Idx getNNZ_Jac(){return N;}

        double lb() { return 0; }
        void lb(double v){}
        double ub() { return 0; }
        void ub(double v){}

        vector<Idx> getJacEntries(){
            vector<Idx> jac_entries(N);
            for (int i=0; i<N; i++)
                jac_entries[i] = i+offset;
            return jac_entries;
        }

        vector<PII> getHessEntries(){
            vector<PII> hess_entries(N);
            for (int i=0; i<N; i++)
                hess_entries[i] = PII(i+offset, i+offset);
            return hess_entries;
        }

        void setEvals(const double* x){
            double tmp;
            g = 0;
            for (int i=0; i<N; i++){
                tmp = x[i+offset]-1;
                jac[i] = 2*tmp;
                g += pow(tmp, 2);
            }
        }

        void eval_h(const double* x, bool new_x, double* values, const double& lambda){
            if (new_x)
                setEvals(x);
            for (int i=0; i<N; i++)
                values[hess_map[i]] += lambda * 2.;
        }

        const vector<double>& getJac()const {return jac;}
        const double& getG()const {return g;}

        const string toString()const { return "TUT OBJ"; }

    private:
        int N;
        int offset;
};
#endif
