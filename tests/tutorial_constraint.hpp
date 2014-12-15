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
#ifndef MADOPT_TUTORIAL_CONSTRAINT_H
#define MADOPT_TUTORIAL_CONSTRAINT_H


#define i0 offset
#define i1 offset+1
#define i2 offset+2

#include "../src/inner_constraint.hpp"
#include <cmath>

using namespace MadOpt;

class TutorialConstraint: public InnerConstraint {
    public:
        TutorialConstraint(int offset, double a): 
            offset(offset), a(a) {
                jac.resize(3);
                hess.resize(3);
            }

        Idx getNNZ_Jac(){return 3;}

        double lb(){ return 0; }
        void lb(double v){ assert(false); }
        double ub(){ return 0; }
        void ub(double v){ assert(false); }

        vector<Idx> getJacEntries(){
            vector<Idx> jac_entries(3);
            jac_entries[0] = i0;
            jac_entries[1] = i1;
            jac_entries[2] = i2;
            return jac_entries;
        }

        vector<PII> getHessEntries(){
            vector<PII> hess_entries(3);
            hess_entries[0] = PII(i1, i1);
            hess_entries[1] = PII(i1, i2);
            hess_entries[2] = PII(i2, i2);
            return hess_entries;
        }

        void setEvals(const double* x){
            double vx = x[i1];
            double vy = x[i2];
            double vz = x[i0];
            double co = cos(vy);
            double si = sin(vy);
            double sq = pow(vx, 2);

            double t1 = sq + 1.5*vx - a;
            double t2 = 2*vx + 1.5;
            double t3 = t1 * co;

            jac[0] = -1;
            jac[1] = t2 * co;
            jac[2] = -t1 * si;
            hess[0] = 2 * co;
            hess[1] = -t2 * si;
            hess[2] = -t3;
            g = t3 - vz;
        }

        const double& getG()const { return g; }
        const vector<double>& getJac()const { return jac; }
        const vector<double>& getHess()const { return hess; }

        const string toString()const { return "TUT"; }

    private:
        int offset;
        double a;
};
#endif
