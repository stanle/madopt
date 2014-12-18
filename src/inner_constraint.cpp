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

#include "inner_constraint.hpp"
#include "logger.hpp"
#include "solution.hpp"

namespace MadOpt {

InnerConstraint::~InnerConstraint(){}

void InnerConstraint::eval_h(double* values, const double& lambda){
    ASSERT_EQ(hess.size(), hess_map.size());
    for (Idx i=0; i<hess.size(); i++){
        VALGRIND_CONDITIONAL_JUMP_TEST(hess[i]);
        VALGRIND_CONDITIONAL_JUMP_TEST(hess_map[i]);
        values[hess_map[i]] += lambda * hess[i];
    }
}

void InnerConstraint::lb(double v){
    assert(false); 
}

void InnerConstraint::ub(double v){
    assert(false); 
}

Idx InnerConstraint::getNNZ_Jac(){
    return getJacEntries().size(); 
}

void InnerConstraint::getNZ_Jac(unsigned int* jCol){
    auto jac_entries = getJacEntries();
    Idx nz = 0;
    for (auto idx: jac_entries){
        VALGRIND_CONDITIONAL_JUMP_TEST(idx);
        jCol[nz++] = idx;
    }
}

vector<PII> InnerConstraint::getHessEntries(){
    assert(false); 
}

void InnerConstraint::init(HessPosMap& hess_pos_map){
    hess_map.clear();
    auto hess_entries = getHessEntries();
    for (const auto& p: hess_entries){
        auto it = hess_pos_map.find(p);
        if (it == hess_pos_map.end())
            hess_pos_map[p] = hess_pos_map.size() - 1;
        hess_map.push_back(hess_pos_map[p]);
    }
}

void InnerConstraint::setPos(Idx epos) {
    pos = epos; 
}

Idx InnerConstraint::getPos()const {
    return pos; 
}


void InnerConstraint::setSolutionClass(Solution* sol){
    this->sol = sol; 
}


double InnerConstraint::lam()const {
    return sol->lam(pos); 
};


const double& InnerConstraint::getG()const { 
    return g; 
}

const vector<double>& InnerConstraint::getJac()const { 
    return jac;
}

const vector<double>& InnerConstraint::getHess()const {
    return hess;
}

const vector<Idx>& InnerConstraint::getHessMap()const{
    return hess_map;
}

}
