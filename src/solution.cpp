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

#include "solution.hpp"
#include "exceptions.hpp"
//#include "common.hpp"

namespace MadOpt {

void Solution::set(const SolverStatus status,
        const Idx x_size, const Idx l_size, 
        const double obj_value, 
        const double* x, const double* lambda){
    lambda_loaded = false;
    _status = status;

    if (hasSolution()){
        _obj_value = obj_value;
        this->_x.resize(x_size);
        for (Idx i=0; i<x_size; i++)
            this->_x[i] = x[i];

        if (l_size > 0){
            _l.resize(l_size);
            for (Idx i=0; i<ng(); i++)
                _l[i] = lambda[i];
            lambda_loaded = true;
        }
    }
}

void Solution::set(const Solution::SolverStatus status,
        const Idx x_size, 
        const double obj_value, 
        const double* x){
    set(status, x_size, 0, obj_value, x, nullptr);
}

Idx Solution::nx()const {
    return _x.size(); 
}

Idx Solution::ng()const {
    return _l.size(); 
}

double Solution::x(const Idx idx)const { 
    if (!hasSolution())
        throw MadOptError("trying to access solution but non is loaded");
    assert(_x.size() > idx);
    return _x[idx]; 
}

double Solution::lam(const Idx idx)const { 
    if (!lambda_loaded)
        throw MadOptError("trying to access lambdas but non are loaded");
    assert(_l.size() > idx);
    return _l[idx]; 
}

Solution::SolverStatus Solution::status()const { 
    //if (_status == SolverStatus::NO_RUN)
    //    throw MadOptError("no call to solve so far");
    return _status; 
}

double Solution::obj_value()const { 
    if (!hasSolution())
        throw MadOptError("trying to access solution objective value but non is loaded");
    return _obj_value; 
}

bool Solution::hasSolution() const{
    return (_status == SolverStatus::SUCCESS 
            || _status == SolverStatus::STOP_AT_ACCEPTABLE_POINT);
}

}
